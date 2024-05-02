#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Hachage.h"

/*функция ключа*/
int fonctionClef(double x, double y){
    return (x+y)*(x+y+1)/2.0+y;
}

/* k - ключ элемента, A - константа, и m - размер хэш-таблицы
Функция принимает ключ k и размер таблицы m в качестве входных данных и возвращает индекс
ячейки хэш-таблицы, в которой должен быть размещен элемент с ключом k*/
int fonctionHachage(int k, int m){
    float a = (sqrt(5)-1)/2 ;
    return (int)(m*(k*a - (int)(k*a)));
}

/*Возвращает узел из сети R, соответствующий точке (x, y) в хэш-таблице H
Если эта точка существует в H, то возвращаем существующую точку в H, а если нет, то создаем узел и 
добавляем его в H и в список узлов в сети R.*/
Noeud* rechercheCreeNoeudHachage(Reseau* R, TableHachage* H, double x, double y){

    int cle = fonctionClef(x, y);// Вычисляем ключ для точки (x, y)
    int i = fonctionHachage(cle,H->tailleMax); // Находим индекс

    CellNoeud *tmp_cn = H->T[i];// Поиск узла в таблице хэш-таблицы

    while(tmp_cn){ 
        
        // Если найден узел с координатами (x, y), возвращаем его
        if(tmp_cn->nd->x==x && tmp_cn->nd->y==y){
            return tmp_cn->nd;
        }
        tmp_cn = tmp_cn->suiv;
    }
    
    // Если узел не найден, создаем новый узел и добавляем его в таблицу и в сеть
    Noeud *nd = (Noeud*)malloc(sizeof(Noeud));
    nd->x = x;
    nd->y = y;
    nd->num = R->nbNoeuds+1;
    nd->voisins = NULL;

    // Добавляем узел в хэш-таблицу
    CellNoeud *cn = (CellNoeud*)malloc(sizeof(CellNoeud));
    cn->nd = nd;
    cn->suiv = H->T[i];
    H->T[i] = cn;

    // Добавляем узел в сеть
    CellNoeud *cn2 = (CellNoeud*)malloc(sizeof(CellNoeud));
    cn2->nd = nd;
    cn2->suiv = R->noeuds;
    R->noeuds = cn2;
    R->nbNoeuds++;

    return nd;// Возвращаем созданный или найденный узел
}


/*Перестраиваем сеть R из списка цепей C и использует хэш-таблицу H размера M*/
Reseau* reconstitueReseauHachage(Chaines *C, int M){

    Reseau *R = (Reseau*)malloc(sizeof(Reseau)); // Выделяем память под структуру Reseau
    R->nbNoeuds = 0;// инициализацируем значений
    R->gamma = C->gamma;
    R->noeuds = NULL;
    R->commodites = NULL;

    // Выделяем память под хэш-таблицу
    TableHachage *H = (TableHachage*)malloc(sizeof(TableHachage));
    H->nbElement = 0;
    H->tailleMax = M;
    H->T = (CellNoeud**)malloc(sizeof(CellNoeud*) * M);// выделение памяти для массива указателей на ячейки
    for(int i = 0; i < M; i++) {
        H->T[i] = NULL;// инициализация каждого элемента массива как NULL
    }

    // Обходим все цепи в списке цепей
    CellChaine *tmp_cch = C->chaines;
    while(tmp_cch){ 

        CellPoint *tmp_p = tmp_cch->points;
        Noeud *A = NULL; Noeud *B = NULL; // определяем концы (экстремумы)

        Noeud *n_prec = NULL; // предыдущий узел (для обновления соседей)

        while(tmp_p){

            Noeud* tmp_n = rechercheCreeNoeudHachage(R, H, tmp_p->x, tmp_p->y);// Поиск или создание узла в хэш-таблице

            if(!A){// сохраняем первый конец цепи (экстремум)
                A = tmp_n;
            }
            if(!tmp_p->suiv){//второй
                B = tmp_n;
            }


            if(n_prec){// Обновление списков соседей

                CellNoeud *voisins_prec = n_prec->voisins;
                while(voisins_prec){ // проверяем, есть ли уже узлы среди соседей
                    if(voisins_prec->nd == tmp_n){
                        break;
                    }
                    voisins_prec = voisins_prec->suiv;
                }
                

                if(!voisins_prec){ // обновляем списки соседей если текущий сосед еще не был добавлен в список соседей
                    CellNoeud *tmp_v = (CellNoeud*)malloc(sizeof(CellNoeud)); // новый узел для предыдущего узла
                    tmp_v->nd = tmp_n;// Указываем на текущий узел
                    tmp_v->suiv = n_prec->voisins;//соседей предыдущего узла присваиваем указателю на след узел
                    n_prec->voisins = tmp_v;// Присваиваем предыдущему узлу указатель на новый узел

                    CellNoeud *tmp_v1 = (CellNoeud*)malloc(sizeof(CellNoeud));// новый узел для текущего узла
                    tmp_v1->nd = n_prec;// Указываем на предыдущий узел
                    tmp_v1->suiv = tmp_n->voisins;//соседей текущего узла присваиваем указателю на след узел
                    tmp_n->voisins = tmp_v1;// Присваиваем текущему узлу указатель на новый узел
                }
            }

            n_prec = tmp_n;

            tmp_p = tmp_p->suiv;
        }

        // Добавление удобного узла в сеть
        CellCommodite *CC = (CellCommodite*)malloc(sizeof(CellCommodite));
        CC->extrA = A;
        CC->extrB = B;
        CC->suiv = R->commodites;

        R->commodites = CC;

        tmp_cch = tmp_cch->suiv;
    }
    
    libererTable(H);
    return R;
}

/* Освободите память, выделенную для хэш-таблицы и ее элементов */
void libererTable(TableHachage *tab) {

    if (tab == NULL) return;// Проверьте, не пуста ли таблица.

    // Освободите цепной список в таблице
    for (int i = 0; i < tab->tailleMax; i++) {
        CellNoeud *current_noeud = tab->T[i];
        while (current_noeud != NULL) {
            CellNoeud *temp_noeud = current_noeud; // Сохраните временную ссылку 
            current_noeud = current_noeud->suiv; 
            free(temp_noeud);// Освободите таблицу
        }
    }