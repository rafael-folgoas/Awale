#include "joueur.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>


Joueur* createJoueur(const char* pseudo, const char* mdp, char* bio) {
    
    Joueur *joueur = (Joueur*)malloc(sizeof(Joueur));

    //Creation du dossier du joueur 
    char dossier[strlen(pseudo)+1];
    strcpy(dossier,pseudo);

    // Utilisation de mkdir pour créer le dossier
    if (mkdir(dossier, 0777) == -1) {
        if (errno == EEXIST) {
            printf("Le dossier existe déjà.\n");
        } else {
            fprintf(stderr, "Erreur lors de la création du dossier.\n");
            perror("Erreur");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Dossier créé avec succès.\n");
    }

    //création des fichiers des joueurs
    //infos
    FILE* fichierI; 
    char nomFichI[strlen(dossier) + sizeof("/infos.txt")];
    strcpy(nomFichI, dossier);
    strcat(nomFichI, "/infos.txt");
    fichierI = fopen(nomFichI, "a");
    if (fichierI == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }
    // Écrire les infos dans le fichier
    fprintf(fichierI, "Pseudo: %s\n", pseudo);
    fprintf(fichierI, "Mot de passe: %s\n", mdp);
    fprintf(fichierI, "Bio: %s\n", bio);
    
    fclose(fichierI);

    //historique parties
    FILE* fichierH; 
    char nomFichH[strlen(dossier) + sizeof("/historiqueParties.txt")];
    strcpy(nomFichH, dossier);
    strcat(nomFichH, "/historiqueParties.txt");
    fichierH = fopen(nomFichH, "a");
    if (fichierH == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }
    joueur->historiqueParties = fichierH;
    fclose(fichierH);


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



