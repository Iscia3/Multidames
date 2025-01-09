#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Définition des constantes pour les types de pions et les couleurs
#define PION_BLEU 1
#define DAME_BLEU 2
#define PION_ROUGE 3
#define DAME_ROUGE 4
#define PION_JAUNE 5
#define DAME_JAUNE 6
#define PION_VERT 7
#define DAME_VERT 8
#define CASE_VIDE 9

// Dimensions du plateau
#define TAILLE 17 // Le plateau est 17x17

// Codes ANSI pour les couleurs
#define RESET "\033[0m"   // Réinitialisation des styles
#define FOND_CYAN "\033[46m" // Fond cyan
#define FOND_NOIR "\033[40m"  // Fond noir
#define FOND_BLANC "\033[47m" // Fond blanc
#define TXT_BLEU "\033[34m"   // Texte bleu

// Structure du plateau de jeu
typedef struct {
    int index[TAILLE][TAILLE]; // État du plateau
    int nb_bleu, nb_rouge, nb_jaune, nb_vert; // Nombre de pions pour chaque couleur
} Damier;

// Structure pour représenter une case
typedef struct {
    int numero;  // Numéro de la case
} Case_debut, Case_fin, Case_intermediaire;

// Structure pour représenter un déplacement
typedef struct {
    Case_debut case_i;  // Case de départ
    Case_fin case_f;    // Case d'arrivée
    char* Mouvement;    // Type de mouvement
    char* Commentaire;  // Commentaire sur le mouvement
    struct Deplacement* Mouv_suivant; // Pointeur vers le prochain mouvement
} Deplacement;

typedef enum {
    LIBRE, 
    OCCUPEE, 
    NON_AFFECTABLE
} EtatCase;

typedef enum{
    AvG,
    AvD,
    PriseAvG,
    PriseAvD,
    PriseArG,
    PriseArD,
    ArG,
    ArD,
    Aucun
} Mouvement;

typedef struct {
    int i; // Ligne
    int j; // Colonne
} Case;

typedef struct {
    char A; // Ligne
    char a; // Colonne
} Lettre;

// Joueur
typedef enum {
    Noire,
    Bleu,
    Rouge,
    Jaune,
    Vert    
} Couleur_joueur;

void afficherDamier(Damier* jeu,Damier* temp, Couleur_joueur couleur);

/*
Fonction pour vérifier si une cellule appartient à un coin cyan
*/

int Coin_Cyan(int i, int j) {
    return ((i >= 0 && i < 4 && j >= 0 && j < 4) ||       // Coin supérieur gauche
            (i >= 13 && i < TAILLE && j >= 0 && j < 4) || // Coin inférieur gauche
            (i >= 0 && i < 4 && j >= 13 && j < TAILLE) || // Coin supérieur droit
            (i >= 13 && i < TAILLE && j >= 13 && j < TAILLE)); // Coin inférieur droit
}

/*
Initialisation du damier
*/

void init_damier(Damier* jeu) {
    jeu->nb_bleu = 22;
    jeu->nb_rouge = 22;
    jeu->nb_jaune = 22;
    jeu->nb_vert = 22;
    //printf("%d",jeu->nb_bleu);
    int nb_cases_bleu, nb_cases_rouge, nb_cases_jaune,nb_cases_vert;
    nb_cases_bleu=nb_cases_jaune=nb_cases_rouge=nb_cases_vert=22;

    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            jeu->index[i][j] = CASE_VIDE;
        }
    }

    for (int i = 0; i < 5; i++) {
        for (int j = 4; j < 13; j++) {
            if ((i + j) % 2 == 1) {
                jeu->index[i][j] = PION_BLEU;
            }
        }
    }

    for (int i = 12; i < 17; i++) {
        for (int j = 4; j < 13; j++) {
            if ((i + j) % 2 == 1) {
                jeu->index[i][j] = PION_ROUGE;
            }
        }
    }

    for (int i = 4; i < 13; i++) {
        for (int j = 12; j < TAILLE; j++) {
            if ((i + j) % 2 == 1) {
                jeu->index[i][j] = PION_JAUNE;
            }
        }
    }

    for (int i = 4; i < 13; i++) {
        for (int j = 0; j < 5; j++) {
            if ((i + j) % 2 == 1) {
                jeu->index[i][j] = PION_VERT;
            }
        }
    }
}

