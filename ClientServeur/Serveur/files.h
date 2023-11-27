#ifndef FILE_H
#define FILE_H
#include <stdlib.h>
#include <stdio.h>
#include "client2.h"

void creerFiles(const char *pseudo);
void remplirFileJoueur(char *pseudo, char *mdp, char *bio);
char* lireHistoriqueFile(char *pseudo);
//void remplirInfosJoueur(Client *joueur);
#endif
