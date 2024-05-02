#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Hachage.h"
#include "ArbreQuat.h"


int main(){
    
    Chaines *cha;
    Reseau *res;

    for(int i = 1; i<11; i++){

        int nbch = i*5;

        cha = generationAleatoire(nbch, 100, 5000, 5000); // Генерация случайных цепей

        clock_t debut, fin;

        /*     LISTE-CHAINE     */

        double t_ch = 0; // Время для связанного списка

        debut = clock();
        
        res = reconstitueReseauListe(cha); // Восстановление сети из списка

        fin = clock();

        t_ch = ((double) (fin - debut)) / CLOCKS_PER_SEC;

        libererReseau(res);// Освобождение памяти

        /*     ARBRE     */
        
        double t_ar = 0; // Время для дерева

        debut = clock();

        res = reconstitueReseauArbre(cha);// Восстановление сети из дерева
        
        fin = clock();

        t_ar = ((double) (fin - debut)) / CLOCKS_PER_SEC;

        libererReseau(res);// Освобождение памяти


        /*     HACHAGE     */
        
        double t_h = 0; // Время для хэш-таблиц

        for(int n = 6; n<21; n++){ // Изменение размера таблицы

            debut = clock();

            res = reconstitueReseauHachage(cha, n);// Восстановление сети из хэш-таблицы
            
            fin = clock();

            t_h += ((double) (fin - debut)) / CLOCKS_PER_SEC;

            libererReseau(res);// Освобождение памяти

        }

        t_h /= 15; // Расчет среднего

        libererChaine(cha);// Освобождение памяти цепей

        FILE *res = fopen("comp.txt","a"); // Запись результатов в файл comp.txt

        fprintf(res, "%d %f %f %f\n", nbch,t_ch, t_h, t_ar);// Запись результатов

        fclose(res);

    }

    return 0;
}