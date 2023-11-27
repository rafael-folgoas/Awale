#include "files.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>



void remplirFileJoueur(char *pseudo, char *mdp, char *bio){
    FILE* fichierJoueurs;
    fichierJoueurs = fopen("data/joueurs.txt", "a");

    if (fichierJoueurs == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    fprintf(fichierJoueurs, "%s %s %s\n", pseudo, mdp, bio);
    printf("Nouveau joueur enregistré avec succès.\n");
    fclose(fichierJoueurs);
}

/*void remplirInfosJoueur(Client *joueur){
    FILE* infosJoueur;
    char infoPath[strlen("data/") + strlen(joueur->pseudo) + strlen("/infos.txt") + 1];
    sprintf(infoPath, "data/%s/infos.txt", joueur->pseudo);

    infosJoueur = fopen(infoPath, "a");

    if (infosJoueur == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    fprintf(infosJoueur, "Pseudo : %s\n", joueur->pseudo);
    fprintf(infosJoueur, "Mdp : %s\n", joueur->mdp);
    fprintf(infosJoueur, "Bio : %s\n", joueur->bio);
    fclose(infosJoueur);
}
*/
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
