#include "jeu.h"

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

    return true;

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

void afficherTableau(Jeu *jeu) {
    printf("\n---------------------------\n");
    // Affiche les cases du plateau avec des traits
    for (int i = 11; i >= 6; i--) {
        if (i == 11) {
            printf(" | ");
        }
        printf("%d", jeu->plateau[i]);

        // Ajoute un trait sauf pour la dernière case
        if (i >= 6) {
            printf(" | ");
        }
    }
    printf("\n---------------------------\n");
    // Affiche les cases du plateau avec des traits
    for (int i = 0; i < 6; i++) {
        if (i == 0) {
            printf(" | ");
        }
        printf("%d", jeu->plateau[i]);

        // Ajoute un trait sauf pour la dernière case
        if (i < 6) {
            printf(" | ");
        }
    }
    printf("\n---------------------------\n");
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


