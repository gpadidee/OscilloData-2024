
/*      Thomas Claux, Jad Tijani

        Code source C.c



        Compilé sur Windows 10

        Fonction en C permettant, à partir d'un fichier binaire contenant des traces, de retrouver la position de chaque pixel des 5 plus grandes traces
        ainsi que la position des pixels des coins (Corners).


        Dépendance : Fichier Pixmap.bin

        © Thomax Claux, Jad Tijani, 2O24

 */





#include <stdio.h>
#include <stdlib.h>



void HISTROGRAME(int * tableau_couleurs, char * argv[], short * Hauteur, short * Largeur) {//histogramme est la fonction qui compte le nombre d'occurence des pixels de chaque couleur possible

    FILE *Pixmap=fopen(argv[1],"rb");
    if (Pixmap==NULL) {printf("Pixmap.bin not found\n");fprintf(stderr, "Pixmap.bin not found");exit(1);}
// vérification que ca s'ouvre

    short profondeur;

    fread(&profondeur,sizeof(short),1,Pixmap);
    fread(Largeur,sizeof(short),1,Pixmap);
    fread(Hauteur,sizeof(short),1,Pixmap);
    float ratio = (float)(*Largeur) / (float)(*Hauteur);
    if(ratio < 0.5 || ratio > 5.0){printf("Ratio de l'image irrealiste\n");fprintf(stderr, "Ratio irrealiste");exit(15);} //On check si le ratio de l'image est bon
    if (profondeur!=8){printf("Profondeur incorect\n");fprintf(stderr, "Profondeur incorect");exit(2);} // Verification de la bonne profondeur
    if (*Largeur < 100 || *Largeur > 1000){printf("Largeur out of bounds\n");fprintf(stderr, "Largeur out of bounds");exit(3);}// Verification de la bonne largeur
    if (*Hauteur < 100 || *Hauteur > 1000){printf("Hauteur out of bounds\n");fprintf(stderr, "Hauteur out of bounds");exit(4);}// Verification de la bonne hauteur
//on lis les premieres variables et on les stock

    unsigned char pixel;
    int pixels_lus=0;

    while ((fread(&pixel, sizeof(unsigned char), 1, Pixmap))) {
        tableau_couleurs[pixel]++;
        pixels_lus++;//on compte combien on a lu de pixels pour s'assurer d'en avoir le bon nombre et on incrémente le nombre d'occurence de la couleur du pixel lu pour tou les pixels du fichier.
    }

    int a = (int)*Largeur;
    int b = (int)*Hauteur;
    int c = a*b;//on converti la hauteur et la largeur en int car dans le cas le plus volumineux on aurait 1O^6 pixels ce qui ne rentre pas dans un short

    if (pixels_lus<c){{printf("to few pixels read, %d instead of %d\n", pixels_lus, c);fprintf(stderr, "too few pixels");exit(5);}}
    if (pixels_lus>c){{printf("to many pixels read, %d instead of %d\n", pixels_lus, c);fprintf(stderr, "too many pixels");exit(6);}} //Erreur si mauvaix nombre de pixels

    fclose(Pixmap);

}

int couleur_trace(int *tableau_couleurs, short*Largeur, short*Hauteur, int*Traces, int* nbr) {
    int couleur_coin=-1;//on prend une valeur impossible pour couleur coins pour vérifier a la fin de la fonction qu'on a bien identié 4 coins
    int compte_traces=0;
    int coins_trouve=0;
    int RTS = 0;


    for (int z=0; z<256; z++) {
        if (tableau_couleurs[z]==4 && coins_trouve!=0){printf("too much corners detected"); fprintf(stderr,"too much corners detected");exit(9);}
// i on a deja des coins et qu'on trouve un autre candidat ;e programme s'arrete
        if (tableau_couleurs[z]==4 && coins_trouve==0) {
            couleur_coin = z;

            coins_trouve = 1;//sinon on sauvegarde la couleur des coins et on change couleur coin pour etre sur que c'est bon
        }

        if (tableau_couleurs[z]>=50 && tableau_couleurs[z]<=300) {
            compte_traces++;
        }
    }

    int *tosort = (int*)calloc(compte_traces,sizeof (int)); //On cherche les traces
    if (tosort==NULL) {printf("Failed to create tosort"); fprintf(stderr,"Error creating tosort");exit(7);}


    for (int z=0; z<256; z++) {
        if (tableau_couleurs[z]>=50 && tableau_couleurs[z]<=300) {
            tosort[RTS]=z;
            RTS++; // On remplie le tableau tosort pour stocker identifiants/couleurs de chaque trace
        }
    }


    for (int i = 0; i < compte_traces - 1; i++) { //On trie les identifiants/couleurs avec les traces ayant le plus de pixels au moins
        for (int j = 0; j < compte_traces - i - 1; j++) {
            if (tableau_couleurs[tosort[j]] < tableau_couleurs[tosort[j + 1]]) {
                // Swap the indices
                int temp = tosort[j];
                tosort[j] = tosort[j + 1];
                tosort[j + 1] = temp;
            }
        }
    }



    for(int j=0; j<5; j++) {

            Traces[j]=tosort[j]; //On garde que les 5 plus grandes traces
        }


    if (couleur_coin==-1) {printf("no corners detected"); fprintf(stderr,"no corners detected");exit(12);}// Verification de la présence de coins
    if (compte_traces==0) {printf("no trace detected"); fprintf(stderr,"no trace detected");exit(8);}//Vérfication de la présence de traces
    if (compte_traces>10) {printf("more then 10 traces"); fprintf(stderr,"more then 10 traces");exit(14);}//Vérification si il y a trop de traces
    if (compte_traces>5){*nbr =5;}
    if (compte_traces<=5){*nbr = compte_traces;}
    return couleur_coin;
    }

