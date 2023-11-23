#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int main() {
    char pseudo1[50];
    char mdp1[50];
    char bio1[100];
    // Demander au joueur 1 de rentrer ses informations
    printf("Joueur 1, Entrez votre pseudo : ");
    scanf("%s", pseudo1);

    printf("Entrez votre mot de passe : ");
    scanf("%s", mdp1);

    printf("Entrez votre bio : ");
    scanf(" %s[^\n]", bio1);

    char pseudo2[50];
    char mdp2[50];
    char bio2[100];
    // Demander au joueur 2 de rentrer ses informations
    printf("Joueur 2, Entrez votre pseudo : ");
    scanf("%s", pseudo2);

    printf("Entrez votre mot de passe : ");
    scanf("%s", mdp2);

    printf("Entrez votre bio : ");
    scanf(" %s[^\n]", bio2);
    
    // Création d'un joueur avec le pseudo "Joueur1"
    Joueur *j1 = createJoueur(pseudo1, mdp1, bio1);
    // Création d'un joueur avec le pseudo "Joueur1"
    Joueur *j2 = createJoueur(pseudo2, mdp2, bio2);

    // initialisation du jeu avec les 2 joueurs j1 et j2
    Jeu *jeu=initJeu(j1,j2);

    while(!jeu->estFini){
        int fin=-1;
    
        if (jeu->tour==0&&fin==-1){
            printf("Tour de ");
            printf(j1->pseudo);
            printf("\nChoisir case entre 0 et 5 :\n");
            printf(afficherTableau(jeu));
            printf("Scores actuels :\n");
            printf("Joueur 1 : %d\n",jeu->scoreJ1);
            printf("Joueur 2 : %d\n",jeu->scoreJ2);          
            int coup;
            scanf("%d", &coup);
            while(!jouerCoup(jeu,coup,j1)){
                printf("Coup pas bon\n");
                scanf("%d", &coup);
            }
            fin=finPartie(jeu);
        }
        
        if (jeu->tour==1 && fin==-1){
            printf("Tour de ");
            printf(j2->pseudo);
            printf("\nChoisir case entre 6 et 11 :\n");
            printf(afficherTableau(jeu));
            printf("Scores actuels :\n");
            printf("Joueur 1 : %d\n",jeu->scoreJ1);
            printf("Joueur 2 : %d\n",jeu->scoreJ2);
            int coup;
            scanf("%d", &coup);
            while(!jouerCoup(jeu,coup,j2)){
                printf("Coup pas bon\n");
                scanf("%d", &coup);
            }
            fin=finPartie(jeu);
        }
        
        if(fin!=-1){
            jeu->gagnant=fin;
            jeu->estFini=1;
            ecrireFinPartieHistoJoueur(jeu);    
            printf("Le gagnant est le joueur : %d\n", jeu->gagnant+1);
            printf("Score final : \n");
            printf("Joueur 1 : %d\n",jeu->scoreJ1);
            printf("Joueur 2 : %d\n",jeu->scoreJ2);
        }
    };

    return 0;
}