/*
 * Affiche une cellule selon son état dans le damier.
*/

void afficher_Cellule(int i, int j, int index[TAILLE][TAILLE]) {
    if (Coin_Cyan(i, j)) {
        printf(FOND_CYAN "   " RESET); // Fond cyan sans traits noirs
    } else {
        switch (index[i][j]) {
            case PION_BLEU:
                printf(FOND_NOIR TXT_BLEU " ● " RESET);
                break;
            case DAME_BLEU:
                printf(FOND_NOIR TXT_BLEU " o " RESET);
                break;
            case PION_ROUGE:
                printf(FOND_NOIR "\033[31m ● " RESET); // Rouge
                break;
            case DAME_ROUGE:
                printf(FOND_NOIR "\033[31m o " RESET);
                break;
            case PION_JAUNE:
                printf(FOND_NOIR "\033[33m ● " RESET); // Jaune
                break;
            case DAME_JAUNE:
                printf(FOND_NOIR "\033[33m o " RESET);
                break;
            case PION_VERT:
                printf(FOND_NOIR "\033[32m ● " RESET); // Vert
                break;
            case DAME_VERT:
                printf(FOND_NOIR "\033[32m o " RESET);
                break;
            case CASE_VIDE:
                // Alternance blanc/noir pour les cases vides
                if ((i + j) % 2 == 0) {
                    printf(FOND_BLANC "   " RESET);
                } else {
                    printf(FOND_NOIR "   " RESET);
                }
                break;
            default:
                printf(" ? "); // Cas invalide
                break;
        }
    }
}


/*
 Affiche le damier complet avec les indices en haut et à gauche, c'est le damier de référence avec le joueur rouge en bas
 */

void afficher_Damier(Damier* jeu) {
    // Affiche les indices des colonnes (1 à 16) en haut
    printf("   "); 
    for (int j = 0; j < TAILLE; j++) {
         if (j<=9){
            printf(TXT_BLEU " %d "RESET, j);
        }
        else{
            printf(TXT_BLEU "%d "RESET, j);
        }
        //printf(TXT_BLEU "%d  "RESET, j);
        
    }
    printf("\n");

    for (int i = 0; i < TAILLE; i++) {
        // Affiche les indices des lignes (1 à 16) à gauche
        if (i<=9){
            printf(TXT_BLEU " %d "RESET, i);
        }
        else{
            printf(TXT_BLEU "%d "RESET, i);
        }
        //printf(TXT_BLEU " %d "RESET, i);        
        for (int j = 0; j < TAILLE; j++) {
            afficher_Cellule(i, j, jeu->index);
        }
        printf("\n");
    }
}

 /*
Fonction qui renvoie la couleur du joueur selon le pion
*/

int couleur_de_case(Damier* jeu, int x, int y){ // x depart, y arrivee
    switch(jeu->index[x][y]){
    case PION_BLEU:
        return ( Bleu);
    case PION_ROUGE:
        return ( Rouge);
    case PION_JAUNE:
        return( Jaune);
    case PION_VERT:
        return (Vert);
    }
}

/*
 Fonction pour déterminer l'état d'une case
*/

EtatCase etat_case(int i, int j, Damier* jeu) {
    if (Coin_Cyan(i, j)) {
        return NON_AFFECTABLE; // Cases des coins cyan
    }

    switch (jeu->index[i][j]) {
        case CASE_VIDE :
            if ((i+j)%2==1)
                return LIBRE;
            else{return NON_AFFECTABLE;}
        case PION_BLEU:
        case DAME_BLEU:
        case PION_ROUGE:
        case DAME_ROUGE:
        case PION_JAUNE:
        case DAME_JAUNE:
        case PION_VERT:
        case DAME_VERT:
            return OCCUPEE;
        default:
            return NON_AFFECTABLE; // Cas inattendu
    }
}

 /*
 Affichage du Damier avec l'état des cases au lieu des pions
*/

void afficher_etats_cases(Damier* jeu) {
    printf("États des cases :\n\n");
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            EtatCase etat = etat_case(i, j, jeu);
            switch (etat) {
                case LIBRE:
                    printf(" L ");
                    break;
                case OCCUPEE:
                    printf(" O ");
                    break;
                case NON_AFFECTABLE:
                    printf(" N ");
                    break;
            }

        }
        printf("\n");
    }
}

