#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Chaine.h"
#include "SVGwriter.h"

/*позволяет выделять, заполнять и возвращать экземпляр нашей структуры из файла.*/
Chaines* lectureChaines(FILE *f){
    // Выделяем память для новой ячейки макс кол волокон, кол цепей, список цепей
    Chaines *ch = (Chaines*)malloc(sizeof(Chaines));
    ch->chaines = NULL;

    /*Считываем количество цепочек и значение gamma 
    (количество оптических волокон) из файла*/
    char input[256];
    fgets(input, sizeof(input), f);
    sscanf(input,"NbChain: %d",&(ch->nbChaines));
    fgets(input, sizeof(input), f);
    sscanf(input,"Gamma: %d",&(ch->gamma));

    // Считываем данные о каждой цепочке
    while(!feof(f)){
        fgets(input, sizeof(input), f);

        // Пропускаем пустые строки
        if(strcmp(input,"\n")==0){
            continue;
        }

        // Выделяем память для новой ячейки номера строки и списка точек
        CellChaine *cch = (CellChaine*)malloc(sizeof(CellChaine));
        cch->points = NULL;
        cch->suiv = NULL;

        int n;
        sscanf(input,"%d %d",&(cch->numero), &n);

        int pos = 0;
        for (int i = 0; i < 2; i++) {
            pos += strcspn(input + pos, " ") + 1;
        }

        // Считываем данные о точках внутри цепочки
        for (int i = 0; i < n; i++){
            // Выделяем память для новой ячейки координат точки
            CellPoint *cp = (CellPoint*)malloc(sizeof(CellPoint));
            float x, y;
            sscanf(input+pos, "%f %f", &x, &y);
            for (int i = 0; i < 2; i++) {
                pos += strcspn(input + pos, " ") + 1;
            }
            cp->x = x;
            cp->y = y;
            cp->suiv = cch->points;
            cch->points = cp;
        }

        // Добавляем новую цепочку в начало списка цепочек
        cch->suiv = ch->chaines;
        ch->chaines = cch;
    }

    return ch;
}

/*записывает содержимое Chaines в файл в том же формате, что и исходный файл*/
void ecrireChaines(Chaines *C, FILE *f){
    // Записываем количество цепочек и количество оптических волокон в файл
    fprintf(f, "NbChain: %d\nGamma: %d\n", C->nbChaines, C->gamma);

    // Обходим список цепочек
    CellChaine *tmp_c = C->chaines;
    while(tmp_c){

        // Записываем номер текущей цепочки в файл
        fprintf(f, "%d", tmp_c->numero);
        // Обходим список точек текущей цепочки
        CellPoint *tmp_p = tmp_c->points;

        int n_p = 0;
        char pts[256] = ""; //В цикле обхода точек каждая пара координат x и y добавляется в эту строку

        while(tmp_p){
            // Форматируем координаты точек и сохраняем их в буфер
            sprintf(pts + strlen(pts), " %.2f %.2f", tmp_p->x, tmp_p->y);

            n_p++;
            tmp_p = tmp_p->suiv;
        }
        // Записываем количество точек и их координаты в файл
        fprintf(f, " %d%s\n", n_p, pts);

        tmp_c = tmp_c->suiv;
    }
}

