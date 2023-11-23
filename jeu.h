#ifndef JEU_H
#define JEU_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "joueur.h"


typedef struct  {
    int plateau[12];
    int tour;
    int gagnant;
    int billesRestantes;
    bool estFini;
    int scoreJ1;
    int scoreJ2;
    Joueur *joueur1;
    Joueur *joueur2;

}Jeu;


//init du plateau 
Jeu* initJeu(Joueur *j1,Joueur *j2);
void enregistrerCoupHistorique(Jeu *jeu,Joueur* joueur, int coup);
//afficher tableau 
char* afficherTableau(Jeu *jeu);
//Choix du premier joueur 
int randomZeroOrOne();

void ecrireDebutPartieHistoJoueur(Jeu *jeu);
void ecrireFinPartieHistoJoueur(Jeu *jeu);

//jouer un coup 
bool jouerCoup(Jeu *jeu,int coup,Joueur* joueur);

bool legaliteCoup(Jeu *jeu,int coup);
//Fin partie 
int finPartie(Jeu *jeu);


#endif