/*
Fonction qui permet le déplacement du pion
*/

Damier* mvt(Damier* jeu, Couleur_joueur couleur){
    int nb_cases_bleu, nb_casses_rouge, nb_cases_jaune,nb_cases_vert;
    Case case_debut, case_fin;
    Damier jeux;
    Lettre fin_x,fin_y,debut_x,debut_y;
    Damier* nb_bleu,nb_vert,nb_jaune,nb_rouge;
    Damier temp;
    int reponse;
    int pas_fini;
    pas_fini=1;
    do
    {

    /*
    IDEE D'AMELIORATION ERGONOMIQUE

    printf("Entrez la case de départ (A j), puis de fin : ");
    scanf("%c %d %c %d", &debut_x.A, &case_debut.j, &fin_x.A, &case_fin.j);

    case_debut.i=debut_x.A-65;
    case_fin.j=fin_x.A-65;
    */


    printf("Entrez la case de départ (ligne colonne) puis la case de fin (ligne colonne) : ");
    scanf("%d %d %d %d", &case_debut.i, &case_debut.j,&case_fin.i, &case_fin.j);

    case_debut.j=case_debut.j+1; //parce qu'il y a la colonne de lettres
    case_fin.j=case_fin.j+1;

    if ( jeu->index[case_fin.i][case_fin.j]== CASE_VIDE && jeu->index[case_debut.i][case_debut.j]== couleur_de_case(jeu,case_debut.i,case_debut.j) )
    //verification case intiale libre et bonne couleur (que ce soir la meme que le joueur)
    {        
        case_debut.j=case_debut.j-1;
        case_fin.j=case_fin.j-1;
        int chiffre1=abs(case_fin.j- case_debut.j);
        int chiffre2=abs(case_fin.i- case_debut.i);
        if (abs(case_fin.j- case_debut.j)==abs(case_fin.i- case_debut.i)){
            if(abs(case_fin.j- case_debut.j)==1){ 
                /*
                        MOUVEMENT SIMPLE
                */
                switch (couleur) {
                    case Rouge:
                        if (abs(case_fin.i- case_debut.i)==1){
                            if (jeu->index[case_debut.i][case_debut.j]==PION_ROUGE){
                                printf("Mouvement valide\n");
                                jeu->index[case_debut.i][case_debut.j] = CASE_VIDE;  // Vide la case de départ
                                jeu->index[case_fin.i][case_fin.j] =OCCUPEE;
                                jeu->index[case_fin.i][case_fin.j] = couleur+1;  // Place le pion à la case d'arrivée
                                //il faut mettre couleur+2 si c'est une dame
                                pas_fini=0; /*on met la variable à zero, comme ca la boucle while se termine (car un seul coup simple autorisé)*/
                            }
                        }
                        else{
                            printf("Mouvement invalide.\n");
                            pas_fini=1;
                        }
                        break;
                        
                    case Jaune:
                        if (abs(case_fin.j- case_debut.j)==1){
                            if (jeu->index[case_debut.j][case_debut.i]==PION_JAUNE){
                            printf("Mouvement valide\n");
                            jeu->index[case_debut.j][case_debut.i] = CASE_VIDE;  // Vide la case de départ
                            jeu->index[case_fin.j][case_fin.i]=OCCUPEE;
                            jeu->index[case_fin.j][case_fin.i] = couleur+2;   // Place le pion à la case d'arrivée 
                            /*
                            couleur+2 car c'est la couleur joueur definie en typedef enum sauf qu'il y a aussi les define
                            donc sans +2 cela donnerait un pion rouge
                            */

                            //couleur+3 si c'est une dame
                            pas_fini=0;
                            }
                        }
                        else{
                            printf("Mouvement invalide.\n");
                            pas_fini=1;
                        }
                        break;
                    case Bleu:
                        if (abs(case_fin.i- case_debut.i)==1){
                            if (jeu->index[case_debut.i][case_debut.j]==PION_BLEU){
                                printf("Mouvement valide\n");
                                jeu->index[case_debut.i][case_debut.j] = CASE_VIDE;  // Vide la case de départ
                                jeu->index[case_fin.i][case_fin.j]=OCCUPEE;
                                jeu->index[case_fin.i][case_fin.j] = couleur;    // Place le pion à la case d'arrivée
                                pas_fini=0;
                            }
                        }
                        else{
                            printf("Mouvement invalide.\n");
                            pas_fini=1;
                        }
                        break;
                        
                    case Vert:
                        if (abs(case_fin.j- case_debut.j)==1){
                            if (jeu->index[case_debut.j][case_debut.i]==PION_VERT){
                            printf("Mouvement valide\n");
                            jeu->index[case_debut.j][case_debut.i] = CASE_VIDE;  // Vide la case de départ
                            jeu->index[case_fin.j][case_fin.i] =OCCUPEE;
                            jeu->index[case_fin.j][case_fin.i] = couleur+3;    // Place le pion à la case d'arrivée
                            pas_fini=0;
                            }
                        }
                        else{
                            printf("Mouvement invalide.\n");
                            pas_fini=1;
                        }
                        break;
                    default: 
                        printf("Mouvement invalide.\n");
                        pas_fini=1;
            }}
            else{
                if(abs(case_fin.j- case_debut.j)==2){ 
                    /*
                        MOUVEMENT AVEC SAUT
                    */
                    printf("Mouvement valide");
                    Case case_intermediaire;
                    case_intermediaire.i=(case_fin.j - case_debut.j)/2+case_debut.j;
                    case_intermediaire.j=(case_fin.i - case_debut.i)/2 + case_debut.i;
                    //jeu->index[case_intermediaire.j][case_intermediaire.i]
                    if ((etat_case(case_intermediaire.j,case_intermediaire.i,jeu)==OCCUPEE)||(etat_case(case_intermediaire.i,case_intermediaire.j,jeu)==OCCUPEE)){
                        //printf("mvt valide");
                        if (couleur!=couleur_de_case(jeu,case_intermediaire.i,case_intermediaire.j)){
                            //printf("mvt valide");
                            switch(couleur){
                            case Rouge: 
                                jeu->index[case_debut.i][case_debut.j] = CASE_VIDE;  // Vide la case de départ
                                jeu->index[case_intermediaire.j][case_intermediaire.i] = CASE_VIDE;  // Vide la case intermediaire
                                jeu->index[case_fin.i][case_fin.j]=OCCUPEE;
                                jeu->index[case_fin.i][case_fin.j] = couleur+1;    // Place le pion à la case d'arrivée
                                break;
                            case Jaune: 
                                jeu->index[case_debut.j][case_debut.i] = CASE_VIDE;  // Vide la case de départ
                                jeu->index[case_intermediaire.i][case_intermediaire.j] = CASE_VIDE;  // Vide la case intermediaire
                                jeu->index[case_fin.j][case_fin.i]=OCCUPEE;
                                jeu->index[case_fin.j][case_fin.i] = couleur+2;    // Place le pion à la case d'arrivée
                                break;
                            case Bleu: 
                                jeu->index[case_debut.i][case_debut.j] = CASE_VIDE;  // Vide la case de départ
                                jeu->index[case_intermediaire.j][case_intermediaire.i] = CASE_VIDE;  // Vide la case intermediaire
                                jeu->index[case_fin.i][case_fin.j]=OCCUPEE;
                                jeu->index[case_fin.i][case_fin.j] = couleur;    // Place le pion à la case d'arrivée
                                break;
                            case Vert: 
                                jeu->index[case_debut.j][case_debut.i] = CASE_VIDE;  // Vide la case de départ
                                jeu->index[case_intermediaire.i][case_intermediaire.j] = CASE_VIDE;  // Vide la case intermediaire
                                jeu->index[case_fin.j][case_fin.i]=OCCUPEE;
                                jeu->index[case_fin.j][case_fin.i] = couleur+3;    // Place le pion à la case d'arrivée
                                break;
                            }
                            switch(jeu->index[case_intermediaire.i][case_intermediaire.j]){
                                case PION_BLEU:
                                    //jeu->nb_bleu=jeu->nb_bleu-1;
                                    nb_cases_bleu=nb_cases_bleu-1;
                                    break;
                                case PION_ROUGE:
                                    jeu->nb_rouge=jeu->nb_rouge-1;
                                    break;
                                case PION_JAUNE:
                                    jeu->nb_jaune=jeu->nb_jaune-1;
                                    break;
                                case PION_VERT:
                                    jeu->nb_vert=jeu->nb_vert-1;
                                    break;
                                case DAME_BLEU:   
                                    jeu->nb_bleu=jeu->nb_bleu-2; //on considere qu'une dame vaut 2 danss le compte de pions
                                    break;
                                case DAME_ROUGE:
                                    jeu->nb_rouge=jeu->nb_rouge-2;
                                    break;
                                case DAME_JAUNE:
                                    jeu->nb_jaune=jeu->nb_jaune-2;
                                    break;
                                case DAME_VERT:
                                    jeu->nb_vert=jeu->nb_vert-2;
                                    break;
                            }    
                            printf("\nContinuer le coup ? (0:non, 1:oui)");
                            scanf("%d",&reponse);
                            pas_fini=reponse; 
                        
                        }
                        else{
                            printf("Mouvement invalide.\n");
                            pas_fini=0;
                        }
                    }
                    else{
                        printf("Mouvement invalide.\n");
                        pas_fini=0;
                    }
                }
                else{
                printf("Mouvement invalide\n");
                pas_fini=0;
                }
    
            }
        }
    }
    else {
        printf("mouvement interdit.\n");
        pas_fini=1;
    }}while (pas_fini==1);
    

    return jeu;
}

