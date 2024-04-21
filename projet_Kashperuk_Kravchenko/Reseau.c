#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Reseau.h"
#include "SVGwriter.h"

/*Возвращаем узел сети R, соответствующий точке (x, y) в списке chaınee noeuds сети R
Если эта точка существует в узлах, то функция возвращает существующий узел в узлах, 
а если нет, то функция создает узел и добавляет его в список узлов*/
Noeud* rechercheCreeNoeudListe(Reseau *R, double x, double y){    
    // Получаем указатель на первую ячейку списка узлов сети R
    CellNoeud *tmp_cn = R->noeuds;

    while(tmp_cn){
        /* Если координаты текущего узла совпадают с заданными координатами (x, y)
        Возвращаем указатель на текущий узел*/
        if(tmp_cn->nd->x == x && tmp_cn->nd->y == y){
            return tmp_cn->nd;
        }
        tmp_cn = tmp_cn->suiv;
    }
    // Если узел с заданными координатами не найден, создаем новый узел
    Noeud *res = (Noeud*)malloc(sizeof(Noeud));
    res->x = x;
    res->y = y;
    res->num = R->nbNoeuds+1;  // Назначаем номер новому узлу (на единицу больше текущего количества узлов)

    CellNoeud *CN = (CellNoeud*)malloc(sizeof(CellNoeud));// Создаем новую ячейку списка узлов
    CN->nd = res;
    CN->suiv = R->noeuds;// Устанавливаем указатель на следующую ячейку в новой ячейке

    R->noeuds = CN;// Обновляем указатель на первую ячейку списка узлов сети R
    R->nbNoeuds++;

    return res;
}

/*Реконструируем сеть R на основе списка цепей C*/
Reseau* reconstitueReseauListe(Chaines *C){
    // Выделяем память под новую сеть R
    Reseau *R = (Reseau*)malloc(sizeof(Reseau));
    // Инициализируем поля сети R
    R->nbNoeuds = 0;
    R->gamma = C->gamma;
    R->noeuds = NULL;
    R->commodites = NULL;

    CellChaine *tmp_cch = C->chaines;//указатель на первую цепь списка цепей
    while(tmp_cch){

        CellPoint *tmp_p = tmp_cch->points;//на первую точку текущей цепи
        Noeud *A = NULL; Noeud *B = NULL; //узлы A и B (концы цепи)

        Noeud *n_prec = NULL; // указатель на предыдущий узел (для обновления соседей)


        while(tmp_p){
            // Ищем или создаем узел для текущей точки
            Noeud* tmp_n = rechercheCreeNoeudListe(R, tmp_p->x, tmp_p->y);

            if(!A){// Если это первая точка цепи, сохраняем ее как узел A
                A = tmp_n;
            }
            if(!tmp_p->suiv){// Если это последняя точка цепи, сохраняем ее как узел B
                B = tmp_n;
            }


            if(n_prec){// Если есть предыдущий узел, проверяем, не являются ли текущий и предыдущий узлы соседями

                CellNoeud *voisins_prec = n_prec->voisins;// Проверяем, есть ли текущий узел в списке соседей предыдущего узла
                while(voisins_prec){ 
                    if(voisins_prec->nd == tmp_n){
                        break;
                    }
                    voisins_prec = voisins_prec->suiv;
                }
                

                if(!voisins_prec){  // Если текущий узел еще не является соседом предыдущего узла, добавляем его в список соседей обоих узлов
                    CellNoeud *tmp_v = (CellNoeud*)malloc(sizeof(CellNoeud));// Добавляем текущий узел в список соседей предыдущего узла
                    tmp_v->nd = tmp_n;
                    tmp_v->suiv = n_prec->voisins;
                    n_prec->voisins = tmp_v;

                    CellNoeud *tmp_v1 = (CellNoeud*)malloc(sizeof(CellNoeud));// Добавляем предыдущий узел в список соседей текущего узла
                    tmp_v1->nd = n_prec;
                    tmp_v1->suiv = tmp_n->voisins;
                    tmp_n->voisins = tmp_v1;
                }
            }

            n_prec = tmp_n;

            tmp_p = tmp_p->suiv;
        }
        // Создаем новую ячейку списка коммодитетов и добавляем ее в список коммодитетов сети R
        CellCommodite *CC = (CellCommodite*)malloc(sizeof(CellCommodite));
        CC->extrA = A;
        CC->extrB = B;
        CC->suiv = R->commodites;

        R->commodites = CC;

        tmp_cch = tmp_cch->suiv;// Переходим к следующей цепи
    }

    return R;

}

