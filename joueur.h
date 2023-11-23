#ifndef JOUEUR_H
#define JOUEUR_H

typedef struct  {
    char* pseudo;
    char* mdp;
    char* bio;
}Joueur;

// Prototype de la fonction pour créer un joueur
Joueur* createJoueur(const char* pseudo, const char* mdp, char* bio);

#endif
