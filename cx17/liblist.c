// *******************************************************
// Nom ......... : liblist.c
// Rôle ........ : Fonctions de gestions des listes (pour cx17.7, cx17.8)
// Auteur ...... : Rosalie Duteuil
// Version ..... : V1.1 du 20/08/2023
// Licence ..... : réalisé dans le cadre du cours de Programmation impérative
// ********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node { void *car ; struct node * cdr ; } node, * list ; 
#define nil NULL			// pour faire plus lisp

typedef enum {False, True} bool ; 	

void usage(char *) ;
bool pareil(char *, char *) ;

// ajoute un élément en début de liste
list cons(void * car, list L)
{	list new = malloc(sizeof(node)) ;
	if (! new) usage("cons : manque de RAM") ; 	// enfin, un peu de sérieux !
	new -> car = car ;
	new -> cdr = L ;
	return new ; }


// affiche les éléments d'une liste d'entiers
void putlist(list L)
{	if (! L) return ; 
	putlist(L -> cdr) ;
	printf("%i ", *(int *)(L -> car)) ; }


// Cherche un mot dans une liste
int dans_list(char * mot, list L)
{	if (! L) return 0 ;		// on a pas trouvé le mot
	return (pareil((char *)L -> car, mot) || dans_list(mot, L -> cdr)) ; }

