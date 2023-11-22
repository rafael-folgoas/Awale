#include "joueur.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Joueur* createJoueur(const char* pseudo) {
    Joueur *joueur = (Joueur*)malloc(sizeof(Joueur));

    // Allocation dynamique pour la chaîne de caractères du pseudo
    joueur->pseudo = (char*)malloc((strlen(pseudo) + 1) * sizeof(char));

    // Vérification si l'allocation a réussi
    if (joueur->pseudo == NULL) {
        printf( "Erreur d'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    // Copie du pseudo dans la structure du joueur
    strcpy(joueur->pseudo, pseudo);


    return joueur;
}
