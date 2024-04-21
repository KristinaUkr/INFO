#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ArbreQuat.h"
/*определяет мин и макс между точками, составляющими разные цепочки сети*/
void chaineCoordMinMax(Chaines* C, double* xmin, double* ymin, double* xmax, double* ymax){
    /*тут создаем временную переменную и 
    даем значение первого элемента */
    CellChaine *tmp_c = C->chaines;

    while(tmp_c){ /*цикл пока не дойдем до конца*/
        CellPoint *tmp_p = tmp_c->points;
        while(tmp_p){ /*проходимся по всем данным одного элемента*/

            if(*xmin > tmp_p->x){ /*сравниваем с входной точкой и если она больше, 
            то меняем ее значение на значение этой точки*/
                *xmin = tmp_p->x;
            }

            if(*xmax < tmp_p->x){/*ну и так далее*/
                *xmax = tmp_p->x;
            }

            if(*ymin > tmp_p->y){
                *ymin = tmp_p->y;
            }

            if(*ymax < tmp_p->y){
                *ymax = tmp_p->y;
            }

            tmp_p = tmp_p->suiv;
        }
        tmp_c = tmp_c->suiv;
    }

}

/*создаем ячейку для четверичного дерева*/

ArbreQuat* creerArbreQuat(double xc, double yc, double coteX, double coteY){

    ArbreQuat *arb = (ArbreQuat*)malloc(sizeof(ArbreQuat));
    arb->xc = xc;
    arb->yc = yc;
    arb->coteX = coteX;
    arb->coteY = coteY;
    arb->noeud = NULL;
    arb->so = NULL; /*sud-ouest юго запад*/
    arb->se = NULL;/*юго-восток*/
    arb->no = NULL; /*северо запад*/
    arb->ne = NULL; /*северо восток*/

    return arb; 
}

/*вставляем в дерево на нужное место эти данные*/
void insererNoeudArbre(Noeud* n, ArbreQuat** a, ArbreQuat* parent){

    // (cas arbre vide) 
    /*мы создаем с помощью функции creerArbreQuat и координаты центра нового дерева,
     а также его длина и высота будут определены благодаря положению вставляемой точки*/
    if(!*a){

        //trouver les coordonnees de centre
        double xc , yc ;
        if(n->x < parent->xc){
            xc = parent->xc - (parent->coteX/4.0); /*Деление на 4 происходит для того, чтобы вычислить новые координаты
             центра ячейки на четверть размера родительской ячейки*/
        }else{
            xc = parent->xc + (parent->coteX/4.0);
        }

        if(n->y < parent->yc){
            yc = parent->yc - (parent->coteY/4.0);
        }else{
            yc = parent->yc + (parent->coteY/4.0);
        }

        *a = creerArbreQuat(xc, yc, parent->coteX/2.0, parent->coteY/2.0); //on cree l'arbre et l'insere dans a
        
        (*a)->noeud = n;

        return;
    }

    //(cas feuille) 
    /*то текущий лист превращается во внутреннюю ячейку, 
    а узлы в текущем и новом узлах рекурсивно вставляются в дерево.*/
    if((*a)->noeud){

        Noeud *tmp = (*a)->noeud;
        
        (*a)->noeud = NULL; //on transforme la feuille en cellule interne
        
        insererNoeudArbre(n, a, parent); //on insere le nouveau n

        insererNoeudArbre(tmp, a, parent); 
        return;
    }

    //else (cas cellule interne)
    /*она является внутренней ячейкой. 
    В этом случае определяется, в какую из четырех подячеек внутренней ячейки 
    нужно продолжить вставку узла, на основе координат узла и центра текущей ячейки. 
    Рекурсивно вызывается insererNoeudArbre*/
    
    if(n->x < (*a)->xc){ //on determine dans quelle sous arbre on doit continuer

        if(n->y < (*a)->yc){
            insererNoeudArbre(n, &(*a)->so, *a);
        }else{
            insererNoeudArbre(n, &(*a)->no, *a);
        }

    }else{

        if(n->y < (*a)->yc){
            insererNoeudArbre(n, &(*a)->se, *a);
        }else{
            insererNoeudArbre(n, &(*a)->ne, *a);
        }

    }

}

/*возвращает узел сети R, соответствующий точке координат (x, y) в четвертичном дереве*/

