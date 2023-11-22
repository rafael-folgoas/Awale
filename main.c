#include "jeu.c"

int main() {
    Jeu jeu;

    // ... initialisation du jeu ...
    initPlateau(jeu);
    // Appel de la fonction pour afficher le plateau
    afficherTableau(jeu);

    return 0;
}