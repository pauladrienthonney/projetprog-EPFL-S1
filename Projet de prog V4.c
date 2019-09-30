/**
 * \file        289056.c / nom à modifier pour le rendu suivant (section 4)
 * \version        2.4
 * \date        2017-11-15
 * \author        Thonney Paul
 * \brief        Rendu 2 pour le projet "Me too ?" du cours CS-111(c) - 2017
 */

	// *******************************************************************
	//         inclusion de fichiers en-tête avec la directives include

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

	// *******************************************************************
	//               Symboles définis avec la directive define

#define NB_SIM_MIN         1
#define N_MIN             2
#define NB_PERS_MIN     2
#define MAX_CYCLES         200

	//             Symboles définies avec l'instruction enum

	//   L veut dire "ligne" et C veut dire "colonne"

enum{POS_L, POS_C, BUT_L, BUT_C, SANTE, NB_INFOS_PERS};
enum ETAT_SANTE {VIDE, SAIN, VACCINE, CONTAMINE, INCUBATION} ;
	// *******************************************************************
	//                        Functions declaration

	// ########   ne pas modifier ces déclarations de fonctions    #######

/**
 * \brief    Si nbSim n'est pas strictement positif
 * \param   nbSim        la valeur invalide de nbSim
 */
static void erreur_nbSim(int nbSim);

/**
 * \brief    Si la taille du monde n'est pas strictement supérieure à 1
 * \param     n               la valeur invalide
 */
static void erreur_taille_monde(int n);

/**
 * \brief    Si le nombre de personnes n'est pas dans [2, n*n]
 * \param     nbP        la valeur invalide
 * \param     n        la taille du monde
 */

static void erreur_nbP(int nbP, int n);

/**
 * \brief    Si un indice de position ou de but n'est pas dans [0, n-1]
 * \param     indice                 la première valeur d'indice invalide
 * \param     indicePersonne         l'indice de la personne dans [0, nbP-1]
 */

static void erreur_indice_ligne_colonne(int indice, int indicePersonne);

/**
 * \brief    Si deux personnes ont la même position
 * \param     indiceP_A    l'indice de la premiere personne dans [0, nbP-1]
 * \param     indiceP_B    l'indice de la seconde  personne dans [0, nbP-1]
 */
static void erreur_superposition(int indiceP_A, int indiceP_B);

	// ############################ END ##################################

static void lire_verbose(void);
static bool lire_afficher_grille(void);
static void lire_param(int *p_nb_sim, int *p_n, int * p_nb_pers);
static void init_tab_personnes(int n, int nb_pers,
							   int tab_pers[nb_pers][NB_INFOS_PERS]);

static int sgn(int x);
static void nouveau_but(int i, int nb_pers, int n, int tab_pers[nb_pers][NB_INFOS_PERS]);
static bool case_occupee(int col, int ligne, int taille_tableau, int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS]);
static int simulation(int nb_sim, int taille_tableau,
					  int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS]);

static void contexte(int n, int nb_pers, int nb_sim,
					 int tab_pers[nb_pers][NB_INFOS_PERS]);
static double valeur_mediane(int resultat_sim[],int nb_sim);
static void tri(int resultat_sim[],int nb_sim);
static void etat_sante(int nbP_vac,int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS]);
static void affichage_grille(int taille_tableau, int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS]);
	// *******************************************************************
	//                        Global variable

/** variable booléenne déterminant l'affichage des messages d'invitation */
static bool    verbose;
static bool dump_grille;

	// *******************************************************************
	//                        MAIN

int main(void)
{
		// lecture des paramètres généraux du programme
	lire_verbose();
	dump_grille = lire_afficher_grille();
	
	int nb_sim ,  n, nb_pers;
	
		// les pointeurs sont nécessaires pour pouvoir modifier les 3 variables
	lire_param(&nb_sim, &n, &nb_pers);
	
	
		// lecture et test des descriptions des personnes
	int tab_pers[nb_pers][NB_INFOS_PERS];
	init_tab_personnes( n, nb_pers, tab_pers);
	
		//boucle globale de la simulation
	contexte(n, nb_pers, nb_sim, tab_pers);
	
	
	return EXIT_SUCCESS;
}

static void lire_verbose(void)
{
	int dump = 0;
	scanf("%d", &dump);
	verbose = (dump != 0);
}

