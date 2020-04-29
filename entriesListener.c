/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "entriesListener.h"

#define INPUT_SIZE 200
#define COMM_SIZE 20

void *entriesListener(void *par) {
    // dostaneme nasu strukturu s parametrami, ktore sme poslali vlaknu
    struct threadParams * paThrParams = (struct threadParams *) par;
    
    char input[INPUT_SIZE]; // vstup od uzivatela
    memset(&input, 0, INPUT_SIZE);
    
    // prikaz pre linux
    char command[COMM_SIZE];
    memset(&command, 0, COMM_SIZE);
    
    // v nekonecnom cykle pocuvame vstupy od uzivatela 
    for(;;) {
        // nacitavaj vstup od usera
        fgets(input, INPUT_SIZE, stdin);
        
        // z fgets vstupu odrezeme NEWLINE charakter
        strtok(input, "\n");
        
        // ak chce uzivatel vypisat smerovaciu tabulku
        if( (strcmp(input, "ip -6 route") == 0) || (strcmp(input, "ip -6 r") == 0) ) {
            strcpy( command, "ip -6 r");
            system(command);
            continue;
        }
        // ak chceme vypisat smerovaciu tabulku
        if(strcmp(input, "print table") == 0) {
            printRouteTable(paThrParams->routes);
            continue;
        }
        
        // ak chce uzivatel vypnut smerovac
        if(strcmp(input, "exit") == 0) {
            break;
        }
        
        // ak sa ziaden prikaz nezhodov, zadali sme zly prikaz
        printf("Nerozpoznany prikaz, skuste pouzit cely tvar prikazu !\n");
    }
}