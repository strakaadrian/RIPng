/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "routeExpiration.h"
#include "structures.h"


void * checkExpiration(void * par) {
    // dostaneme nasu strukturu s parametrami, ktore sme poslali vlaknu
    struct threadParams * paThrParams = (struct threadParams *) par;
    
    if(paThrParams->routes == NULL) {
        printf("ERROR: Tabulka neexistuje");
	return NULL;
    }
    
    char command[200];
    char ip[INET6_ADDRSTRLEN];
    char sourceIp[INET6_ADDRSTRLEN];
    
    // v nekonecnom cykle kontrolujem ci routa neexpirovala
    for(;;) {
        // prirad prvy prvok
        struct Route * route = paThrParams->routes->head;
        struct Route * pomRoute = NULL;
        struct Route * delRoute = NULL;
        // prechadzam celu smerovaciu tabulku
        while(route != NULL) {
            // ak zaznam expiroval, musim ho vymazat z tejto tabulky aj z linuxovej, expiracia je 180+ sekund 
            if( difftime(time(NULL), route->time) > 180 ) {

                // VYMAZANIE ZAZNAMU Z LINUXU
                
                inet_ntop(AF_INET6, &route->prefix, ip, INET6_ADDRSTRLEN);
                inet_ntop(AF_INET6, &route->prefixNextHop, sourceIp, INET6_ADDRSTRLEN);
                
                // vynuluj string
                memset(command, 0, 200);

                //teraz musime zmazat zaznam v linuxe
                sprintf(command, "sudo ip -6 route del %s/%d via %s dev %s", ip, route->prefixLen, sourceIp, route->nextHopInt);
                
                // posli prikaz do linuxu
                system(command);
                
                // VYMAZANIE ZAZNAMU ZO SMEROVACEJ TABULKY
                
                //ak je len jeden zaznam v tabulke a idem ho zmazat
                if(paThrParams->routes->count == 1) {
                    //nastav head na NULL
                    paThrParams->routes->head = NULL;

                    free(route);
                    
                    paThrParams->routes->count--;
                    
                    break;
                    
                } else if(pomRoute == NULL) {
                    // ak je to prvy zaznam v tabulke tak do head musim dat nasledujuci
                    if(route->next != NULL) {
                        // ak je to prvy zaznam a mam dalsi zaznam
                        paThrParams->routes->head = route->next;
                        // odlozim si route na mazanie
                        delRoute = route;
                        //odloz si predoslu route
                        pomRoute = NULL;
                        //do next si dam next routu
                        route = route->next;
                        //uvilnim pamat
                        free(delRoute);
                        // zniz pocet v tabulke
                        paThrParams->routes->count--;
                        
                        continue;   
                    } 
                } else {
                    if(route->next == NULL) {
                        //ak idem mazat posledny zaznam tak staci len predosli nastavit na NULL
                        pomRoute->next = NULL;

                        // odloz si cestu ktoru ides mazat
                        delRoute = route;

                        // pridaj dalsie cestu
                        route = route->next;

                        // uvolni pamat po tej co mazes
                        free(delRoute);

                        // zniz pocet v tabulke
                        paThrParams->routes->count--;

                        continue;
                    } else {
                        // ak to nieje posledny zaznam tak predoslemu nastav nasledujuci
                        pomRoute->next = route->next;

                        // odloz si cestu ktoru ides mazat
                        delRoute = route;

                        // pridaj dalsie cestu
                        route = route->next;

                        // uvolni pamat po tej co mazes
                        free(delRoute);

                        // zniz pocet v tabulke
                        paThrParams->routes->count--;

                        continue;
                    }
                }    
            }
            // do pomocnej si odloz predoslu routu aby si mohol mazat
            pomRoute = route;
            route = route->next;
        }  
    }
}