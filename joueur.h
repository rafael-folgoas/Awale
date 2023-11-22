#ifndef JOUEUR_H
#define JOUEUR_H

typedef struct  {
    char* pseudo;
} Joueur;

// Prototype de la fonction pour créer un joueur
Joueur* createJoueur(const char* pseudo);

#endif