Noeud* rechercheCreeNoeudArbre(Reseau* R, ArbreQuat** a, ArbreQuat* parent, double x, double y){

    // (cas arbre vide)
    if(!*a){

        Noeud *n = (Noeud*)malloc(sizeof(Noeud)); //создаем узел
        n->x = x;
        n->y = y;
        n->num = R->nbNoeuds+1; 

        CellNoeud *cn = (CellNoeud*)malloc(sizeof(CellNoeud)); // создаем ячейку
        cn->nd = n;

        cn->suiv = R->noeuds; // вставляем в сеть
        R->noeuds = cn;
        R->nbNoeuds++;

        insererNoeudArbre(n, a, parent); //вставляем в список
        return n;
    }

    //(cas feuille)
    if((*a)->noeud){

        if((*a)->noeud->x == x && (*a)->noeud->y == y){ /*проверяется, совпадают ли координаты узла в этом листе с искомыми координатами (x, y).*/
            return (*a)->noeud; //если д, то все прекрасно и мы выдаем этот результат
        }

        Noeud *n = (Noeud*)malloc(sizeof(Noeud)); /*если нет, то создаем новый узел с указанными координатами и добовляем эту сеть в дерево*/
        n->x = x;
        n->y = y;
        n->num = R->nbNoeuds+1;

        CellNoeud *cn = (CellNoeud*)malloc(sizeof(CellNoeud)); // créer la cellule
        cn->nd = n;

        cn->suiv = R->noeuds; // inserer dans le reseau
        R->noeuds = cn;
        R->nbNoeuds++;

        insererNoeudArbre(n, a, parent); //inserer dans le tableau

        return n;
    }

    //else (cas cellule interne)
    /*текущая ячейка является внутренней. рекурсивно вызывается функция 
    rechercheCreeNoeudArbre для одной из четырех подячеек этой ячейки в 
    зависимости от положения указанных координат (x, y).*/
    
    if(x < (*a)->xc){ //on determine dans quelle sous arbre on doit continuer

        if(y < (*a)->yc){
            return rechercheCreeNoeudArbre(R, &(*a)->so, *a, x, y);
        }else{
            return rechercheCreeNoeudArbre(R, &(*a)->no, *a, x, y);
        }

    }else{

        if(y < (*a)->yc){
            return rechercheCreeNoeudArbre(R, &(*a)->se, *a, x, y);
        }else{
            return rechercheCreeNoeudArbre(R, &(*a)->ne, *a, x, y);
        }

    }
}
/*восстанавливаем сеть по методу квадратичного дерева*/
Reseau* reconstitueReseauArbre(Chaines* C){
    //выделяем память и передаем значения
    Reseau *R = (Reseau*)malloc(sizeof(Reseau)); //allocation du reseau et initialisation des valeurs
    R->nbNoeuds = 0;
    R->gamma = C->gamma;
    R->noeuds = NULL;
    R->commodites = NULL;
    /*Вычисляются минимальные и максимальные значения координат xmin и тп 
    с помощью функции chaineCoordMinMax, применяемой к списку цепей C.*/
    double xmin, ymin, xmax, ymax;
    chaineCoordMinMax(C, &xmin, &ymin, &xmax, &ymax);

    //Вычисление центра и размеров дерева
    double coteX = xmax - xmin; //Вычисление размеров дерева
    double coteY = ymax - ymin;
    double xc = xmin + (coteX/2); //Вычисление центра дерева
    double yc = ymin + (coteY/2);
    ArbreQuat *AR = creerArbreQuat(xc, yc, coteX, coteY); //создаем дерево

    CellChaine *tmp_cch = C->chaines;
    while(tmp_cch){ //on parcourt les chaines

        CellPoint *tmp_p = tmp_cch->points;
        Noeud *A = NULL; Noeud *B = NULL; //les extrémités

        Noeud *n_prec = NULL; //le noeud precedent(pour mettre à jour les voisins)


        while(tmp_p){ //on parcourt les points de la chaine
        /*Для каждой точки выполняется поиск или создание соответствующего узла в дереве AR и сети R*/

            Noeud* tmp_n = rechercheCreeNoeudArbre(R, &AR, AR, tmp_p->x, tmp_p->y);

            if(!A){//on stock la premiere extrémité
            //обновляем указатели на первую (А) и последнюю (В) точки цепи 
                A = tmp_n;
            }
            if(!tmp_p->suiv){//ici la deuxieme
                B = tmp_n;
            }


            if(n_prec){
                /*Обновление списка соседей узлов*/
                CellNoeud *voisins_prec = n_prec->voisins;
                while(voisins_prec){ //on verifie si les 2 noeuds sont deja enregistrés comme des voisins
                    if(voisins_prec->nd == tmp_n){
                        break;
                    }
                    voisins_prec = voisins_prec->suiv;
                }
                

                if(!voisins_prec){ //on met à jour les listes des voisins
                    CellNoeud *tmp_v = (CellNoeud*)malloc(sizeof(CellNoeud));
                    tmp_v->nd = tmp_n;
                    tmp_v->suiv = n_prec->voisins;
                    n_prec->voisins = tmp_v;

                    CellNoeud *tmp_v1 = (CellNoeud*)malloc(sizeof(CellNoeud));
                    tmp_v1->nd = n_prec;
                    tmp_v1->suiv = tmp_n->voisins;
                    tmp_n->voisins = tmp_v1;
                }
            }

            n_prec = tmp_n;

            tmp_p = tmp_p->suiv;
        }
        /*После обработки всех точек в цепи создается коммодитет (пара узлов) 
        и добавляется в список *коммодитетов сети R.*/
        CellCommodite *CC = (CellCommodite*)malloc(sizeof(CellCommodite));
        CC->extrA = A;
        CC->extrB = B;
        CC->suiv = R->commodites;

        R->commodites = CC; //*вот туть

        tmp_cch = tmp_cch->suiv;
    }

    return R;
}

