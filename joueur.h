#ifndef JOUEUR_H
#define JOUEUR_H
#include <stdlib.h>
#include <stdio.h>

typedef struct  {
    char* pseudo;
    char* mdp;
    char* bio;
    FILE* historiqueParties;
}Joueur;

// Prototype de la fonction pour créer un joueur
Joueur* createJoueur(const char* pseudo, const char* mdp, char* bio);

#endif
