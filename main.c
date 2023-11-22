#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int main() {

    // Création d'un joueur avec 0 points et le pseudo "Joueur1"
    Joueur *j1 = createJoueur("Joueur1");
    // Création d'un joueur avec 0 points et le pseudo "Joueur1"
    Joueur *j2 = createJoueur("Joueur2");

    // Affichage des informations du joueur
    printf("Joueur : %s, Points : %d\n", j1->pseudo);
    Jeu *jeu=initJeu(j1,j2);




    while(!jeu->estFini){
  
        printf("Tour de ");
        if (jeu->tour==0){
            printf(j1->pseudo);
            printf("\nChoisir case entre 0 et 5 :\n");
            afficherTableau(jeu);
            int coup;
            scanf("%d", &coup);
            while(!jouerCoup(jeu,coup,j1)){
                printf("Coup pas bon\n");
                scanf("%d", &coup);
            }
        }

        if (jeu->tour==1){
            printf(j2->pseudo);
            printf("\nChoisir case entre 6 et 11 :\n");
            afficherTableau(jeu);
            int coup;
            scanf("%d", &coup);
            while(!jouerCoup(jeu,coup,j2)){
                printf("Coup pas bon\n");
                scanf("%d", &coup);
            }
        }
    
    };

    return 0;
}