/*Подсчитывает кол-во связей*/
int nbLiaisons(Reseau *R){
    int res = 0;
    CellNoeud *tmp_n = R->noeuds;

    while(tmp_n){

        // Перебираем всех соседей текущего узла и увеличиваем счетчик связей
        CellNoeud *tmp_v = tmp_n->nd->voisins;
        while(tmp_v){
            res++;
            tmp_v = tmp_v->suiv;
        }

        tmp_n = tmp_n->suiv;
    }
    // Каждая связь учитывается дважды (для каждого узла), поэтому делим результат на 2
    return res/2;
}

/*Подсчитывает кол-во удобных узлов или комодите*/
int nbCommodites(Reseau *R){
    int res = 0;

    // Перебираем все комодитеты и увеличиваем счетчик
    CellCommodite *tmp_c = R->commodites;

    while(tmp_c){
        res++;
        tmp_c = tmp_c->suiv;
    }

    return res;
}

/* записываем содержимое Reseau в файл, используя тот же формат, что и файл 00014 burma.res*/
void ecrireReseau(Reseau *R, FILE *f){

    // Записываем общую информацию о сети: количество узлов, связей, комодитетов и значение параметра gamma
    fprintf(f, "NbNoeuds: %d\nNbLiaisons: %d\nNbCommodites: %d\nGamma: %d\n\n", R->nbNoeuds, nbLiaisons(R), nbCommodites(R), R->gamma);

    CellNoeud *tmp_n = R->noeuds;// Перебираем все узлы сети и записываем их координаты в файл

    char lia[256] = "";// Строка для хранения информации о связях между узлами

    while(tmp_n){

        // Записываем координаты текущего узла в файл
        fprintf(f, "v %d %.2f %.2f\n", tmp_n->nd->num, tmp_n->nd->x, tmp_n->nd->y);

        // Перебираем всех соседей текущего узла и записываем информацию о связях
        CellNoeud *tmp_v = tmp_n->nd->voisins;
        while(tmp_v){
            
            // Добавляем информацию о связи в строку lia
            sprintf(lia + strlen(lia), "l %d %d\n", tmp_v->nd->num, tmp_n->nd->num);
            tmp_v = tmp_v->suiv;
        }

        tmp_n = tmp_n->suiv;

    }

    fprintf(f, "\n%s\n", lia);// Записываем строку с информацией о связях в файл

    CellCommodite *tmp_c = R->commodites;// Перебираем все комодитеты и записываем их в файл
    while(tmp_c){

        fprintf(f, "k %d %d\n", tmp_c->extrA->num, tmp_c->extrB->num);// Записываем информацию о комодитете в файл
        tmp_c = tmp_c->suiv;
    }

}


/*готовый код который нам дан, что бы сверить результаты
Нам нужно подтвердить свой код на нескольких экземплярах, чтобы подтвердить (частично) свои функции.*/
void afficheReseauSVG(Reseau *R, char* nomInstance){
    CellNoeud *courN,*courv;
    SVGwriter svg;
    double maxx=0,maxy=0,minx=1e6,miny=1e6;

    courN=R->noeuds;
    while (courN!=NULL){
        if (maxx<courN->nd->x) maxx=courN->nd->x;
        if (maxy<courN->nd->y) maxy=courN->nd->y;
        if (minx>courN->nd->x) minx=courN->nd->x;
        if (miny>courN->nd->y) miny=courN->nd->y;
        courN=courN->suiv;
    }
    SVGinit(&svg,nomInstance,500,500);
    courN=R->noeuds;
    while (courN!=NULL){
        SVGpoint(&svg,500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
        courv=courN->nd->voisins;
        while (courv!=NULL){
            if (courv->nd->num<courN->nd->num)
                SVGline(&svg,500*(courv->nd->x-minx)/(maxx-minx),500*(courv->nd->y-miny)/(maxy-miny),500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
            courv=courv->suiv;
        }
        courN=courN->suiv;
    }
    SVGfinalize(&svg);
}