static bool lire_afficher_grille(void)
{
	int dump = 0;
	if(verbose)
		printf("# Do you want to see the grid ?\n");
	scanf("%d", &dump);
	return dump;
}

	// remarque: les pointeurs sont autorisés pour cette fonction car
	//            elle DOIT pouvoir modifier les 3 variables de main
	//            dont on donne les adresses pour initialiser les 3
	//            pointeurs déclarés comme paramètres formels.

static void lire_param(int *p_nb_sim, int *p_n, int * p_nb_pers)
{
	if(verbose)
		printf("# How many simulations ?\n");
	
		// remarque: la valeur du pointeur p_nb_sim est l'adresse de nb_sim
		//           qui a été founie à l'appel de cette fonction (ligne99).
		//           C'est exactement ce dont a besoin scanf pour ranger
		//           la valeur lue au clavier pour nb_sim (de main).
		//           Donc, on donne simplement la valeur de p_nb_sim à scanf.
	
	scanf("%d", p_nb_sim);
	
		// remarque: ensuite on doit tester si la VALEUR de nb_sim est
		//           correcte. on fait donc une indirection sur le pointeur
		//           p_nb_sim pour obtenir cette valeur car p_nb_sim a reçu
		//           l'adresse de nb_sim comme valeur initiale (ligne 99).
	
	if(*p_nb_sim < NB_SIM_MIN)
	{
		erreur_nbSim(*p_nb_sim);
		exit(EXIT_FAILURE);
	}
	
	if(verbose)
		printf("# How large is the world ?\n");
	scanf("%d", p_n);
	if(*p_n < N_MIN)
	{
		erreur_taille_monde(*p_n);
		exit(EXIT_FAILURE);
	}
	
	if(verbose)
		printf("# How many people ?\n");
	scanf("%d", p_nb_pers);
	if(*p_nb_pers < NB_PERS_MIN || *p_nb_pers > (*p_n) * (*p_n))
	{
		erreur_nbP(*p_nb_pers, *p_n);
		exit(EXIT_FAILURE);
	}
}


static void init_tab_personnes(int n,   int nb_pers,
							   int tab_pers[nb_pers][NB_INFOS_PERS])
{
	int i,j,k ;
	
	for(i = 0 ; i < nb_pers ; i++)
		for(k = 0 ; k < NB_INFOS_PERS-1 ; k++)
		{
			scanf("%d", &tab_pers[i][k]);
			if(tab_pers[i][k] < 0 || tab_pers[i][k] >= n)
			{
				erreur_indice_ligne_colonne(tab_pers[i][k], i);
				exit(EXIT_FAILURE);
			}
		}
	
	for(i = 0 ; i < nb_pers - 1 ; i++)
		for(j = i + 1 ; j < nb_pers ; j++)
			if(tab_pers[i][POS_L] == tab_pers[j][POS_L] &&
			   tab_pers[i][POS_C] == tab_pers[j][POS_C])
			{
				erreur_superposition(i, j);
				exit(EXIT_FAILURE);
			}
}

int sgn(int x) {
	return ( x < 0 ? 1: -1);
}

// cherche un nouveau but qui ne soit pas son but actuel ni sa position
static void nouveau_but(int i, int nb_pers, int n, int tab_pers[nb_pers][NB_INFOS_PERS]) {
	
	int but_actuelC = tab_pers[i][BUT_C];
	int but_actuelL = tab_pers[i][BUT_L];
	int posC = tab_pers[i][POS_C];
	int posL = tab_pers[i][POS_L];
	bool done = 0;
	while (!done) {
		int butC = rand() % n;
		int butL = rand() % n;
		if ((butC != but_actuelC) && (butC != posC) && (butL != but_actuelL) && (butL != posL)) {
			done = 1;
			tab_pers[i][BUT_C] = butC;
			tab_pers[i][BUT_L] = butL;
		}
	}

}

	// retourne l'état actuel d'une personne à la position col,ligne (SAIN, VACCINE, etc...) ou VIDE si la position est vide
static int etat_actuel_sante(int col, int ligne, int taille_tableau, int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS]) {
	int etat_actuel = VIDE;
	
	for (int i=0; i<nb_pers; i++) {
		if ((tab_pers[i][POS_C] == col) && (tab_pers[i][POS_L] == ligne)) {
			etat_actuel = tab_pers[i][SANTE];
		}
	}
	return etat_actuel;
	
}

	// case_occupee(1,2,3,4, tab);
