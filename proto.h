#ifndef PROTO_H
#define PROTO_H

#include "reqRep.h"
#include "data.h" // Potentiellement

// Correspond à la couche 7 du modèle OSI (couche Application)

typedef struct chatter {
    char nom[60];        /** numéro de requête -- toujours présent. Définit le type de requête (message ou type de commande) */

} chatter_t;

typedef struct chatter_tab {
    chatter_t chatters[60];        /** numéro de requête -- toujours présent. Définit le type de requête (message ou type de commande) */
    int nbChatters;
} chatter_tab;




#endif /* PROTO_H */
void dialSrv2Clt(int socketDial,chatter_tab * chatterTab);
void dialClt2srv(int socketAppel);