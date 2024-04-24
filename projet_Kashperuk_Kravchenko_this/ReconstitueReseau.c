#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Hachage.h"
#include "ArbreQuat.h"
//Мы тут перестраиваем сеть тремя методами
int main(){

    int rep;
    char input[256];
    FILE *fich;

    do{      
        // Запрос имени файла .cha у пользователя
        printf("Veuillez ecrire le nom du fichier .cha (ou tapez 'quit' pour quitter): ");

        fgets(input, sizeof(input), stdin); // Чтение имени файла из командной строки

        input[strcspn(input, "\n")] = '\0';// Удаление символа новой строки из ввода

        if(strcmp(input,"quit") == 0){// Если пользователь ввел 'quit', завершаем программу
            return 0;
        }
        
        fich = fopen(input,"r");// Попытка открыть файл для чтения
        if (fich == NULL) {
            perror("erreur lors de l'ouverture du fichier(nom probablement incorrecte)! Réessayez!\n");
            rep = 1;
        }else{rep = 0;}

    }while(rep);

    int m; //methode

    do{      
        // Запрос выбора метода у пользователя
        printf("Veuillez choisir la méthode souhaitée parmi les suivanetes:\n1.liste\n2.table de hachage\n3.arbre\n(ou tapez 'quit' pour quitter): ");

        fgets(input, sizeof(input), stdin);// Чтение выбора метода

        input[strcspn(input, "\n")] = '\0';

        if(strcmp(input,"quit") == 0){
            return 0;
        }

        m = atoi(input);// Преобразование введенной строки в целое число
        
        if(!m || m > 3){// Проверка на корректность выбора метода
            printf("insertion non valide!\n");
            rep = 1;
        }else{rep = 0;}

    }while(rep);

    switch (m){// В зависимости от выбранного метода выполняем соответствующие действия

        case 1: ; //кейс Метод списка
            Chaines *cha = lectureChaines(fich);// Чтение цепей из файла 
            fclose(fich);

            // Реконструируем сеть на основе цепей
            Reseau *res = reconstitueReseauListe(cha);

            // Записываем сеть в файл
            FILE *fich2 = fopen("Test_ch.res","w");
            ecrireReseau(res, fich2);
            fclose(fich2);

            // Создаем SVG-файл для визуализации сети
            afficheReseauSVG(res, "Test_ch_affich");
            break;

        case 2: ; // кейс Метод таблицы хэширования
            Chaines *cha_h = lectureChaines(fich);// Чтение цепей из файла
            fclose(fich);

            // Реконструируем сеть на основе цепей с использованием таблицы хэширования
            Reseau *res_h = reconstitueReseauHachage(cha_h, 10);

            // Записываем сеть в файл
            FILE *fich2_h = fopen("Test_h.res","w");
            ecrireReseau(res_h, fich2_h);
            fclose(fich2_h);

            // Создаем SVG-файл для визуализации сети
            afficheReseauSVG(res_h, "Test_h_affich");

            break;

        case 3: ;// Метод дерева
            Chaines *cha_ar = lectureChaines(fich); // Чтение цепей из файла
            fclose(fich);
            
            // Реконструируем сеть на основе цепей с использованием дерева
            Reseau *res_ar = reconstitueReseauArbre(cha_ar);

            // Записываем сеть в файл
            FILE *fich2_ar = fopen("Test_ar.res","w");
            ecrireReseau(res_ar, fich2_ar);
            fclose(fich2_ar);

            // Создаем SVG-файл для визуализации сети
            afficheReseauSVG(res_ar, "Test_ar_affich");

            break;

        default:
            break;

    }
    

    return 0;
}