static bool case_occupee(int col, int ligne, int taille_tableau, int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS]) {

	bool occupee = 0;
//	printf("func occupee: %d, %d\n", col, ligne);
	if (col < 0) {
		col = taille_tableau-1;
	}
	if (ligne < 0) {
		ligne = taille_tableau-1;
	}
	for (int i = 0; i<nb_pers; i++) {
		if ((tab_pers[i][POS_C] == col) && (tab_pers[i][POS_L] == ligne)) {
			occupee = 1;
 		}
	}
//	printf("Case %d, %d est %s\n", col, ligne, (occupee ? "occupee":"libre"));
	return occupee;
}

static bool personne_contaminee_autour(int pos_c, int pos_l, int taille_tableau, int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS]) {

	for (int colonne = -1; colonne<=1; colonne++) {
		for (int ligne = -1; ligne<= 1; ligne++) {
			// on ne se teste pas nous-même
			if ((colonne != 0 ) && (ligne != 0)) {
				int ligne_test = pos_l+ligne;
				int col_test = pos_c + colonne;
				if (col_test < 0) {
					col_test = taille_tableau-1;
				}
				if (ligne_test < 0) {
					ligne_test = taille_tableau-1;
				}
				for (int i = 0; i<nb_pers; i++) {
					if ((tab_pers[i][POS_C] == col_test) && (tab_pers[i][POS_L] == ligne_test)) {
						if (tab_pers[i][SANTE] == CONTAMINE) {
							return 1;
						}
					}
				}

			}
		}
	}
	return 0;
}

static int simulation(int nb_sim, int n,
					  int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS])
{
	int cycles = 0;
	int i;
	int distanceCol;
	int distanceLigne;
	
	bool condition_arret = 0;
	while (condition_arret == 0) {
		
		// passe tous les patients en incubation en mode contaminé.
		for (i = 0 ; i < nb_pers - 1 ; i++) {
			if (tab_pers[i][SANTE] == INCUBATION) {
				tab_pers[i][SANTE] = CONTAMINE;
			}
		}
	
		if(dump_grille==1)
		{
			affichage_grille(n, nb_pers, tab_pers);
		}
		
		for (i = 0 ; i < nb_pers ; i++) {
			int posC = tab_pers[i][POS_C];
			int posL = tab_pers[i][POS_L];
			int etat_sante = tab_pers[i][SANTE];
			if (etat_sante == SAIN) {
					// cherche les personnes autour qui sont contaminées
				if (personne_contaminee_autour(posC, posL, n, nb_pers, tab_pers) == 1) {
					tab_pers[i][SANTE] = INCUBATION;
				}			
			}
		}


		for (i = 0 ; i < nb_pers ; i++) {
			bool bouge_ok = 1; // si on peut se déplacer
			
			int posC = tab_pers[i][POS_C];
			int posL = tab_pers[i][POS_L];
			int butC = tab_pers[i][BUT_C];
			int butL = tab_pers[i][BUT_L];
			int etat_sante = tab_pers[i][SANTE];
			
			if (etat_sante == SAIN) {
				// cherche les personnes autour qui sont contaminées
				if (personne_contaminee_autour(posC, posL, n, nb_pers, tab_pers) == 1) {
					tab_pers[i][SANTE] = INCUBATION;
				}
				
			}
			// on est arrivé au but, on se déplace. A voir si ce test se fait à la fin de la boucle ou au début
			if ((posC == butC) && (posL == butL)) {
				nouveau_but(i, nb_pers, n, tab_pers);
				// on a changé BUT, on le recharge
				butC = tab_pers[i][BUT_C];
				butL = tab_pers[i][BUT_L];
			}
			
			int directionLigne = sgn(posL - butL);
			distanceLigne = abs(posL - butL) ;
			if (distanceLigne == 0) {
				directionLigne = 0;
			} else {
				if (abs(n - distanceLigne) < distanceLigne) {
					directionLigne = -directionLigne;
					distanceLigne = n - distanceLigne;
				}
			}
			
			int directionCol = sgn(posC - butC);
			distanceCol = abs(posC - butC) ;
			if (distanceCol == 0) {
				directionCol = 0;
			} else {
				if (abs(n - distanceCol) < distanceCol) {
					directionCol = -directionCol;
					distanceCol = n - distanceCol;
				}
			}
			
			posC = posC + directionCol;
			if (posC < 0) {
				posC = n + posC;
			}
			posL = posL + directionLigne;
			if (posL < 0) {
				posL = n + posL;
			}
			
			// STEPHAN: C'est normal qu'on ait posC ou posL > n ??
			// ca devrait pas arriver, non?
			if (posC >= n) {
				posC = 0;
			}
			if (posL >= n) {
				posL = 0;
			}
			
			// si la case de destination est occupée, chercher une case libre.
			if (case_occupee(posC, posL, n, nb_pers, tab_pers) == 1) {
				bool case_libre_trouvee = 0;
				for (int i = -1; i<=1 && !case_libre_trouvee; i++) {
					for (int j = -1; j<= 1 && !case_libre_trouvee; j++) 
					{
						if (case_occupee(posC+i, posL+j, n, nb_pers, tab_pers) == 0) {
							posC = posC+i;
							posL = posL+j;
							if (posC < 0) {
								posC = n-1;
							}
							if (posL < 0) {
								posL = n-1;
							}
							if (posC >= n) {
								posC = 0;
							}
							if (posL >= n) {
								posL = 0;
							}
							case_libre_trouvee = 1;
						}
					}
				}

				if (case_libre_trouvee == 0) 
				{
						// chercher nouveau but
					nouveau_but(i, nb_pers, n, tab_pers);
					bouge_ok = 0;
				}
			}
			
			if (bouge_ok) 
			{
				tab_pers[i][POS_C] = posC;
				tab_pers[i][POS_L] = posL;
//				fprintf(stderr, "C: %d, L: %d\n", posC, posL);
			} else {
//				fprintf(stderr, "bouge pas\n");
			}
			
		}

		// on compte le nombre de personnes saines restantes.
		int personnes_saines = 0;
		for (i= 0; i<nb_pers; i++) 
		{
			if (tab_pers[i][SANTE] == SAIN)
			{
					personnes_saines++;
			}
			
		}

		// on s'arrete si on a atteint le max de cycles ou si tout le monde est mort
		if ((personnes_saines == 0) || (cycles >= MAX_CYCLES)) 
		{
			condition_arret = 1;
			if (cycles >= MAX_CYCLES) 
			{
				printf("\n MAX_CYCLES ATTEINT\n");
			}
			if (personnes_saines == 0) 
			{
				printf("0 personnes saines\n");
			}
		}
		cycles++;

	}
	if(dump_grille==1)
	{
		affichage_grille(n, nb_pers, tab_pers);
	}
	

	return cycles;
}

	//Fait varier les contextes
