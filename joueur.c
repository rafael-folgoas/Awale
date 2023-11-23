#include "joueur.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>


Joueur* createJoueur(const char* pseudo, const char* mdp, char* bio) {
    
    Joueur *joueur = (Joueur*)malloc(sizeof(Joueur));

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
    creerFiles(joueur->pseudo);
    remplirFileJoueur(joueur);
    return joueur;
}

void remplirFileJoueur(Joueur *joueur){
    FILE* fichierJoueurs;
    fichierJoueurs = fopen("data/joueurs.txt", "a");

    if (fichierJoueurs == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    fprintf(fichierJoueurs, "%s %s %s\n", joueur->pseudo, joueur->mdp, joueur->bio);
    printf("Nouveau joueur enregistré avec succès.\n");
    fclose(fichierJoueurs);
}
void creerFiles(const char *pseudo) {
    //Creation du dossier data
    if (mkdir("data", 0777) == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "Erreur lors de la création du dossier.\n");
            perror("Erreur");
            exit(EXIT_FAILURE);
        }
    }
    // Création du chemin du dossier du joueur
    char dossierPath[strlen("data/") + strlen(pseudo) + 1];
    sprintf(dossierPath, "data/%s", pseudo);

    // Création du chemin du fichier historiquesPartie.txt
    char historiquesPath[strlen(dossierPath) + strlen("/historiquesParties.txt") + 1];
    sprintf(historiquesPath, "%s/historiquesParties.txt", dossierPath);

    // Création du chemin du fichier infos.txt
    char infosPath[strlen(dossierPath) + strlen("/infos.txt") + 1];
    sprintf(infosPath, "%s/infos.txt", dossierPath);

    // Utilisation de mkdir pour créer le dossier
    if (mkdir(dossierPath, 0777) == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "Erreur lors de la création du dossier.\n");
            perror("Erreur");
            exit(EXIT_FAILURE);
        }
    }

    // Création des fichiers
    FILE *historiquesFile = fopen(historiquesPath, "a");
    if (historiquesFile == NULL) {
        fprintf(stderr, "Erreur lors de la création du fichier historiquesParties.txt.\n");
        perror("Erreur");
        exit(EXIT_FAILURE);
    }
    fclose(historiquesFile);

    FILE *infosFile = fopen(infosPath, "a");
    if (infosFile == NULL) {
        fprintf(stderr, "Erreur lors de la création du fichier infos.txt.\n");
        perror("Erreur");
        exit(EXIT_FAILURE);
    }
    fclose(infosFile);

    printf("Dossier et fichiers créés avec succès.\n");
}


// Fonction pour libérer la mémoire allouée pour le joueur
void destroyJoueur(Joueur* joueur) {
    free(joueur->pseudo);
    free(joueur->mdp);
    free(joueur->bio);
    fclose(joueur->historiqueParties);
    free(joueur);
}