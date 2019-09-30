/**
 * \file        289056.c 
 * \version        1.6
 * \date        2017-11-15
 * \author        Thonney Paul
 * \brief        Rendu 2 pour le projet "Me too ?" du cours CS-111(c) -07/12/2017
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
							   int tab_ref[nb_pers][NB_INFOS_PERS]);
static void remplir_tab_pers(int nb_pers, int tab_ref[nb_pers][NB_INFOS_PERS],
							 int tab_pers[nb_pers][NB_INFOS_PERS]);
static int sgn(int x);
static void nouveau_but(int i, int nb_pers, int n,
						int tab_pers[nb_pers][NB_INFOS_PERS]);
static bool case_occupee(int col, int ligne, int taille_tableau, int nb_pers,
						 int tab_pers[nb_pers][NB_INFOS_PERS], int nbP_prime);
static bool personne_contaminee_autour(int pos_c, int pos_l,
									   int taille_tableau,int nb_pers,
									   int tab_pers[nb_pers][NB_INFOS_PERS],
									   int nbP_prime);
static void contamine_pers_autour(int pos_c, int pos_l,
								  int taille_tableau,int nb_pers,
								  int tab_pers[nb_pers][NB_INFOS_PERS],
								  int nbP_prime);
static void incubation_contamination(int nb_pers,
									 int tab_pers[nb_pers][NB_INFOS_PERS],
									 int nbP_prime);
static int nb_pers_saines(int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS], 
						  int nbP_prime);
static void deplacement(int posC, int posL, int butC, int butL,int nb_pers,
						int tab_pers[nb_pers][NB_INFOS_PERS], int nbP_prime, int n,
						int indice);
static int simulation(int nb_sim, int taille_tableau,
					  int nb_pers,
					  int tab_pers[nb_pers][NB_INFOS_PERS], int nbP_prime);

static void contexte(int n, int nb_pers, int nb_sim,
					 int tab_ref[nb_pers][NB_INFOS_PERS]);
static double valeur_mediane(int resultat_sim[],int nb_sim);
static void tri(int resultat_sim[],int nb_sim);
static void init_etat_sante(int nbP_vac,int nb_pers,
													  int tab_pers[nb_pers][NB_INFOS_PERS]);
static void affichage_grille(int taille_tableau, int nb_pers,
							 int tab_pers[nb_pers][NB_INFOS_PERS], int nbP_prime);
static int etat_actuel_sante(int col, int ligne, int taille_tableau,
							 int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS],
							 int nbP_prime);
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

		// lit les paramètres en entrée
	lire_param(&nb_sim, &n, &nb_pers);


		// lecture et test des descriptions des personnes
	int tab_ref[nb_pers][NB_INFOS_PERS];
	init_tab_personnes( n, nb_pers, tab_ref);

		//boucle globale des contexte plus simulation
	contexte(n, nb_pers, nb_sim, tab_ref);


	return EXIT_SUCCESS;
}
	//lis et enregistre le choix quand à l'affichage des messages
static void lire_verbose(void)
{
	int dump = 0;
	scanf("%d", &dump);
	verbose = (dump != 0);
}
	//lis et mémorise l'affichage de la grille
static bool lire_afficher_grille(void)
{
	int dump = 0;
	if(verbose)
		printf("# Do you want to see the grid ?\n");
	scanf("%d", &dump);
	return dump;
}
	// lis et enregistre le nombre de simulations 
static void lire_param(int *p_nb_sim, int *p_n, int * p_nb_pers)
{
	if(verbose)
		printf("# How many simulations ?\n");

	scanf("%d", p_nb_sim);

		// on vérifie la valeur de nbSim
	if(*p_nb_sim < NB_SIM_MIN)
	{
		erreur_nbSim(*p_nb_sim);
		exit(EXIT_FAILURE);
	}
	// lis et enregistre la taille du monde
	if(verbose)
		printf("# How large is the world ?\n");
	scanf("%d", p_n);
	if(*p_n < N_MIN)
	{
		erreur_taille_monde(*p_n);
		exit(EXIT_FAILURE);
	}
	// lis et enregistre le nombre de personnes
	if(verbose)
		printf("# How many people ?\n");
	scanf("%d", p_nb_pers);
	if(*p_nb_pers < NB_PERS_MIN || *p_nb_pers > (*p_n) * (*p_n))
	{
		erreur_nbP(*p_nb_pers, *p_n);
		exit(EXIT_FAILURE);
	}
}

	// // lis et enregistre les indices des personnes dans un tableau de référence
static void init_tab_personnes(int n,   int nb_pers,
							   int tab_ref[nb_pers][NB_INFOS_PERS])
{
	int i,j,k ;

	for(i = 0 ; i < nb_pers ; i++)
	{
		for(k = 0 ; k < NB_INFOS_PERS-1 ; k++)
		{
			scanf("%d", &tab_ref[i][k]);
			if(tab_ref[i][k] < 0 || tab_ref[i][k] >= n)
			{
				erreur_indice_ligne_colonne(tab_ref[i][k], i);
				exit(EXIT_FAILURE);
			}
		}
	}
	for(i = 0 ; i < nb_pers - 1 ; i++)
	{
		for(j = i + 1 ; j < nb_pers ; j++)
		{
			if(tab_ref[i][POS_L] == tab_ref[j][POS_L] &&
			   tab_ref[i][POS_C] == tab_ref[j][POS_C])
			{
				erreur_superposition(i, j);
				exit(EXIT_FAILURE);
			}
		}
	}
}
	/*remplis le tab_pers utilisé dans la simulation
	 *  par les valeurs de références luent en entrée*/
