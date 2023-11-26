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
   ETAT_DOIT_REPONDRE_INVITATION,
   ETAT_OBSERVATEUR_JEU,
   ETAT_CHOIX_OBSERVATEUR,
};
typedef struct Client
{
   SOCKET sock;
   struct Client * adversaire;
   char name[BUF_SIZE];
   enum EtatClient etat;
   char bio[800];
   // int inscrit;
   struct Client *amis[50];
   bool confidentialitePublique;
   bool sauvegardeMode;

}Client;

#endif /* guard */
