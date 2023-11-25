#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"
enum EtatClient
{
   ETAT_MENU,
   ETAT_AJOUTER_AMI,
   ETAT_INVITATION_PARTIE,
   ETAT_ECRIRE_BIO,
   ETAT_VOIR_BIO,
   ETAT_MESSAGE,
   ETAT_ATTENTE_REPONSE_INVITATION,
};
typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   enum EtatClient etat;
   char bio[800];
   struct Client *adversaire;
   // int inscrit;
   struct Client *amis[50];
   bool confidentialitePublique;
   bool sauvegardeMode;

} Client;

#endif /* guard */