static void remplir_tab_pers(int nb_pers, int tab_ref[nb_pers][NB_INFOS_PERS],
							 int tab_pers[nb_pers][NB_INFOS_PERS])
{

	for(int i = 0; i < nb_pers; i++)
	{
		for(int j=0; j<NB_INFOS_PERS;j++)
		{
			tab_pers[i][j] = tab_ref[i][j];
		}
	}
}
	//création de la fonction signe utilisée dans la simulation
int sgn(int x)
{
	return ( x < 0 ? 1: -1);
}

// cherche un nouveau but qui ne soit pas son but actuel ni sa position
static void nouveau_but(int i, int nb_pers, int n,
						int tab_pers[nb_pers][NB_INFOS_PERS])
{
	int but_actuelC = tab_pers[i][BUT_C];
	int but_actuelL = tab_pers[i][BUT_L];
	int posC = tab_pers[i][POS_C];
	int posL = tab_pers[i][POS_L];
	bool done = 0;
	while (!done)
	{
		int butC = rand() % n;
		int butL = rand() % n;
		if ((butC != but_actuelC) && (butC != posC) &&
			(butL != but_actuelL) && (butL != posL))
		{
			done = 1;
			tab_pers[i][BUT_C] = butC;
			tab_pers[i][BUT_L] = butL;
		}
	}

}

	/* retourne l'état actuel d'une personne à la position col,
	ligne (SAIN, VACCINE, etc...) ou VIDE si la position est vide*/

static int etat_actuel_sante(int col, int ligne, int taille_tableau,
							 int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS],
							 int nbP_prime)
{
	int etat_actuel = VIDE;

	for (int i=0; i<nbP_prime; i++)
	{
		if ((tab_pers[i][POS_C] == col) && (tab_pers[i][POS_L] == ligne))
		{
			etat_actuel = tab_pers[i][SANTE];
		}
	}
	return etat_actuel;

}

	// vérifie si la case est occupée ou non
static bool case_occupee(int col, int ligne, int taille_tableau, int nb_pers,
						 int tab_pers[nb_pers][NB_INFOS_PERS], int nbP_prime)
{
	bool occupee = 0;

	if (col < 0)
		col = taille_tableau-1;
	if (ligne < 0)
		ligne = taille_tableau-1;
	if (ligne==taille_tableau)
		ligne = 0;
	if (col==taille_tableau)
		col = 0;
	
	for (int i = 0; i<nbP_prime; i++)
	{
		if ((tab_pers[i][POS_C] == col) && (tab_pers[i][POS_L] == ligne))
		{
			occupee = 1;
 		}
	}
	return occupee;
}
 // check si une personne contaminée se trouve sur une case adjacente