int coordonnee(char*argv[],int couleur_coins, int* Traces, int* nbr_courbes) {
    FILE* Pixmap=fopen(argv[1],"rb");
    FILE* Courbes=fopen("Traces.txt","w");

    if (Pixmap==NULL) {printf(stderr,"Pixmap.bin not found\n");exit(1);}//On check que pixmab est bien présente
    if (Traces==NULL) {printf(stderr,"Traces.txt not found\n");exit(10);} //On check que traces est bien écrite

    short profondeur;
    fread(&profondeur,sizeof(short),1,Pixmap);
    short Largeur;
    fread(&Largeur,sizeof(short),1,Pixmap);
    short Hauteur;
    fread(&Hauteur,sizeof(short),1,Pixmap);
//pour la simplicité du code vu qu'on doit déja relire le fichier pour trouver les coordonnées qui décrivent un point de la couleur soit des traces soit des coins on prend hauteur et largeur exactement comme dans histograme
    unsigned char pixel;

    int a = (int)Largeur;
    int b = (int)Hauteur;
    int c = a*b;

    fprintf(Courbes,"Corners = [\n "); // On écrit les coordonnées des corners
    for(int i=0;i<c;i++) {
        fread(&pixel,sizeof(unsigned char),1,Pixmap);
        if (pixel==(unsigned char)couleur_coins) {
            int x= i % a;//Calcul coordonnées en x
            int y=b-( i / a);//Calcul coordonnées en y
            fprintf(Courbes,"%d,%d;\n",x,y);
        }
    }//Les coordonnées des coins sont écrits en premier puis les coordonnees des traces une par une
    fprintf(Courbes,"];\n ");

    for(int j=0; j<(*nbr_courbes); j++) {//On lit tout le fichier pour toutes les traces
        fprintf(Courbes, "C%d =", j);// On note le numéro de la trace avant d'écrire la position des pixels dans le fichier
        fprintf(Courbes,"[");
        fseek(Pixmap,6,SEEK_SET);//on remet le pointeur au premier pixel pour lire tout les pixels
        for (int k=0;k<c;k++) {
            fread(&pixel,sizeof(unsigned char),1,Pixmap);
            if( pixel == (unsigned char)Traces[j] ) {
                int x= k % a; //Calcul de a position en x
                int y=b-( k / a);//Calcul de la position en y
                fprintf(Courbes,"%d, %d;\n",x,y);//on ecrit les coordonnées du point qui a la bonne couleur quand on l'a trouvé
            }
        }

        fseek(Pixmap,6,SEEK_SET);
        fprintf(Courbes,"];\n"); // On ferme le crochet pour la prochaine trace qui va etre écrite
    }
    fprintf(Courbes,"Traces = {");
    for (int k=0;k<(*nbr_courbes); k++) {
        fprintf(Courbes," C%d",k);//On liste chaque trace et son identifiant à la fin du fichier
    }
    fprintf(Courbes,"};\n ");

    fclose(Pixmap); //On ferme pixmap pour éviter des erreurs
    fclose(Courbes);//On ferme Courbes pour éviter des erreurs

}

int main(int argc, char* argv[]){
    if (argc!=2) {printf("incorect number of parameters, %d",argc); fprintf(stderr,"incorrect number of parameters");exit(100);}
//on vérifie qu'on a le bon nombre d'argument dans l'appel du fichier C
    int * tableau = (int*) calloc(256,sizeof(int));
    int * Traces = (int*) calloc(5,sizeof(unsigned char));//on créé les tableaux dynamiques dans main car plus simple pour gérer la libération de la mémoire a la fin du code
    int nbr_courbes=0;
    short large;
    short haut;

    HISTROGRAME(tableau, argv, &large, &haut);
    int coins = couleur_trace(tableau, &large, &haut, Traces, &nbr_courbes);
    printf("C:%d\n ",coins);
    printf("T:");
    for(int i=0;i<nbr_courbes;i++){printf("%d ", Traces[i] );}
    printf("\n");
    coordonnee(argv, coins, Traces,&nbr_courbes);
    //on execute chaque fonction dans le bon ordre et les choses a modifier ou afficher sont directement écrites dans les fonctions concernees

    free (tableau);
    free (Traces);//on libère pour pas de Memory leak
    }
