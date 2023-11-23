#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int randomZeroOrOne() {
    // Initialise le générateur de nombres aléatoires avec le temps actuel
    srand(time(NULL));
    //test
    // Génère un nombre aléatoire (0 ou 1)
    return rand() % 2;
}


bool jouerCoup(Jeu *jeu,int coup, Joueur *joueur){
    if (!legaliteCoup(jeu, coup)){
        return false;
    } 
    
    // Stocker le nombre de billes dans la case choisie
    int billes = jeu->plateau[coup];

    // Retirer toutes les billes de la case choisie
    jeu->plateau[coup] = 0;
    // Distribuer les billes aux cases suivantes
    for (int i = 1; i <= billes; i++) {
        jeu->plateau[(coup + i) % 12]++;
    }

    int caseRetirer=(coup+billes)%12;
    bool retirer=true;
    while(retirer){
        if (jeu->plateau[caseRetirer]==2||jeu->plateau[caseRetirer]==3){
            if(jeu->tour==1){
                jeu->scoreJ2+=jeu->plateau[caseRetirer];
            }else{
                jeu->scoreJ1+=jeu->plateau[caseRetirer];
            }
            jeu->billesRestantes -= jeu->plateau[caseRetirer];
            jeu->plateau[caseRetirer] = 0;
        }else{
            retirer=false;
        }
        caseRetirer=abs((caseRetirer-1)%12);
    }
    // Changer de tour
    jeu->tour = (jeu->tour == 0) ? 1 : 0;
    //Enregistrer le coup dans l'historique
    enregistrerCoupHistorique(jeu,joueur,coup);
    return true;

}
// Fonction pour enregistrer un coup dans l'historique de partie du joueur
void enregistrerCoupHistorique(Jeu *jeu, Joueur *joueur, int coup) {
    
    // Création d'une chaîne de caractères pour stocker toutes les informations
    char buffer[1000]; // Ajustez la taille en fonction de vos besoins

    // Ajout des informations dans le buffer
    sprintf(buffer, "\n%s a joué le coup : %d\n", joueur->pseudo, coup);

    // Ajout du plateau dans le buffer
    sprintf(buffer + strlen(buffer), "Plateau : ");
    sprintf(buffer + strlen(buffer),afficherTableau(jeu) ); // Remplacez par votre code

    // Ajout des scores dans le buffer
    sprintf(buffer + strlen(buffer), "Score %s: %d | %s: %d\n", jeu->joueur1->pseudo, jeu->scoreJ1, jeu->joueur2->pseudo, jeu->scoreJ2);

    //FILE J1 
    char historiquePath1[strlen("data/") + strlen(jeu->joueur1->pseudo) + strlen("/historiquesParties.txt") + 1];
    sprintf(historiquePath1, "data/%s/historiquesParties.txt", jeu->joueur1->pseudo);
    FILE *historiquesFile1 = fopen(historiquePath1, "a");
    if (historiquesFile1 == NULL) {
        fprintf(stderr, "Erreur lors de la création ou de l'ouverture du fichier historiquesParties.txt.\n");
        perror("Erreur");
        exit(EXIT_FAILURE);
    }

    // Écriture du buffer dans le fichier
    fprintf(historiquesFile1, "%s", buffer);

    fflush(historiquesFile1); // Force l'écriture dans le fichier
    // Ferme le fichier historiqueParties.txt
    fclose(historiquesFile1);


    //FILE J2 
    char historiquePath2[strlen("data/") + strlen(jeu->joueur2->pseudo) + strlen("/historiquesParties.txt") + 1];
    sprintf(historiquePath2, "data/%s/historiquesParties.txt", jeu->joueur2->pseudo);
    FILE *historiquesFile2 = fopen(historiquePath2, "a");
    if (historiquesFile2 == NULL) {
        fprintf(stderr, "Erreur lors de la création ou de l'ouverture du fichier historiquesParties.txt.\n");
        perror("Erreur");
        exit(EXIT_FAILURE);
    }

    // Écriture du buffer dans le fichier
    fprintf(historiquesFile2, "%s", buffer);

    fflush(historiquesFile2); // Force l'écriture dans le fichier
    // Ferme le fichier historiqueParties.txt
    fclose(historiquesFile2);

}

