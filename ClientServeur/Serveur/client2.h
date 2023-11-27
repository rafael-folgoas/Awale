#ifndef CLIENT_H
#define CLIENT_H
#define MAX_OBSERVERS 30
#include <sys/socket.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
typedef int SOCKET;

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
   ETAT_JEU_EN_COURS,
};
typedef struct Client
{
   SOCKET sock;
   struct Client * adversaire;
   char mdp[20];
   char name[20];
   enum EtatClient etat;
   char bio[800];
   struct Client *amis[50];
   int nbAmis;
   struct Jeu *jeuEnCours;
   bool confidentialitePublique;
   bool sauvegardeMode;

}Client;

typedef struct Jeu
{
   int plateau[12];
   int tour;
   int gagnant;
   int billesRestantes;
   bool estFini;
   int scoreJ1;
   int scoreJ2;
   Client *joueur1;
   Client *joueur2;
   Client *observers[MAX_OBSERVERS];
   int nbObservers;
} Jeu;


#endif /* guard */
