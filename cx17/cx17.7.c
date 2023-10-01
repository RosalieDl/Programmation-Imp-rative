// *******************************************************
// Nom ......... : cx17.7.c
// Rôle ........ : Indexation des mots d'un fichier
// Auteur ...... : Rosalie Duteuil
// Version ..... : V1.1 du 20/08/2023
// Licence ..... : réalisé dans le cadre du cours de Programmation impérative
// Compilation.. : gcc -Wall -c liblist.c
// 				   ar -r liblist.a liblist.o
// 				   gcc -Wall cx17.7.c liblist.a -o cx17.7
// Usage ....... : Pour exécuter : cx17.7 "source.txt" [-s "stopliste.txt"]
// ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned idx ; 		// type index
typedef char * str ;		// type string
typedef enum {False, True} bool ; 	

// structures de listes
typedef struct node { void *car ; struct node * cdr ; } node, * list ; 
#define nil NULL					// pour faire plus lisp

struct ndex { str mot ; list refs; } ;	

// dans liblist.c
list cons(void *, list) ;
void putlist(list) ;
int dans_list(str, list) ;
int mots_en_liste(str) ;

void usage(str) ;
void ini_stoplist(int, const str*) ;
void indexe(str, idx) ;
int indice(str) ;
bool pareil(str, str) ;
void ajoute_mot(idx, str, idx) ;
void ajoute_ref(idx, idx) ;
void dump(idx) ; 

#define maximum 1024		// espace mémoire pour lire une ligne de texte
#define max_mots 4096		// nombre max d'éléments dans la table mot

const str split_chars = "()[]., ;:'!?/\n\t\r\"<>—–`" ;
list stoplist = nil ;			// mots ignorés --> à placer dans "stoplist.data"

struct ndex mots[max_mots] ;	// table des mots indexés
idx mot_libre = 0 ;				// indice du premier emplacement libre 


int main(int k, const str ldc[])
{	ini_stoplist(k, ldc) ;
	if (k < 2) usage("Usage : indiquer le nom du fichier à indexer...") ;
	char ligne[maximum] ;					// espace pour une ligne
	FILE * flux = fopen(ldc[1], "r") ;		// ouvre le texte
	if (! flux) usage("Erreur : problème avec le fichier...") ;
	idx x = 0 ;								// à partir de l'index 0
	while (fgets(ligne, maximum, flux))		// tant qu'il y a du texte
		indexe(ligne, ++x) ;				// indexe la ligne suivante
	fclose(flux) ;	
	dump(mot_libre) ;						// affiche – jusqu'au mot inutilisé
	return 0 ; }


void usage(str message) { fprintf(stderr, "%s\n", message) ; exit(1) ; }

// remplit la variable stoplist avec le contenu du fichier spécifié en LDC (ou "stoplist.data" par défaut)
void ini_stoplist(int argc, const str args[])
{	int i = 0;
	while (i < argc && strcmp(args[i++],"-s")) ;	// on parcourt les arguments à la recherche de l'option -s
	if (i != argc) mots_en_liste(args[i]) ;			// si -s était spécifié, on récupère les mots de la stoplist
	else mots_en_liste("stoplist.data") ; }			// sinon stoplist par défaut

//stocke les mots contenus dans fichier dans la variable globale stoplist.
int mots_en_liste(char * fichier)		
{	FILE * R = fopen(fichier, "r") ;					// ouverture du fichier
	if (! R) usage ("Erreur : problème avec la stop-liste. Vérifier le fichier stoplist.data ou préciser le chemin avec l'option -s") ;	
	char buffer[32] ;									// espace pour un mot
	while (fscanf(R, "%s", buffer) != EOF)				// tant qu'il reste des mots
		stoplist = cons(strdup(buffer), stoplist) ;	// on enregistre le mot	
	return 0 ;}


// fait le boulot d'indexage pour une ligne, fournie en entrée
void indexe(str ligne, idx ref)
{	str mot = strtok(strdup(ligne), split_chars) ;		// récupère le premier mot de la ligne
	while (mot)
	{	if (!dans_list(mot, stoplist)) 					// si le mot n'est pas dans la stoplist, on indexe :
		{	int x = indice(mot) ;							// est-ce que le mot est déjà indexé ?
			if (x < 0) ajoute_mot(mot_libre, mot, ref) ;	// si non on ajoute à l'index
			else ajoute_ref(x, ref) ; } ;					// si oui on ajoute juste la nouvelle réf
		mot = strtok(NULL, split_chars) ; } }			// on passe au mot suivant


// Renvoie l'indice d'un mot donné dans l'index, -1 s'il n'est pas indexé
int indice(str mot)
{	idx x ;
	for (x = 0; mots[x].mot ; ++x) 					// ce mot est-il déjà mémorisé ?
		if (pareil(mot, mots[x].mot)) return x ; 	// oui : retourne cet index
	return -1 ; }								// non : retourne négatif


// compare deux chaînes de caractères, renvoie un booléen (insensible à la casse)
bool pareil(str x, str y) { return strcasecmp(x, y) ? False : True ; }	// cf typedef bool en haut


// ajoute un mot nouveau dans les tables mots et refs, à l'indice donné ; màj indice de prochain emplacement libre
void ajoute_mot(idx x, str mot, idx ref)
{	mots[x].mot = mot ;					// stocke le mot à l'emplacement x
	int * ligne = malloc(sizeof(int)) ;
	if (! ligne) usage("cons : manque de RAM") ; 
	*ligne = ref ;
	mots[x].refs = cons(ligne, nil) ;	// ajoute la première référence pour ce mot (ligne actuelle)
	++mot_libre ; }


// ajoute une référence pour un mot déjà indexé (+ màj du compteur ref_libre)
void ajoute_ref(idx x, idx ref)
{	if (*(int *)(mots[x].refs -> car) != ref) {			// on vérifie que la ligne ne soit pas déjà indexée
		int * ligne = malloc(sizeof(int)) ;
		if (! ligne) usage("cons : manque de RAM") ; 
		*ligne = ref ;
		mots[x].refs = cons(ligne, mots[x].refs) ;	}} 	// ajout de la référence


// fonction de comparaison de deux entités ndex (classement alphabétique entre les deux mots)
int compare(void const * E1, void const * E2) 
{	struct ndex const *pE1 = E1;
	struct ndex const *pE2 = E2;
	return strcasecmp(pE1 -> mot, pE2 -> mot); }


// affichage final
void dump(idx k)	// indice du dernier élément de mot [pas nécessaire]
{	qsort(mots, mot_libre, sizeof(struct ndex), compare);
	for (idx x = 0 ; x < k ; ++x)			// scanne les mots un par un
	{	if (mots[x].mot)					// test redondant, en l'état
		{	printf("%s : ", mots[x].mot) ;	// affiche le mot
		putlist(mots[x].refs) ;				// parcourt et affiche ses références
		printf("\n") ; } } }				// termine par un alinéa