/* 
Fonction qui fait une rotation de 90 degre
*/

Damier* rotationDamier(Damier* jeu,Damier* temp) {
    // Rotation de 90° dans le sens des aiguilles d'une montre
    

    // Remplir la matrice temporaire avec les éléments de la matrice originale
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            //int index[i][j];
            temp->index[j][TAILLE-i-1]=jeu->index[i][j];
        }
    }
    return(temp);
}

/* 
Fonction pour afficher le damier avec une rotation des indices
*/

void afficherDamier(Damier* jeu,Damier* temp, Couleur_joueur couleur) {
    switch(couleur){
            case Rouge: 
                printf("    "); 
                //affiche les indices des colonnes (1 à 16) en haut 
                for (int j = 0; j < TAILLE; j++) {
                    if (j<=9){
                        printf(TXT_BLEU "%d  "RESET, j);
                    }
                    else{
                        printf(TXT_BLEU "%d "RESET, j);
                    }}
                printf("\n");

                for (int i = 0; i < TAILLE; i++) {
                // Affiche les indices des lignes (1 à 16) à gauche
                    //printf(TXT_BLEU " %d "RESET, i);
                    if (i<=9){
                        printf(TXT_BLEU " %d "RESET, i);
                    }
                    else{
                        printf(TXT_BLEU "%d "RESET, i);
                    }

                    for (int j = 0; j < TAILLE; j++) {
                        afficher_Cellule(i, j, temp->index);
                     }
                    printf("\n");
                 }
                break;

            case Jaune: 
                printf("   "); 
                for (int j = 0; j < TAILLE; j++) { // indices colonnes (16 à 1) en haut
                    //printf(TXT_BLEU "%d "RESET,j);
                    if (j<7){
                        printf(TXT_BLEU "%d "RESET, 16-j);
                    }
                    //if(j==9){
                      //  printf(TXT_BLEU "%d  "RESET, 16-j);}
                    else{
                        printf(TXT_BLEU " %d "RESET, 16-j);
                    }
        
                    }
                printf("\n");

                for (int i = 0; i < TAILLE; i++) {
                    // Affiche les indices des lignes (1 à 16) à gauche
                    //printf(TXT_BLEU " %d "RESET,i);
                    if (i<=9){
                        printf(TXT_BLEU " %d "RESET, i);
                    }
                    else{
                        printf(TXT_BLEU "%d "RESET, i);
                    }

                    for (int j = 0; j < TAILLE; j++) {
                        afficher_Cellule(i, j, temp->index);
                     }
                    printf("\n");
                }
                break;
            case Bleu:
                //affiche les indices des colonnes (16 à 1) en haut
                printf("   "); 
                for (int j = 0; j < TAILLE; j++) {
                    if (j<7){
                        printf(TXT_BLEU "%d "RESET, 16-j);
                    }
                    else{
                        printf(TXT_BLEU " %d "RESET, 16-j);
                    }
        
                    }
                printf("\n");

                for (int i = 0; i < TAILLE; i++) {
                    // Affiche les indices des lignes (16 à 1) à gauche
                    //printf(TXT_BLEU " %d "RESET, i);
                    if (i<7){
                        printf(TXT_BLEU "%d "RESET, 16-i);
                    }
                    else{
                        printf(TXT_BLEU " %d "RESET, 16-i);
                    }
                    for (int j = 0; j < TAILLE; j++) {
                        afficher_Cellule(i, j, temp->index);
                     }
                    printf("\n");
                }
                break;
            case Vert:
                printf("    "); 
                //affiche les indices des lignes (1 à 16) en haut
                for (int j = 0; j < TAILLE; j++) {
                    if (j<=9){
                        printf(TXT_BLEU " %d "RESET, j);
                    }
                    else{
                        printf(TXT_BLEU "%d "RESET, j);
                    }
                    }
                printf("\n");

                for (int i = 0; i < TAILLE; i++) {
                    // Affiche les indices des lignes (16 à 1) à gauche
                    //printf(TXT_BLEU " %d "RESET, i);
                    if (i<7){
                        printf(TXT_BLEU "%d "RESET, 16-i);
                    }
                    else{
                        printf(TXT_BLEU " %d "RESET, 16-i);
                    }
                    for (int j = 0; j < TAILLE; j++) {
                        afficher_Cellule(i, j, temp->index);
                     }
                    printf("\n");
                }
                break;
            
    }
}

