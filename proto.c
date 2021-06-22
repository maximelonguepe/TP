#include "data.h" // On suit l'ordre des couches de manière croissante
#include "proto.h"
#include <stdio.h>
#include <string.h>
#include "read_lib/read_lib.h"
// Correspond à la couche 7 du modèle OSI (couche Application)
#define OK            "OK"
#define BYE            "BYE"


// Traitement en fonction du type de requête reçue + fonctions venant de StreamCltSrv
void dialSrv2Clt(int socketDial,chatter_tab *  chatters) {

    char buff[MAX_BUFF];
    requete_t requete;
    do {
        memset(buff, 0, MAX_BUFF);
        //recevoirMessage(socketDial, buff, MAX_BUFF) ;
        recevoirRequete(socketDial, &requete);
        // appeler une fct avec le param buff pour connaître le numero de requete reçue
        // on réalise un switch sur ce numéro : à charque numéro correspond une fonction
        // de traitement qui génére un réponse
        // cette réponse sera envoyer après serialization
        printf("Message recu : %s\n", requete.reqBuff);
        switch (requete.reqNum) {
            case LOG:
                strcpy(chatters[chatters->nbChatters].chatters->nom, requete.reqBuff);
                printf("Joueur connecte : %s\n",chatters[0].chatters->nom);
                chatters->nbChatters++;
                printf("Nb joueurs : %d\n",chatters->nbChatters++);




                break;

        }
        if (strcmp(requete.reqBuff, "/bye") == 0) envoyerMessage(socketDial, BYE);
        else envoyerMessage(socketDial, OK);
    } while (strcmp(requete.reqBuff, "/bye") != 0);
}

void dialClt2srv(int socketAppel) {

    char buff[MAX_BUFF];
    printf("Veuillez vous log : pour cela entrez /log suivi de votre nom d'utilisateur :\n");
    custom_read(buff, MAX_BUFF);
    envoyerRequete(socketAppel, buff);

    do {
        memset(buff, 0, MAX_BUFF);
        printf("tapez votre message : \n"); //fflush(stdout);
        //scanf("%[^ ]\n", buff);
        //gets(buff);
        custom_read(buff, MAX_BUFF);
        envoyerMessage(socketAppel, buff);
        recevoirMessage(socketAppel, buff, MAX_BUFF);
    } while (strcmp(buff, "BYE") != 0);
}