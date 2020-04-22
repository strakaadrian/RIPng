/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: student
 *
 * Created on April 13, 2020, 2:09 PM
 */
#include "structures.h"
#include "interfaceTable.h"
#include "routeTable.h"
#include "entriesListener.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <net/if.h>
#include <arpa/inet.h>

#define LINE_SIZE 250

/*
 * 
 */
int main(int argc, char** argv) {
    // vytvor tabulku rozhrani
    struct intTable * interfaces = createIntTable();
    
    // nacitavame z konfiguracneho suboru
    FILE *config;
    char line[LINE_SIZE];
    
    memset(line, 0, LINE_SIZE);
    
    //otvor subor na citanie
    config = fopen("vstup.txt", "r");
    
    // over ci sa spravne subor otvoril
    if(config == NULL) {
        perror("ERROR LOADING FILE:");
        return (EXIT_FAILURE);
    }
    
    // nacitavaj zo suboru po riadkoch, potom tieto riadky parsuj
    while (fgets(line, LINE_SIZE, config)) {
        // pomocne parametre kam si dam hodnoty zo suboru
        unsigned int intIndex = 0;
        char intName[10];
        struct in6_addr prefix;
        uint8_t prefixLen = 0;
        bool rip = 0;
        bool passive = 0;
        
        memset(intName, 0, 10);
        //////////////////////////////////
        
        //parsovanie jedneho riadku
        char *str = strtok(line, " "); 
        intIndex = if_nametoindex(str);
        strncpy(intName, str, strlen(str));
        
        str = strtok(NULL, " ");
        inet_pton(AF_INET6, str, &prefix);
        
        str = strtok(NULL, " ");
        prefixLen = atoi(str);
        
        str = strtok(NULL, " ");
        rip = atoi(str);
        
        str = strtok(NULL, " ");
        passive = atoi(str);
        
        // pridanie noveho zaznamu do tabulky interfacov
        addInterface(interfaces, intIndex, intName, prefix, prefixLen, rip, passive);
        
        memset(line, 0, LINE_SIZE);
    }
    
    // vytvorenie struktury smerovacej tabulky
    struct routeTable * routes = createRouteTable();
    
    // VYTVARANIE MUTEXU //
    
    // vytvorenie struktury, ktora bude obsahovat parametre pre vlakna
    struct threadParams thrParams;
    
    thrParams.exitStatus = false;
    
    // vytvorenie mutexu
    if (pthread_mutex_init(&thrParams.lock, NULL) != 0) 
    { 
        printf("Nepodarilo sa vytvorit mutex\n");
        return(EXIT_FAILURE);
    }
    
    
    // VYTVARANIE VLAKIEN //
    
    // deklarovanie premennych  //
    pthread_t thrEntries; // vlakno pre pocuvanie vstupov od uzivatela
    //pthread_t thrRecv; // vlakno pre pocuvanie paketov a vkladanie do smerovacej tabulky
    //pthread_t thrSend; // vlakno pre posielanie updatov
    //pthread_t thrRouteExp; // vlakno pre kontrolu expiracie zaznamov v smerovacej tabulke
    
    
    // vytvor vlakno pre pocuvanie vstupov od uzivatela
    if(pthread_create(&thrEntries, NULL, entriesListener, &thrParams)) {
        printf("Nepodarilo sa vytvorit vlakno\n");
        return(EXIT_FAILURE);
    }
    
    
    
    // znic mutex
    pthread_mutex_destroy(&thrParams.lock);
    
    // joiny vlakna, vlakna na seba pockaju pri ukoncovani
    pthread_join(thrEntries, NULL);
    //pthread_join(thrRecv, NULL);
    //pthread_join(thrSend, NULL);
    //pthread_join(thrRouteExp, NULL);
    
    
    
    
    printIntTable(interfaces);
    
    
    destroyIntTable(interfaces);
    destroyRouteTable(routes);
    
    fclose(config);
    
    return (EXIT_SUCCESS);
}

