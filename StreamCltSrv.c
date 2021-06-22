#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <libgen.h>
//#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "session.h"
#include "read_lib/read_lib.h"

#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
#define PAUSE(msg)	printf("%s [Appuyez sur entrée pour continuer]", msg); getchar();
#define MAX_BUFF 1024
#define IP_ANY		"0.0.0.0"
#define IP_LOOP		"127.0.0.1"
#define IP_LOCAL	"172.20.30.13"
#define IP_DISTANT	"172.20.30.19"
#define PORT_SRV	50000
#define MSG1		"Salut toi, le fond de l'eau est clair par ici !"
#define MSG2		"Salut toi, le fond de l'eau est brouillé par ici !"
#define OK			"OK"
#define NOK			"Pas OK"
#define BYE			"BYE"

#include "session.h"
typedef struct chatter {
    char nom[60];        /** numéro de requête -- toujours présent. Définit le type de requête (message ou type de commande) */

} chatter_t;

typedef struct chatter_tab {
    chatter_t chatters[70];
    int nbChatters;
} chatter_tab;

int sockEcoute;		// socket d'écoute de l'aaplication serveur
chatter_tab * chatterTab;
int nbChatters;
void installSigServer(int sigNum);
void client(char *adrIP, int port);
void serveur (char *adrIP, int port);
void dialSrv2Clt(int socketDial,chatter_tab * chatterTab);
void dialClt2srv(int socketAppel);
key_t genererCleChatters();
chatter_tab *recupererTabChatters(key_t cle2);
char * progName;

int main (__attribute__((unused)) int c, char **v) {
	progName = v[0];
#ifdef CLIENT
	if (c<3) {printf("usage %d: %s @IP port\n", c, basename(progName)); /*exit(-1);*/ client(IP_ANY, PORT_SRV);}
	else client(v[1], atoi(v[2]));
#endif

#ifdef SERVER
	installSigServer(SIGCHLD);
	installSigServer(SIGINT);
	if (c<3) {printf("usage: %s @IP port\n", basename(progName)); /*exit(-1);*/ serveur(IP_ANY, PORT_SRV);}
	else serveur(v[1], atoi(v[2]));
#endif
	return 0;
}
void creerProcService(int sockEcoute, int sockDial) { // TODO : A déplacer dans proto.c
	int pid;
    //chatterTab.nbChatters=0;
    key_t cleChatter;
    cleChatter=genererCleChatters();
    chatterTab=recupererTabChatters(cleChatter);
    //chatterTab->nbChatters=0;
	// Créer un processus de service pour l'affecter au service du client connecté
	CHECK(pid=fork(), "-- PB fork() --");
	if (pid == 0) {
		// Processus de service
		// fermer la socket d'écoute puisque le processus de service ne répond pas
		// aux requêtes de connexion
		CHECK(close(sockEcoute),"-- PB close() --");
		// Dialoguer avec le client
		dialSrv2Clt(sockDial,chatterTab);
		// Fermer la socket de dialogue
		CHECK(close(sockDial),"-- PB close() --");
		// Fin du processus de service
		exit(sockDial);
	}
}

void serveur (char *adrIP, int port) {

	int sockDial;	
	//struct sockaddr_in addrClt;	// adressage du client connecté

	sockEcoute = creerSocketEcoute(adrIP, port);
	while(1)	// daemon !
	{
		// Accepter une connexion
		sockDial = accepterClt(sockEcoute);
		// Lancer un processus de service pour le client connecté
		creerProcService(sockEcoute, sockDial);
		// SEUL LE SERVEUR execute la suite du code !
		// Fermer la socket de dialogue utilisé par le processus de service, elle est inutile pour le serveur
		CHECK(close(sockDial),"-- PB close() --");
	}
	CHECK(close(sockEcoute),"-- PB close() --");
}
void client (char *adrIP, int port) {
	int sockAppel;				// socket d'appel

	// Créer une connexion avec le serveur
	sockAppel = connecterClt2Srv (adrIP, port);
	// Dialoguer avec le serveur
	dialClt2srv(sockAppel);
	// Fermer la socket d'appel
	CHECK(close(sockAppel),"-- PB close() --");
	//PAUSE("CLIENT-close()");
}




void deroute (int sigNum) {
	int pidFils, status;
	switch (sigNum) {
	case SIGCHLD : CHECK(pidFils=wait(&status),"-- PB wait() --");
			printf("Fin du processus de service [%d], socket de dialogue n°%d\n", pidFils, WEXITSTATUS(status));
			break;
	case SIGINT	 : close(sockEcoute);
			printf("\nFin du serveur [%d] et de tous les processus de service\n", getpid());
			kill(SIGKILL, 0);
			exit(0);
			break;
	}
}

void installSigServer(int sigNum) {
	struct sigaction newAct;

	newAct.sa_handler = deroute;
	newAct.sa_flags = SA_RESTART;
	sigemptyset(&newAct.sa_mask) ;
	CHECK(sigaction(sigNum, &newAct, NULL),"-- PB sigaction() --");
}