/*Мы хотим получить графическое представление экземпляров
(Векторная графика) создаем SVG-файл в формате html из struct Chaines*/
void afficheChainesSVG(Chaines *C, char* nomInstance){

    //Инициализация переменных для определения границ для корректного масштабирования координат
    double maxx=0,maxy=0,minx=1e6,miny=1e6;
    CellChaine *ccour;
    CellPoint *pcour;
    double precx,precy;
    SVGwriter svg;
    /*Первый проход для определения границ, т е поиск
    мин и макс знач координат x и y среди всех точек цепочек*/
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        while (pcour!=NULL){
            if (maxx<pcour->x) maxx=pcour->x;
            if (maxy<pcour->y) maxy=pcour->y;
            if (minx>pcour->x) minx=pcour->x;
            if (miny>pcour->y) miny=pcour->y;  
            pcour=pcour->suiv;
        }
    ccour=ccour->suiv;
    }
    //Инициализация SVG-файла
    SVGinit(&svg,nomInstance,500,500);
    //Второй проход для отображения цепочек для отображения их на SVG-изображении
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        SVGlineRandColor(&svg);// Установка случайного цвета для линии
        /*SVGpoint(&svg, x, y) для отображения точки на SVG-изображении в указанных координатах
        500 бла-бла нормализует коорд x точ относительно мин и макс знач коорд x 
        и еще потому что 500x500 единиц мы хотим разместить*/
        SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny)); 
        precx=pcour->x;
        precy=pcour->y;  
        pcour=pcour->suiv;
        while (pcour!=NULL){
            //SVGline(&svg, x1, y1, x2, y2)тут мы добавляем линию между двумя точками
            SVGline(&svg,500*(precx-minx)/(maxx-minx),500*(precy-miny)/(maxy-miny),500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            //тут появляеться новая точка
            SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            precx=pcour->x;
            precy=pcour->y;    
            pcour=pcour->suiv;
        }
        ccour=ccour->suiv;
    }
    SVGfinalize(&svg);
}

//вычисляет длину конкретной цепи, переданной в качестве аргумента.
double longueurChaine(CellChaine *c){

    double res = 0;
    if(!c->points){
        return res;
    }

    CellPoint *tmp = c->points; // Указатель для перебора точек по цепи
    while(tmp->suiv){
        // Добавляем расстояние между текущей точкой и следующей к общей сумме длин
        res+= sqrt(pow(tmp->suiv->x - tmp->x, 2) + pow(tmp->suiv->y - tmp->y, 2));

        tmp = tmp->suiv;
    }

    return res;
}


//вычисляет суммарную длину всех цепей в структуре Chaines
double longueurTotale(Chaines *C){

    double res = 0;
    CellChaine *tmp = C->chaines;
    // Пока есть цепи в структуре Chaines добавляем длину к результату
    while(tmp){
        res+= longueurChaine(tmp);

        tmp = tmp->suiv;
    }

    return res;
}

//общее количество вхождений точек (точки, появляющиеся > 1 р учитываются > 1 р)
int comptePointsTotal(Chaines *C){
    
    int res = 0;
   
    CellChaine *tmp_c = C->chaines; // Получаем указатель на первую цепочку в структуре Chaines
    while(tmp_c){
        CellPoint *tmp_p = tmp_c->points;// Получаем указатель на первую точку в текущей цепочке
        while(tmp_p){
            res++; //считаем
        }
    }
    
    return res;
}

//we create random chaine between (0,0) et (xmax,ymax)
Chaines* generationAleatoire(int nbChaines, int nbPointsChaine, int xmax, int ymax){

    Chaines *ch = (Chaines*)malloc(sizeof(Chaines)); //allocation
    ch->nbChaines = nbChaines;
    ch->gamma = 3;
    ch->chaines = NULL; 

    for(int j=0; j<nbChaines; j++){ //

        CellChaine *cch = (CellChaine*)malloc(sizeof(CellChaine)); //allocation
        cch->suiv = NULL;
        cch->points = NULL;
        
        for (int i = 0; i < nbPointsChaine; i++){ //lire n points

            CellPoint *cp = (CellPoint*)malloc(sizeof(CellPoint)); //allocation
            float x = ((float)rand() / RAND_MAX) * xmax;
            float y = ((float)rand() / RAND_MAX) * ymax;

            //on insere le point en tete
            cp->x = x;
            cp->y = y;
            cp->suiv = cch->points;
            cch->points = cp;
        }

        //on insere la chaine en tete
        cch->numero = j;
        cch->suiv = ch->chaines;
        ch->chaines = cch;
    }

    return ch;
}

/*Освобождаем цепь*/
void libererChaine(Chaines *ch) {
    
    if (ch == NULL) return;// Проверьте, не является ли структура уже пустой

    CellChaine *cch = ch->chaines;

    while (cch){
        CellChaine *tmp_ch = cch; 
        cch = cch->suiv;

        CellPoint *cp = tmp_ch->points;
        while (cp != NULL){
            CellPoint *tmp_cp = cp;
            cp = cp->suiv;
            free(tmp_cp); // Освободите точку
        }

        free(tmp_ch); // Освободите цепь
    }

    free(ch); // Освободите саму структуру Chaines
}
