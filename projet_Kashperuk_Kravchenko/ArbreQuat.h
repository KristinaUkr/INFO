#ifndef __ARBRE_QUAT_H__
#define __ARBRE_QUAT_H__
#include "Reseau.h"

/* Квадратичное дерево, которое содержит узлы сети*/
typedef struct arbreQuat{
    double xc, yc;          /* Координаты центра ячейки */	
    double coteX;           /* Длина ячейки*/
    double coteY;           /* Высота ячейки */
    Noeud* noeud;           /* Указатель на узел цепи*/
    struct arbreQuat *so;   /* Sous-arbre sud-ouest, pour x < xc et y < yc */
    struct arbreQuat *se;   /* Sous-arbre sud-est, pour x >= xc et y < yc */
    struct arbreQuat *no;   /* Sous-arbre nord-ouest, pour x < xc et y >= yc */
    struct arbreQuat *ne;   /* Sous-arbre nord-est, pour x >= xc et y >= yc */
} ArbreQuat;

void chaineCoordMinMax(Chaines* C, double* xmin, double* ymin, double* xmax, double* ymax); /*определяет мин и макс между точками, составляющими разные цепочки сети*/

ArbreQuat* creerArbreQuat(double xc, double yc, double coteX, double coteY); /*создаем ячейку для четверичного дерева*/

void insererNoeudArbre(Noeud* n, ArbreQuat** a, ArbreQuat* parent); /*вставляем в дерево на нужное место эти данные*/

Noeud* rechercheCreeNoeudArbre(Reseau* R, ArbreQuat** a, ArbreQuat* parent, double x, double y); /*возвращает узел сети R, соответствующий точке координат (x, y) в четвертичном дереве*/

Reseau* reconstitueReseauArbre(Chaines* C); /*восстанавливаем сеть по методу квадратичного дерева*/

#endif