static void contexte(int n, int nb_pers,int nb_sim,
					 int tab_pers[nb_pers][NB_INFOS_PERS])
{
	int nbP_prime;
	int nbP_vac;
	int resultat_sim[nb_sim];
	int i;
	double densite = 0;
	double taux_de_vaccination = 0;
	
	
	
	for(nbP_prime=nb_pers;nbP_prime>=2; nbP_prime--)
	{
		for(nbP_vac=0;nbP_vac<=nbP_prime-2;nbP_vac++)
		{
			// relire le tab_pers
			init_tab_personnes( n, nb_pers, tab_pers);
			etat_sante(nbP_vac, nb_pers, tab_pers);
//			for (int i=0; i<nb_pers; i++) {
//				printf("%d, %d, %d, %d, %d\n", tab_pers[i][POS_C], tab_pers[i][POS_L], tab_pers[i][BUT_C], tab_pers[i][BUT_L], tab_pers[i][SANTE]);
//			}

			for(i=0; i<nb_sim; i++)
			{
					//caclul de la densité de population et du taux de vaccination
				densite= (double) nbP_prime/(n*n);
				taux_de_vaccination= (double) (nbP_vac)/nbP_prime;
				resultat_sim[i] = simulation(nb_sim, n,
											 nb_pers, tab_pers);
			}
				//tri
			tri(resultat_sim, nb_sim);
			
				//affichage du résultat
			printf( "%lf %lf %lf \n",densite ,taux_de_vaccination ,
				   valeur_mediane(resultat_sim, nb_sim));
		}
	}
}
	//valeur médiane des nb_sim après tri
static double valeur_mediane(int resultat_sim[],int nb_sim)
{
	
	if(nb_sim%2==0)
	{
		return (resultat_sim[(int) nb_sim/2] +resultat_sim[(int) nb_sim/2-1])/2;
	}
	else
	{
		return resultat_sim[(int) nb_sim/2];
	}
}
	//tri des résultats des nb_sim
