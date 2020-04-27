/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "routeTable.h"

struct routeTable * createRouteTable() {
    struct routeTable * table;
    table = (struct routeTable *)malloc(sizeof(struct routeTable));
    
    if(table == NULL) {
        printf("ERROR: Nepodarilo sa vytvorit tabulku");
	return NULL;
    }
    
    memset(table, 0, sizeof(struct routeTable));

    return table;
}



struct Route * addRoute(struct routeTable * paTable, char paOrigin, struct in6_addr paPrefix, uint8_t paPrefixLen, uint8_t paMetric, char *paNextHopInt, pthread_mutex_t paLock) {
   
    if(paTable == NULL) {
        printf("ERROR: Tabulka neexistuje");
	return NULL;
    }
    
    struct Route * route;
    
    route = (struct Route *)malloc(sizeof(struct Route));
    
    if (route == NULL) {
	printf("ERROR: Nepodarilo sa vytvorit routu");
	return NULL;
    }
    
    route->origin = paOrigin;
    route->prefix = paPrefix;
    route->prefixLen = paPrefixLen;
    route->metric = paMetric;
    strncpy(route->nextHopInt, paNextHopInt, 10);
    route->time = time(NULL);
    
    struct Route * tableRoute = NULL; // smerovaci zaznam z tabulky
    char command[200];
    char * ip;
    
    tableRoute = paTable->head;

    // prechadzame smerovaciu tbaulku a hladame rovnaky zaznam
    while(tableRoute != NULL) {
          
        //overenie ci je tam taka siet s rovnakou IP a MASKOU
        if( (memcmp(&tableRoute->prefix, &route->prefix, sizeof(struct in6_addr)) == 0) && (tableRoute->prefixLen == route->prefixLen) && ( strcmp(tableRoute->nextHopInt, route->nextHopInt) == 0) ) {
            // ak je tam taka siet, ale ma mensiu metriku tak zaznam pridaj za ten zaznam, ktory tam je
            if(route->metric < tableRoute->metric) {
                
                //zamnkni mutex
                pthread_mutex_lock(&paLock);
                
                // pridaj zaznam za uz existujuci zaznam
                tableRoute->metric = route->metric;
                tableRoute->time = time(NULL);
                
                // odomnkni mutex
                pthread_mutex_unlock(&paLock);
                
                //pomocny string pe pridanie a odstranenie smerovacieho zaznamu
                memset(command, 0, 200);
                
                inet_ntop(AF_INET6, &tableRoute->prefix, ip, INET6_ADDRSTRLEN);
                
                //teraz musime zmazat zaznam v linuxe
                sprintf(command, "sudo ip -6 route del %s dev %s", ip, tableRoute->nextHopInt);
                // posli prikaz do linuxu
                system(command);
               
                // teraz pridame novy smerovaci zaznam v linuxe
                memset(command, 0, 200);
                sprintf(command, "sudo ip -6 route add %s dev %s metric", ip, tableRoute->nextHopInt, tableRoute->metric);
                // posli prikaz do linuxu
                system(command);
                
                return route;
            } else if( (memcmp(&tableRoute->prefix, &route->prefix, sizeof(struct in6_addr)) == 0) && (tableRoute->prefixLen == route->prefixLen) && ( strcmp(tableRoute->nextHopInt, route->nextHopInt) == 0) && (tableRoute->metric == route->metric) ) {
                // ak pride identicky zaznam len mu daj cas na aktualny nech sa mu nezmaze zaznam
                
                //zamnkni mutex
                pthread_mutex_lock(&paLock);
                
                tableRoute->time = time(NULL);
                
                // odomnkni mutex
                pthread_mutex_unlock(&paLock);
                
                return route;
            }
        }   
        tableRoute = tableRoute->next;
    }
    
    //zamnkni mutex
    pthread_mutex_lock(&paLock);
    
    struct Route * entry = paTable->head;
    
    if(entry == NULL) {
        paTable->head = route;
        route->next = NULL;
    } else {
        route->next = paTable->head;
        paTable->head = route;
    }
    
    paTable->count++;
    
    // odomnkni mutex
    pthread_mutex_unlock(&paLock);
    
    // pridaj zaznam aj do linuxovej smerovacej tabulky
    inet_ntop(AF_INET6, &route->prefix, ip, INET6_ADDRSTRLEN);
    
    // vynuluj string
    memset(command, 0, 200);
    
    //teraz musime pridat zaznam v linuxe
    sprintf(command, "sudo ip -6 route add %s dev %s", ip, route->nextHopInt);
    // posli prikaz do linuxu
    system(command);
    
    return route;
}

// vypisanie pomocnej smerovacej tabluky | len pre testovacie uceli
void printPomRouteTable(struct routeTable * paTable) {
    if(paTable == NULL) {
        printf("ERROR: Tabulka neexistuje");
	return;
    }
    
    struct Route * route = NULL;
    route = paTable->head;

    printf("| Origin |         Address        | Length | Metric\n");

    while(route != NULL) {
            char prefix[INET6_ADDRSTRLEN];

            printf("| %c |", route->origin);

            inet_ntop(AF_INET6, &route->prefix, prefix, sizeof(prefix));
            printf(" %s |", prefix);

            printf("   %hhu   |", route->prefixLen);

            printf("  %hhu \n", route->metric);

            route = route->next;
    }
    
}



void destroyRouteTable(struct routeTable * paTable) {
    if(paTable == NULL) {
        printf("ERROR: Tabulka neexistuje");
	return;
    }
    struct Route * route;
    struct Route * pom;
    
    route = paTable->head;
    
    while(route != NULL) {
	pom = route;
	route = route->next;
	free(pom);
    }

    free(paTable);
}