static bool personne_contaminee_autour(int pos_c, int pos_l,
									   int taille_tableau,int nb_pers,
									   int tab_pers[nb_pers][NB_INFOS_PERS],
									   int nbP_prime)
{
	for (int colonne = -1; colonne<=1; colonne++)
	{
		for (int ligne = -1; ligne<= 1; ligne++)
		{
			// on ne se teste pas nous-même
			if ((colonne != 0 ) || (ligne != 0))
			{
				int ligne_test = (pos_l+ligne+taille_tableau) % taille_tableau;
				int col_test = (pos_c + colonne+taille_tableau) % taille_tableau;
				for (int i = 0; i<nbP_prime; i++)
				{
					if ((tab_pers[i][POS_C] == col_test) &&
						(tab_pers[i][POS_L] == ligne_test))
					{
						if (tab_pers[i][SANTE] == CONTAMINE)
						{
							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}
	// contamine les personnes autour d'un contaminé
static void contamine_pers_autour(int pos_c, int pos_l,
									   int taille_tableau,int nb_pers,
									   int tab_pers[nb_pers][NB_INFOS_PERS],
									   int nbP_prime)
{
	for (int colonne = -1; colonne<=1; colonne++)
	{
		for (int ligne = -1; ligne<= 1; ligne++)
		{
			// on ne se teste pas nous-même
			if ((colonne != 0 ) || (ligne != 0))
			{
				int ligne_test = (pos_l+ligne) % taille_tableau;
				int col_test = (pos_c + colonne) % taille_tableau;
				if (col_test < 0)
				{
					col_test = taille_tableau-1;
				}
				if (ligne_test < 0)
				{
					ligne_test = taille_tableau-1;
				}
				for (int i = 0; i<nbP_prime; i++)
				{
					if ((tab_pers[i][POS_C] == col_test) &&
						(tab_pers[i][POS_L] == ligne_test))
					{
						if (tab_pers[i][SANTE] == SAIN)
						{
							tab_pers[i][SANTE] = INCUBATION;
						}
					}
				}
			}
		}
	}
}
	// passe les incubé en contaminé
static void incubation_contamination(int nb_pers,
									 int tab_pers[nb_pers][NB_INFOS_PERS],
									 int nbP_prime)
	{
		 for (int i = 0 ; i < nbP_prime ; i++)
		{
			if (tab_pers[i][SANTE] == INCUBATION)
			{
				tab_pers[i][SANTE] = CONTAMINE;
			}
		}
	}
	// compte les personnes saines
static int nb_pers_saines(int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS], 
						  int nbP_prime)
{
	int nb=0;
	for (int i= 0; i<nbP_prime; i++)
	{
		if (tab_pers[i][SANTE] == SAIN)
		{
			nb++;
		}
	}
	return nb;
}
static void deplacement(int posC, int posL, int butC, int butL,int nb_pers,
						int tab_pers[nb_pers][NB_INFOS_PERS], int nbP_prime, int n, 
						int indice)
{
	bool bouge_ok = 1; // si on peut se déplacer
	int directionLigne = sgn(posL - butL);
	int distanceLigne = abs(posL - butL) ;
	if (distanceLigne == 0) 
		directionLigne = 0;
	else if (abs(n - distanceLigne) < distanceLigne)
	{
		directionLigne = -directionLigne;
		distanceLigne = n - distanceLigne;
	}
	int directionCol = sgn(posC - butC);
	int distanceCol = abs(posC - butC) ;
	if (distanceCol == 0)
		directionCol = 0;
	else if (abs(n - distanceCol) < distanceCol)
	{
		directionCol = -directionCol;
		distanceCol = n - distanceCol;
	}
	posC = (posC + directionCol + n)%n;
	posL = (posL + directionLigne + n)%n;
	if (case_occupee(posC, posL, n, nb_pers, tab_pers, nbP_prime) == 1)
	{ // si la case de destination est occupée, chercher une case libre.
		bool case_libre_trouvee = 0;
		for (int i = -1; i<=1 && !case_libre_trouvee; i++)
			for (int j = -1; j<= 1 && !case_libre_trouvee; j++)
				if (!case_occupee(posC+i, posL+j, n, nb_pers, tab_pers, nbP_prime))
				{
					posC = (n+posC+i)%n;
					posL = (n+posL+j)%n;
					case_libre_trouvee = 1;
				}
		if (case_libre_trouvee == 0)
		{ // chercher nouveau but
			nouveau_but(indice, nb_pers, n, tab_pers);
			bouge_ok = 0;
		}
		if (bouge_ok)
		{
			tab_pers[indice][POS_C] = posC;
			tab_pers[indice][POS_L] = posL;
		}
	}
	else
	{
		tab_pers[indice][POS_C] = posC;
		tab_pers[indice][POS_L] = posL;
	}
}
// simulation avec mise a jour asychrone
static int simulation(int nb_sim, int n,
					  int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS], int nbP_prime)
{
	int cycles = 0;
	int i;
	bool condition_arret = 0;
	int personnes_saines = nb_pers_saines(nb_pers, tab_pers, nbP_prime);
	if(personnes_saines == 0)
    { // Si aucun sain : on passe de incubé à contaminé
        incubation_contamination(nb_pers, tab_pers, nbP_prime);
		if(dump_grille) //affichage éventuel de la grille
            affichage_grille(n, nb_pers, tab_pers, nbP_prime);
        return 0;  // retour de 0 cycles
    }
	while (condition_arret == 0)
	{// passe tous les patients en incubation en mode contaminé.
		 incubation_contamination(nb_pers, tab_pers, nbP_prime);
		if(dump_grille==1)// affiche la grille
			affichage_grille(n, nb_pers, tab_pers, nbP_prime);
		for (i = 0 ; i < nbP_prime ; i++)
		{
			int etat_sante = tab_pers[i][SANTE];
			if ((tab_pers[i][POS_C] == tab_pers[i][BUT_C]) && 
				(tab_pers[i][POS_L] == tab_pers[i][BUT_L]))
			{// on est arrivé au but, on change
				nouveau_but(i, nb_pers, n, tab_pers);
			}
			deplacement(tab_pers[i][POS_C], tab_pers[i][POS_L], tab_pers[i][BUT_C], 
						tab_pers[i][BUT_L], nb_pers, tab_pers, nbP_prime, n, i);
			//contamine les personne saine sur son chemin
			if (etat_sante == CONTAMINE)
				contamine_pers_autour(tab_pers[i][POS_C], tab_pers[i][POS_L], n, 
									  nb_pers, tab_pers, nbP_prime);
			if (etat_sante == SAIN){
				// cherche les personnes autour qui sont contaminées et se contamine
				if (personne_contaminee_autour(tab_pers[i][POS_C], tab_pers[i][POS_L], 
											   n, nb_pers, tab_pers, nbP_prime))
					tab_pers[i][SANTE] = INCUBATION;
			}
			
		}
		// on compte le nombre de personnes saines restantes.
		personnes_saines = nb_pers_saines(nb_pers, tab_pers, nbP_prime);
		/* on s'arrete si on a atteint le max de
		 cycles ou si tout le monde est contaminé*/
		if ((personnes_saines == 0) || (cycles >= MAX_CYCLES))
			condition_arret = 1;
		cycles++;
	}
	if(dump_grille==1)
	{//on passe de incubé à contaminé
        incubation_contamination(nb_pers, tab_pers, nbP_prime);
        affichage_grille(n, nb_pers, tab_pers, nbP_prime);// on affiche la grille
	}
	return cycles >= MAX_CYCLES ? MAX_CYCLES : cycles;
}

	//Fait varier les contextes
static void contexte(int n, int nb_pers,int nb_sim,
					 int tab_ref[nb_pers][NB_INFOS_PERS])
{
	int nbP_prime;
	int nbP_vac;
	int resultat_sim[nb_sim];
	int i, j;
	double densite = 0;
	double taux_de_vaccination = 0;
	int tab_pers[nb_pers][NB_INFOS_PERS];


	for(nbP_prime=nb_pers;nbP_prime>=2; nbP_prime--)
	{
		for(nbP_vac=0;nbP_vac<=nbP_prime-2;nbP_vac++)
		{



			for(i=0; i < nb_sim; i++)
			{
					// remplir tab_pers avec les valeurs de dŽpart
				remplir_tab_pers(nb_pers,tab_ref, tab_pers);
				init_etat_sante(nbP_vac, nb_pers, tab_pers);

					// contamine avant de commencer le déplacement
				for (j = 0 ; j < nbP_prime ; j++)
				{
					int posC = tab_pers[j][POS_C];
					int posL = tab_pers[j][POS_L];
					int etat_sante = tab_pers[j][SANTE];
					if (etat_sante == SAIN)
					{
							// cherche s'il y a des personnes autour qui sont contaminées.
							// si oui, on passe en mode incubation
						if (personne_contaminee_autour(posC, posL, n, nb_pers,
													   tab_pers, nbP_prime) == 1)
						{
							tab_pers[j][SANTE] = INCUBATION;
						}
					}
				}
				resultat_sim[i] = simulation(nb_sim, n,
											 nb_pers, tab_pers, nbP_prime);
			}
				//caclul de la densité de population et du taux de vaccination
			densite= (double) nbP_prime/(n*n);
			taux_de_vaccination= (double) (nbP_vac)/nbP_prime;


				//tri
			tri(resultat_sim, nb_sim);

				//affichage du résultat
			printf( "%-10g %-10g %-10g \n",densite ,taux_de_vaccination ,
				   valeur_mediane(resultat_sim, nb_sim));
		}
		printf("\n");
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


	for(i=0 ; i <= nb_sim/2 ; i++)
	{
		min  = resultat_sim[i];
		jmin = i;
		for(j=i+1 ; j < nb_sim ; j++)
		{
			if(resultat_sim[j] < min)
			{
				min = resultat_sim[j];
				jmin= j;
			}
		}
		resultat_sim[jmin] = resultat_sim[i];
		resultat_sim[i]    = min;
	}

}
	//attribue un état de santé
static void init_etat_sante(int nbP_vac, int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS])
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
static void affichage_grille(int n, int nb_pers, int tab_pers[nb_pers][NB_INFOS_PERS],
							 int nbP_prime)
{
	int ligne,col;

	// on remplit le separateur
	char separateur[2*n+2];
	for (col=0;col<=
		 n;col++)
	{
		separateur[col*2]='-';
		separateur[col*2+1]='-';
	}
	separateur[2*n+1]=0;

	printf("     ");
	int compteur=0;
	for (col = 0; col<n; col++)
	{
		printf("%d:", compteur);
		compteur++;
		compteur = compteur % 10;
	}
	printf("\n");
	for(ligne=0;ligne<n;ligne++)
	{
		printf("    %s\n", separateur);

		printf("%03d ", ligne);
		for(col=0;col<n;col++)
		{
			char etat_tableau = ' ';
			printf(":");
			int etat_sante_actuel=etat_actuel_sante(col, ligne, n, nb_pers, tab_pers,
													nbP_prime);
			for (int i=0; i<nbP_prime; i++)
			{
				if ((tab_pers[i][BUT_C] == col) && (tab_pers[i][BUT_L] == ligne))
				{
					etat_tableau = 'b';
				}
			}
			switch (etat_sante_actuel)
			{
				case VIDE:
					break;
				case INCUBATION:
				case SAIN:
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
