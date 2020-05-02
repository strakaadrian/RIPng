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
#include "recvRoutes.h"
#include "routeExpiration.h"
#include "sendRoutes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <net/if.h>
#include <arpa/inet.h>

#define LINE_SIZE 250
#define RECV_NUMB 5
#define SEND_NUMB 5

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
        struct in6_addr prefixLL;
        struct in6_addr prefixNetwork;
        uint8_t prefixLen = 0;
        uint8_t prefixNetworkLen = 0;
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
        inet_pton(AF_INET6, str, &prefixLL);
        
        str = strtok(NULL, " ");
        inet_pton(AF_INET6, str, &prefixNetwork);
        
        str = strtok(NULL, " ");
        prefixNetworkLen = atoi(str);
        
        str = strtok(NULL, " ");
        rip = atoi(str);
        
        str = strtok(NULL, " ");
        passive = atoi(str);
        
        // pridanie noveho zaznamu do tabulky interfacov
        addInterface(interfaces, intIndex, intName, prefix, prefixLen, prefixLL, prefixNetwork, prefixNetworkLen, rip, passive);
        
        memset(line, 0, LINE_SIZE);
    }
    
    //zavri subor, z ktoreho sme citali
    fclose(config);
    
    // vytvorenie struktury smerovacej tabulky
    struct routeTable * routes = createRouteTable();
    
    // VYTVARANIE MUTEXU //
    
    // vytvorenie struktury, ktora bude obsahovat parametre pre vlakna
    struct threadParams thrParams;

    // vytvorenie mutexu
    if (pthread_mutex_init(&thrParams.lock, NULL) != 0) 
    { 
        printf("Nepodarilo sa vytvorit mutex\n");
        return(EXIT_FAILURE);
    }

    // naplnenie premennych
    thrParams.routes = routes;
    thrParams.interfaces = interfaces;
    
    // VYTVARANIE VLAKIEN //
    
    // deklarovanie premennych  //
    pthread_t thrEntries; // vlakno pre pocuvanie vstupov od uzivatela
    pthread_t thrRecv[RECV_NUMB]; // vlakna pre pocuvanie paketov a vkladanie do smerovacej tabulky
    pthread_t thrSend[SEND_NUMB]; // vlakna pre posielanie updatov
    pthread_t thrRouteExp; // vlakno pre kontrolu expiracie zaznamov v smerovacej tabulke
    
    
    // vytvor vlakno pre pocuvanie vstupov od uzivatela
    if(pthread_create(&thrEntries, NULL, entriesListener, &thrParams)) {
        printf("Nepodarilo sa vytvorit vlakno\n");
        return(EXIT_FAILURE);
    }
    
    
    //kontrolu expiracie zaznamu v smerovacej tabulke
    if(pthread_create(&thrRouteExp, NULL, checkExpiration, &thrParams)) {
        printf("Nepodarilo sa vytvorit vlakno\n");
        return(EXIT_FAILURE);
    }
    
    // pocitadlo pre nasledujuci cyklus
    int counter = 0;
    // pole struktur parametrov pre vlakna
    struct threadParams thrParamsArr[10];
    
    // pre kazdy interface, na ktorom sme zapli RIPng budeme spustat nove vlakno, ktore bude pocuvat
    if(interfaces->count == 0) {
        printf("Nemame priradene ziadne interfacy\n");
        return(EXIT_FAILURE);
    } else {
        // vytvotime pomocny interface
        struct Interface * interface; 
        
        // pridame prvy interface
        interface = interfaces->head;
        
        // prebiehami interfaci a zistujeme ci je na nich RIPng ak ano vytvorime vlakno
        while(interface != NULL) {     
            
            if(interface->rip == true) {
                // pripravime si strukturu pre parametre
                struct threadParams thrRecvParams;
                memset(&thrRecvParams, 0, sizeof(thrRecvParams));
                
                strcpy(thrRecvParams.intName, interface->intName);
                thrRecvParams.prefixLL = interface->prefixLL;
                thrRecvParams.prefixNetwork = interface->prefixNetwork;
                thrRecvParams.prefixNetworkLen = interface->prefixNetworkLen;
                thrRecvParams.routes = routes;
                thrRecvParams.interfaces = interfaces;
                thrRecvParams.lock = thrParams.lock;
                
                // pre kazde vlakno si pripravime ja parametre
                thrParamsArr[counter] = thrRecvParams;
                
                // vytvor vlakno pre pocuvanie paketov
                if(pthread_create(&thrRecv[counter], NULL, recvRoutes, &thrParamsArr[counter])) {
                    printf("Nepodarilo sa vytvorit vlakno\n");
                    return(EXIT_FAILURE);
                }
                
                //posielanie smerovacich zaznamov
                if(pthread_create(&thrSend[counter], NULL, sendRoutes, &thrParamsArr[counter])) {
                    printf("Nepodarilo sa vytvorit vlakno\n");
                    return(EXIT_FAILURE);
                }
            }
            counter++;
            interface = interface->next;
        }      
    }
    
    // ak joineme vlakno pre pocuvanie od usera tak znicime vsetky ostatne vlakna
    if(pthread_join(thrEntries, NULL) == 0) {
        // znicime vsetky vlakna pre pocuvanie 
        for (int i = 0; i < counter; i++) {
            pthread_cancel(thrRecv[i]);
            pthread_cancel(thrSend[i]);
        }
        
        pthread_cancel(thrRouteExp);
        //pthread_cancel(thrSend);
    }

    // znic mutex
    pthread_mutex_destroy(&thrParams.lock);
    

    destroyIntTable(interfaces);
    destroyRouteTable(routes);
    

    return (EXIT_SUCCESS);
}