/* 
Fonction qui fait la rotation selon la couleur du joueur
*/

void Rotation_Couleur(Damier* jeu, Damier* temp1, Damier* temp2,Damier* temp3,Couleur_joueur couleur){

    switch (couleur) {
            case Rouge:
                afficher_Damier(jeu);
                break;
            case Jaune :
                afficherDamier(rotationDamier(jeu,temp1),temp1,couleur);
                break;
            case Bleu:
                afficherDamier(rotationDamier(rotationDamier(jeu,temp1),temp2),temp2,couleur);
                break;
            case Vert:
                afficherDamier(rotationDamier(rotationDamier(rotationDamier(jeu,temp1),temp2),temp3),temp3,couleur);
                break;
            }
}

/* 
Fonction qui fait le jeu: affiche le joueur actuel en bas (rotation) avec le deplacement par couleur(mvt). On a choisi l'ordre des joueurs suivant : Rouge, Jaune, Bleu, Vert
*/

void total(Damier* jeu, bool* fin){
    Damier* nb_bleu,nb_vert,nb_jaune,nb_rouge;
    Damier temp, temp1, temp2, temp3;
    Couleur_joueur couleur;
    int nb_cases_bleu, nb_cases_rouge, nb_cases_jaune,nb_cases_vert;
    switch(nb_cases_bleu+nb_cases_rouge+nb_cases_jaune+nb_cases_vert){
        case Bleu:
            printf("Le joueur Bleu a gagné, Félicitations !");
            *fin=true;
            break;
        case Rouge:
            printf("Le joueur Rouge a gagné, Félicitations !");
            *fin=true;
            break;
        case Jaune:
            printf("Le joueur Jaune a gagné, Félicitations !");
            *fin=true;
            break;
        case Vert:
            printf("Le joueur Vert a gagné, Félicitations !");
            *fin=true;
            break;
        default: 
            *fin=false;
    }
        
    while(*fin==false){
        printf("Nouveau tour\n");
        if (nb_cases_rouge=!0){
            printf("Joueur Rouge\n");
            couleur=Rouge;
            Rotation_Couleur(jeu, &temp1, &temp2, &temp3, couleur);
            mvt(jeu, couleur);
        }
        if (nb_cases_jaune=!0){
            printf("Joueur Jaune\n");
            couleur=Jaune;
            Rotation_Couleur(jeu, &temp1, &temp2, &temp3, couleur);
            mvt(jeu, couleur);
        }
        if (nb_cases_bleu=!0){
            printf("Joueur Bleu\n");
            couleur=Bleu;
            Rotation_Couleur(jeu, &temp1, &temp2, &temp3, couleur);
            mvt(jeu, couleur);
        }
        if (nb_cases_vert=!0){
            printf("Joueur Vert\n");
            couleur=Vert;
            Rotation_Couleur(jeu, &temp1, &temp2, &temp3, couleur);
            mvt(jeu, couleur);
        }
    }
}




int main(){
    Damier jeu;
    Damier* jeux;
    bool fin;
    init_damier(&jeu);
    afficher_Damier(&jeu);
    //afficher_etats_cases(&jeu);
    total(&jeu,&fin);
    return 0;
}
