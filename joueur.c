#include "joueur.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Joueur* createJoueur(const char* pseudo, const char* mdp, char* bio) {
    Joueur *joueur = (Joueur*)malloc(sizeof(Joueur));
    //Creation du fichier joueur 
    FILE* fichier;
    fichier = fopen(pseudo+"-historiquesParties-.txt", "a");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }
    joueur->historiqueParties = fichier;
    fclose(fichier);

    if (joueur != NULL) {
        // Allocation dynamique pour la chaîne de caractères du pseudo et mdp
        joueur->pseudo = (char*)malloc((strlen(pseudo) + 1) * sizeof(char));
        joueur->mdp = (char*)malloc((strlen(mdp) + 1) * sizeof(char));

        // Vérification si l'allocation a réussi
        if (joueur->pseudo == NULL) {
            printf( "Erreur d'allocation mémoire pour pseudo\n");
            exit(EXIT_FAILURE);
        }
        if (joueur->mdp == NULL) {
            printf( "Erreur d'allocation mémoire pour mdp\n");
            exit(EXIT_FAILURE);
        }

        // Copie du pseudo et mdp dans la structure du joueur
        strcpy(joueur->pseudo, pseudo);
        strcpy(joueur->mdp, mdp);

        // Vérifie si bio existe
        if (bio != NULL) {
            joueur->bio = (char*)malloc((strlen(bio) + 1) * sizeof(char));
            strcpy(joueur->bio, bio);
        } else {
            joueur->bio = NULL;  // bio est NULL si n'existe pas
        }
    }
    return joueur;
}