Jeu* initJeu(Joueur *j1,Joueur *j2){
    Jeu *jeu =(Jeu*)malloc(sizeof(Jeu));
    for (int i=0;i<12;i++){
        jeu->plateau[i]=4;
    }
    jeu->joueur1=j1;
    jeu->joueur2=j2;

    jeu->tour=randomZeroOrOne();
    jeu->gagnant=-1;
    jeu->billesRestantes=48;
    jeu->estFini=false;
    jeu->scoreJ1=0;
    jeu->scoreJ2=0;
    ecrireDebutPartieHistoJoueur(jeu);
    return jeu;
}

bool legaliteCoup(Jeu *jeu,int coup){
    
    if(jeu->tour==1&&coup<6){
        return false;
    }
    if(jeu->tour==0&&coup>5){
        return false;
    }
    if(jeu->plateau[coup]==0){
        return false;
    }
    //coup du joueur 1
    if (jeu->tour==0){
        bool vide=true;
        int case1=6;
        while(vide&&case1<12){
            if(jeu->plateau[case1]!=0){
                vide=false;
            }
            case1++;
        }
        if(vide){
            if(coup+jeu->plateau[coup]<=5){ //cas de famine de l'adversaire
                return false;
            }
        }
    }
    //coup du joueur 2
    if (jeu->tour==1){
        bool vide=true;
        int case2=0;
        while(vide&&(case2<6)){
            if(jeu->plateau[case2]!=0){
                vide=false;
            }
            case2++;
        }
        if(vide){
            if(coup+jeu->plateau[coup]<=11){ //cas de famine de l'adversaire
                return false;
            }
        }
    }
    return true;
}

char* afficherTableau(Jeu *jeu) {
   char* plateauString = (char*)malloc(500); // Taille arbitraire, ajustez selon vos besoins

    sprintf(plateauString, "\n---------------------------\n");

    // Affiche les cases du plateau avec des traits
    for (int i = 11; i >= 6; i--) {
        if (i == 11) {
            sprintf(plateauString + strlen(plateauString), " | ");
        }
        sprintf(plateauString + strlen(plateauString), "%d", jeu->plateau[i]);

        // Ajoute un trait sauf pour la dernière case
        if (i >= 6) {
            sprintf(plateauString + strlen(plateauString), " | ");
        }
    }

    sprintf(plateauString + strlen(plateauString), "\n---------------------------\n");

    // Affiche les cases du plateau avec des traits
    for (int i = 0; i < 6; i++) {
        if (i == 0) {
            sprintf(plateauString + strlen(plateauString), " | ");
        }
        sprintf(plateauString + strlen(plateauString), "%d", jeu->plateau[i]);

        // Ajoute un trait sauf pour la dernière case
        if (i < 6) {
            sprintf(plateauString + strlen(plateauString), " | ");
        }
    }

    sprintf(plateauString + strlen(plateauString), "\n---------------------------\n");

    return plateauString;
}