static void tri(int resultat_sim[],int nb_sim)
{
	int i,j,jmin,min;
	
	if(nb_sim%2==0)
	{
		for(i=0 ; i <nb_sim/2 ; i++)
		{
			min  = resultat_sim[i];
			jmin = i;
			for(j=i+1 ; j < nb_sim/2+1 ; j++)
			{
				if(resultat_sim[j] < min)
				{
					min = resultat_sim[j];
					jmin= j;
				}
			}
			resultat_sim[jmin] = resultat_sim[i];
			resultat_sim[i]    = min;
			printf("%d\n", resultat_sim[i]);
		}
	}
	else
	{
		for(i=0 ; i <nb_sim/2+1 ; i++)
		{
			min  = resultat_sim[i];
			jmin = i;
			for(j=i+1 ; j < nb_sim/2+2 ; j++)
			{
				if(resultat_sim[j] < min)
				{
					min = resultat_sim[j];
					jmin= j;
				}
			}
			resultat_sim[jmin] = resultat_sim[i];
			resultat_sim[i]    = min;
			printf("%d\n", resultat_sim[i]);
		}
	}
}
	//attribue un état de santé
static void etat_sante(int nbP_vac, int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS])
{
	tab_pers[0][SANTE]=CONTAMINE;
	int i=0;
	for(i=1;i<=nb_pers;i++)
	{
		if(i<=nbP_vac)
		{
			tab_pers[i][SANTE]=VACCINE;
		}
		else
		{
			tab_pers[i][SANTE]=SAIN;
		}
	}
}
	//affichage de la grille
static void affichage_grille(int n, int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS])
{
	int ligne,col;
	
	// on remplit le separateur
	char separateur[2*n+2];
	for (col=0;col<=
		 n;col++) {
		separateur[col*2]='-';
		separateur[col*2+1]='-';
	}
	separateur[2*n+1]=0;
	
	printf("     ");
	int compteur=0;
	for (col = 0; col<n; col++) {
		printf("%d:", compteur);
		compteur++;
		compteur = compteur % 10;
	}
	printf("\n");
	for(ligne=0;ligne<n;ligne++) {
		printf("    %s\n", separateur);
		
		printf("%03d ", ligne);
		for(col=0;col<n;col++)
		{
			char etat_tableau = ' ';
			printf(":");
			int etat_sante_actuel = etat_actuel_sante(col, ligne, n, nb_pers, tab_pers);
			for (int i=0; i<nb_pers; i++) {
				if ((tab_pers[i][BUT_C] == col) && (tab_pers[i][BUT_L] == ligne)) {
					etat_tableau = 'b';
				}
			}
			switch (etat_sante_actuel) {
				case VIDE:
					etat_tableau = ' ';
					break;
				case SAIN:
				case INCUBATION:
					etat_tableau = 'N';
					break;
				case CONTAMINE:
					etat_tableau = 'C';
					break;
				case VACCINE:
					etat_tableau = 'V';
					break;
				default:
					break;
			}
			printf("%c", etat_tableau);
				//etat_sante(nbP_vac, nb_pers, tab_pers);
			
		}
		printf(":\n");
	}
	printf("    %s\n", separateur);
}
	// *******************************************************************

	// ################## Ne pas modifier ces fonctions ##################
	//====================================================================//
	//                                                                      //
	//                ***         Fonctions d'erreurs       ***                      //
	//                                                                      //
	//                      /!\ NE PAS MODIFIER /!\                          //
	//                                                                      //
	//====================================================================//

static void erreur_nbSim(int nbSim)
{
	printf("nbSim (=%d) ne valide pas nbSim > 0 !\n", nbSim);
}

static void erreur_taille_monde(int n)
{
	printf("n (=%d) ne valide pas n > 1 !\n", n);
}

static void erreur_nbP(int nbP, int n)
{
	printf("nbP (=%d) ne valide pas nbP > 1 ET nbP <= %d !\n", nbP, n*n);
}

static void erreur_indice_ligne_colonne(int indice, int indicePersonne)
{
	printf("indice incorrect %d de ligne ou colonne de la personne d'indice %d !\n",
		   indice, indicePersonne);
}

static void erreur_superposition(int indiceP_A, int indiceP_B)
{
		// s'assure d'avoir les indices A et B dans l'ordre croissant
	if(indiceP_B > indiceP_A)
	{
		int temp = indiceP_A;
		indiceP_A = indiceP_B;
		indiceP_B = temp;
	}
	
	printf("les personnes d'indices %d et %d se superposent !\n",
		   indiceP_A, indiceP_B);
}

	// ############################ END ##################################
