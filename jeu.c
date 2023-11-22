#include "jeu.h"

void randomZeroOrOne(Jeu jeu) {

    // Initialise le générateur de nombres aléatoires avec le temps actuel
    srand(time(NULL));

    // Génère un nombre aléatoire (0 ou 1)
    jeu.tour = rand() % 2;
    
}


bool jouerCoup(Jeu jeu,int coup){


}

void initPlateau(Jeu jeu){
    for (int i=0;i<12;i++){
        jeu.plateau[i]=i;
    }
}
bool legaliteCoup(Jeu jeu,int coup){
    
    if(jeu.tour&&coup<7){
        return false;
    }
    if(jeu.tour==0&&coup>6){
        return false;
    }
    if(plateau[coup]==0){
        return false;
    }

}

void afficherTableau(Jeu jeu) {

    // Affiche les cases du plateau avec des traits
    for (int i = 12; i >= 7; i++) {
        printf("%d", jeu.plateau[i]);

        // Ajoute un trait sauf pour la dernière case
        if (i <= 7) {
            printf(" | ");
        }
    }
    printf("\n");
    // Affiche les cases du plateau avec des traits
    for (int i = 0; i < 7; i++) {
        printf("%d", jeu.plateau[i]);

        // Ajoute un trait sauf pour la dernière case
        if (i < 7) {
            printf(" | ");
        }
    }
    printf("\n");
}

