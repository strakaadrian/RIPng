/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "sendRoutes.h"

#define PORT 521
#define BUFF_SIZE 2000

void * sendRoutes(void * par) {
    // dostaneme nasu strukturu s parametrami, ktore sme poslali vlaknu
    struct threadParams * paThrParams = (struct threadParams *) par;
    
    // vytvor socket
    int sock;
    // priprav si premennu pre adresu IPV6
    struct sockaddr_in6 addr;
    //pomocna struktura interfacov
    struct Interface * interface;
    //pomocna struktura pre smerovacie zaznamy
    struct Route * route;
    // buffer kam budeme skladat celu spravu
    char buffer[BUFF_SIZE];
    // pomocna, kam budem skladat pomocnu velkost bufera
    int buffSize;
    //hlavicka spravy
    struct ripHdr hdr;
    //smerovacie zaznamy
    struct ripEntry entry;
    
    //naplnime si strukturu ADDR
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(PORT);
    inet_pton(AF_INET6, "ff02::9", &addr.sin6_addr);
   
    // v nekonecnom cykle posielame kazdych 30 sekund celu smerovaciu tabulku
    for(;;) {
        // ak je aspom 1 zaznam v smerovacej tabulke, inak nic neposiela
        if(paThrParams->routes->head != NULL) {
            // vynuluj buffer
            memset(buffer, 0, sizeof(buffer));
            //vynuluj hlavicku spravy
            memset(&hdr, 0, sizeof(struct ripHdr));

            buffSize = 0;

            //vytvorime hlavicky spravy
            hdr.cmd = 2; // response 2
            hdr.ver = 1;
            hdr.empty = 0;
            
            //zvys velkost
            buffSize += sizeof(struct ripHdr) + sizeof(struct ripEntry);
            
            // pridaj hlavicku do buffera
            memcpy(buffer, &hdr, sizeof(struct ripHdr));
            
            // pridaj prvu entry
            route = paThrParams->routes->head;
            
            // pridavaj smerovacie zaznamy
            while(route != NULL) {
                //vynuluj
                memset(&entry, 0, sizeof(struct ripEntry));
                
                entry.metric = route->metric;
                entry.prefixLen = route->prefixLen;
                entry.routeTag = 0; // NETUSIM CO TU MA BYT ASI 0
                entry.prefix = route->prefix;
                
                // pridaj do buffera dany zaznam
                memcpy( (buffer + buffSize), &entry, sizeof(struct ripEntry));
                
                buffSize += sizeof(struct ripEntry);
                
                route = route->next;
            }
            
            //priradime si prvy prvok z tabulky interfacov
            interface = paThrParams->interfaces->head;

            // prechadzame tabulku interfacov a zistujeme ci maju zapnute RIPng
            while(interface != NULL) {
                // skontrolujeme ci ma interface zapnute RIPng
                if(interface->rip == true) {
                    //--------------------------------------------
                    // pridanie prveho divneho zaznamu
                    //vynuluj
                    memset(&entry, 0, sizeof(struct ripEntry));
                    entry.metric = 16;
                    entry.prefixLen = interface->prefixLen;
                    entry.routeTag = 0; // NETUSIM CO TU MA BYT ASI 0
                    inet_pton(AF_INET6, "2001:db8:1::", &entry.prefix);
                    
                    // pridaj do buffera dany zaznam
                    memcpy( (buffer + sizeof(struct ripHdr)), &entry, sizeof(struct ripEntry));
                    

                    // posli socket    
                    sendto(paThrParams->socketParam, buffer, buffSize, 0, (struct sockaddr*)&addr, sizeof(struct sockaddr_in6));    
                }
                interface = interface->next;
            }
            // cakaj 30 sekund 
            sleep(30);
        }
    }
}