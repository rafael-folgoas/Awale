#include "joueur.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

struct Jeu {
    int* plateau[12];
    int tour;
    int gagnant;
    int billesRestantes;
    Joueur joueur1;
    Joueur joueur2;

};

//init du plateau 
void initPlateau(Jeu jeu);

//afficher tableau 
void afficherTableau(Jeu jeu );
//Choix du premier joueur 
void randomZeroOrOne(Jeu jeu) ;

//jouer un coup 
bool jouerCoup(Jeu jeu,int coup);

//Fin partie 

