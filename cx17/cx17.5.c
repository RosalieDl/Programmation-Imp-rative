#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned idx ; 		// type index
typedef char * str ;		// type string
typedef enum {False, True} bool ; 	

typedef struct node { idx car ; struct node * cdr ; } node, * list ; 
#define nil NULL			// pour faire plus lisp

void usage(str) ;
void indexe(str, idx) ;
int indice(str) ;
bool pareil(str, str) ;
void ajoute_mot(idx, str, idx) ;
void ajoute_ref(idx, idx) ;
void dump(idx) ;
int mots_en_liste(str) ;
list cons(idx, list) ;
void putlist(list) ;

#define maximum 1024		// espace mémoire pour lire une ligne de texte
#define max_mots 4096		// nombre max d'éléments dans la table mot

const str split_chars = "()[]., ;:'!?/\n\t\r\"<>—–`" ;
char *stoplist[128] ;			// mots ignorés --> à placer dans "stoplist.data"

struct ndex
{	str mot ;					// le mot à mémoriser
	list refs; } ;				// sa table de références

struct ndex mots[max_mots] ;	// table des mots indexés
idx mot_libre = 0 ;				// indice du premier emplacement libre 


int main(int k, const str ldc[])
{	mots_en_liste("stoplist.data") ;
	if (k < 2) usage("manque le nom du fichier...") ;
	char ligne[maximum] ;					// espace pour une ligne
	FILE * flux = fopen(ldc[1], "r") ;		// ouvre le texte
	if (! flux) usage("fichier pas bon du tout...") ;
	idx x = 0 ;								// à partir de l'index 0
	while (fgets(ligne, maximum, flux))		// tant qu'il y a du texte
		indexe(ligne, ++x) ;				// indexe la ligne suivante
	fclose(flux) ;	
	dump(mot_libre) ;						// affiche – jusqu'au mot inutilisé
	return 0 ; }


void usage(str message) { fprintf(stderr, "%s\n", message) ; exit(1) ; }


// produit une liste des mots contenus dans un fichier et la stocke dans 
int mots_en_liste(str fichier)		// type ? Valeurs retournées ?
{
	FILE * R = fopen(fichier, "r") ;
	if (! R) return 1 ;				// gérer l'erreur ?
	int x = 0 ;
	char buffer[32] ;
	while (fscanf(R, "%s", buffer) != EOF)	// ajouter and x < 127 ?
		stoplist[x++] = strdup(buffer) ;
	return 0 ;}

// ajoute un élément en début de liste
list cons(idx car, list L)
{	list new = malloc(sizeof(node)) ;
	if (! new) usage("cons : manque de RAM") ; 	// enfin, un peu de sérieux !
	new -> car = car ;
	new -> cdr = L ;
	return new ; }


// fait le boulot d'indexage pour une ligne, fournie en entrée
void indexe(str ligne, idx ref)
{	str mot = strtok(strdup(ligne), split_chars) ;		// récupère le premier mot de la ligne
	while (mot)
	{	int x = indice(mot) ;							// est-ce que le mot est déjà indexé ?
		if (x < 0) ajoute_mot(mot_libre, mot, ref) ;	// si non on ajoute à l'index
		else ajoute_ref(x, ref) ;						// si oui on ajoute juste la nouvelle réf
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
	mots[x].refs = cons(ref, nil) ;		// ajoute la première référence pour ce mot (ligne actuelle)
	++mot_libre ; }


// ajoute une référence pour un mot déjà indexé 
void ajoute_ref(idx x, idx ref)
{	if (mots[x].refs -> car != ref)					// on vérifie que la ligne ne soit pas déjà indexée
		mots[x].refs = cons(ref, mots[x].refs) ;	}	// ajout de la référence



// fonction de comparaison de deux entités ndex (classement alphabétique entre les deux mots)
int compare(void const * E1, void const * E2) 
{	struct ndex const *pE1 = E1;
	struct ndex const *pE2 = E2;
	return strcasecmp(pE1 -> mot, pE2 -> mot); }

// affiche les éléments d'une liste d'entiers
void putlist(list L)
{	if (! L) return ; 
	putlist(L -> cdr) ;
	printf("%i ", L -> car) ; }


// affichage final
void dump(idx k)	// indice du dernier élément de mot [pas nécessaire]
{	qsort(mots, mot_libre, sizeof(struct ndex), compare);
	for (idx x = 0 ; x < k ; ++x)			// scanne les mots un par un
	{	if (mots[x].mot)					// test redondant, en l'état
		{	printf("%s : ", mots[x].mot) ;	// affiche le mot
		putlist(mots[x].refs) ;				// parcourt et affiche ses références
		printf("\n") ; } } }			// termine par un alinéa