int finPartie(Jeu *jeu){
    if(jeu->scoreJ1>25){
        return 0;
    }else if(jeu->scoreJ2>25){
        return 1;
    }

    //coup du joueur 1
    if (jeu->tour==0){
        bool vide=true;
        int case1=6;
        while(vide&&case1<12){
            if(jeu->plateau[case1]!=0){
                vide=false;
            }
            case1++;
        }
        if(vide){
            for (int i=0;i<6;i++){
                if (jeu->plateau[i]+i>=6){
                    jeu->scoreJ1+=jeu->billesRestantes;
                    if (jeu->scoreJ2>jeu->scoreJ1){
                        return 1;
                    }else{
                        return 0;
                    }
                }
            }
            return 0;
        }
    }
    //coup du joueur 2
    if (jeu->tour==1){
        bool vide=true;
        int case2=0;
        while(vide&&(case2<6)){
            if(jeu->plateau[case2]!=0){
                vide=false;
            }
            case2++;
        }
        if(vide){
            for (int i=6;i<12;i++){
                if (jeu->plateau[i]+i>11){
                    jeu->scoreJ2+=jeu->billesRestantes;
                    if (jeu->scoreJ2>jeu->scoreJ1){
                        return 1;
                    }else{
                        return 0;
                    }
                }
            }
        }
    }

    return -1;
}
void ecrireDebutPartieHistoJoueur(Jeu *jeu){
    // Création du chemin du fichier historiquesPartie.txt
    char historiquePath[strlen("data/") + strlen(jeu->joueur1->pseudo) + strlen("/historiquesParties.txt") + 1];
    sprintf(historiquePath, "data/%s/historiquesParties.txt", jeu->joueur1->pseudo);
    FILE *historiquesFile = fopen(historiquePath, "a");
    if (historiquesFile == NULL) {
        fprintf(stderr, "Erreur lors de la création ou de l'ouverture du fichier historiquesParties.txt.\n");
        perror("Erreur");
        exit(EXIT_FAILURE);
    }
    fprintf(historiquesFile, "\n+ %s VS %s --------------", jeu->joueur1->pseudo, jeu->joueur2->pseudo);
    fprintf(historiquesFile, afficherTableau(jeu));
    fflush(historiquesFile); // Force l'écriture dans le fichier
    // Ferme le fichier historiqueParties.txt
    fclose(historiquesFile);

    // Création du chemin du fichier historiquesPartie.txt
    char historiquePath2[strlen("data/") + strlen(jeu->joueur2->pseudo) + strlen("/historiquesParties.txt") + 1];
    sprintf(historiquePath2, "data/%s/historiquesParties.txt", jeu->joueur2->pseudo);
    FILE *historiquesFile2 = fopen(historiquePath2, "a");
    if (historiquesFile2 == NULL) {
        fprintf(stderr, "Erreur lors de la création ou de l'ouverture du fichier historiquesParties.txt.\n");
        perror("Erreur");
        exit(EXIT_FAILURE);
    }
    fprintf(historiquesFile2, "\n+ %s VS %s --------------", jeu->joueur2->pseudo, jeu->joueur1->pseudo);
    fprintf(historiquesFile2, afficherTableau(jeu));
    fflush(historiquesFile2); // Force l'écriture dans le fichier
    // Ferme le fichier historiqueParties.txt
    fclose(historiquesFile2);
}

void ecrireFinPartieHistoJoueur(Jeu *jeu){
    char* gagnant;
    if (jeu->gagnant==0){
        gagnant=jeu->joueur1->pseudo;
    }else{
        gagnant=jeu->joueur2->pseudo;
    }
    // Création du chemin du fichier historiquesPartie.txt
    char historiquePath[strlen("data/") + strlen(jeu->joueur1->pseudo) + strlen("/historiquesParties.txt") + 1];
    sprintf(historiquePath, "data/%s/historiquesParties.txt", jeu->joueur1->pseudo);
    FILE *historiquesFile = fopen(historiquePath, "a");
    if (historiquesFile == NULL) {
        fprintf(stderr, "Erreur lors de la création ou de l'ouverture du fichier historiquesParties.txt.\n");
        perror("Erreur");
        exit(EXIT_FAILURE);
    }
    fprintf(historiquesFile, "\n+Gagant: %s --------------", gagnant);
    fflush(historiquesFile); // Force l'écriture dans le fichier
    // Ferme le fichier historiqueParties.txt
    fclose(historiquesFile);

    // Création du chemin du fichier historiquesPartie.txt
    char historiquePath2[strlen("data/") + strlen(jeu->joueur2->pseudo) + strlen("/historiquesParties.txt") + 1];
    sprintf(historiquePath2, "data/%s/historiquesParties.txt", jeu->joueur2->pseudo);
    FILE *historiquesFile2 = fopen(historiquePath2, "a");
    if (historiquesFile2 == NULL) {
        fprintf(stderr, "Erreur lors de la création ou de l'ouverture du fichier historiquesParties.txt.\n");
        perror("Erreur");
        exit(EXIT_FAILURE);
    }
    fprintf(historiquesFile, "\n+Gagant: %s --------------", gagnant);
    fflush(historiquesFile2); // Force l'écriture dans le fichier
    // Ferme le fichier historiqueParties.txt
    fclose(historiquesFile2);
}





