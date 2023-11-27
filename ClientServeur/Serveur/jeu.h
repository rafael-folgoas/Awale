#ifndef JEU_H
#define JEU_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "client2.h"
#include "files.h"

Jeu* initJeu(Client *j1,Client *j2);
void enregistrerCoupHistorique(Jeu *jeu,Client* joueur, int coup);
char* afficherTableau(Jeu *jeu);
int randomZeroOrOne();
void ecrireDebutPartieHistoJoueur(Jeu *jeu);
void ecrireFinPartieHistoJoueur(Jeu *jeu);
bool jouerCoup(Jeu *jeu,int coup,Client* joueur);
bool legaliteCoup(Jeu *jeu,int coup);
int finPartie(Jeu *jeu);


#endif
