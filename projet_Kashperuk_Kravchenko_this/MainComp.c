#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Hachage.h"
#include "ArbreQuat.h"


int main(){
    
    Chaines *cha;
    Reseau *res;

    for(int i = 6; i<11; i++){

        int nbch = i*500; // Количество цепей
        int nbpts = nbch*100; // Количество точек

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
        
        double t_h_quart = 0; //temps pour les tables d'hachage de taille nbpts/4 

        debut = clock();

        res = reconstitueReseauHachage(cha, nbpts/4);
        
        fin = clock();

        t_h_quart = ((double) (fin - debut)) / CLOCKS_PER_SEC;

        libererReseau(res);
        

        double t_h_moitie = 0; //taille nbpts/2

        debut = clock();

        res = reconstitueReseauHachage(cha, nbpts/2);
        
        fin = clock();

        t_h_moitie = ((double) (fin - debut)) / CLOCKS_PER_SEC;

        libererReseau(res);


        double t_h = 0; //taille nbpts

        debut = clock();

        res = reconstitueReseauHachage(cha, nbpts);
        
        fin = clock();

        t_h = ((double) (fin - debut)) / CLOCKS_PER_SEC;

        libererReseau(res);


        double t_h_double = 0; //taille nbpts*2

        debut = clock();

        res = reconstitueReseauHachage(cha, nbpts*2);
        
        fin = clock();

        t_h_double = ((double) (fin - debut)) / CLOCKS_PER_SEC;

        libererReseau(res);


        double t_h_4 = 0; //taille nbpts*4

        debut = clock();

        res = reconstitueReseauHachage(cha, nbpts*4);
        
        fin = clock();

        t_h_4 = ((double) (fin - debut)) / CLOCKS_PER_SEC;

        libererReseau(res);
        
        
        libererChaine(cha);

        FILE *res = fopen("comp.txt","a"); //stocker les resultats dans comp.txt

        fprintf(res, "%d %f %f %f %f %f %f %f\n", nbch, t_ch, t_ar, t_h_quart, t_h_moitie, t_h, t_h_double, t_h_4);

        fclose(res);

    }

    return 0;
}