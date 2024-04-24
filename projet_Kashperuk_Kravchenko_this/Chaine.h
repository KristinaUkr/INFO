#ifndef __CHAINE_H__
#define __CHAINE_H__	
#include <stdio.h>

/* Liste chainee de points 
элемент в списке точек и содержит координаты точки*/
typedef struct cellPoint{
  double x,y;                   /* Coordonnees du point */
  struct cellPoint *suiv;       /* Cellule suivante dans la liste */
} CellPoint;

/* Celllule d une liste (chainee) de chaines 
элемент из списка строк, содержащий номер строки и список точек*/
typedef struct cellChaine{
  int numero;                   /* Numero de la chaine */
  CellPoint *points;            /* Liste des points de la chaine */
  struct cellChaine *suiv;      /* Cellule suivante dans la liste */
} CellChaine;

/* L'ensemble des chaines 
структура, содержащая максимальное количество волокон в кабеле, 
количество цепей и список цепей*/
typedef struct {
  int gamma;                    /* Nombre maximal de fibres par cable */
  int nbChaines;                /* Nombre de chaines */
  CellChaine *chaines;          /* La liste chainee des chaines */
} Chaines;

Chaines* lectureChaines(FILE *f);
void ecrireChaines(Chaines *C, FILE *f);
void afficheChainesSVG(Chaines *C, char* nomInstance);
double longueurTotale(Chaines *C);
double longueurChaine(CellChaine *c);
int comptePointsTotal(Chaines *C);
Chaines* generationAleatoire(int nbChaines, int nbPointsChaine, int xmax, int ymax);

#endif	
