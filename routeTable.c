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

struct Route * addRoute(struct routeTable * paTable, char paOrigin, struct in6_addr paPrefix, uint8_t paPrefixLen, uint8_t paMetric, struct in6_addr paSourceIp, char *paNextHopInt, pthread_mutex_t paLock) {
   
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
    route->prefixNextHop = paSourceIp;
    strncpy(route->nextHopInt, paNextHopInt, 10);
    route->time = time(NULL);
    
    struct Route * tableRoute = NULL; // smerovaci zaznam z tabulky
    char command[200];
    char ip[INET6_ADDRSTRLEN];
    char sourceIp[INET6_ADDRSTRLEN];
    
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
                inet_ntop(AF_INET6, &paSourceIp, sourceIp, INET6_ADDRSTRLEN);
                
                //teraz musime zmazat zaznam v linuxe
                sprintf(command, "sudo ip -6 route del %s/%d via %s dev %s", ip, tableRoute->prefixLen, sourceIp, tableRoute->nextHopInt);
                // posli prikaz do linuxu
                system(command);
               
                // teraz pridame novy smerovaci zaznam v linuxe
                memset(command, 0, 200);
                sprintf(command, "sudo ip -6 route add %s/%d via %s dev %s metric %d", ip, tableRoute->prefixLen, sourceIp, tableRoute->nextHopInt, tableRoute->metric);
                // posli prikaz do linuxu
                system(command);
                
                return route;
                
            } else if( (tableRoute->metric == route->metric) ) {
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
    inet_ntop(AF_INET6, &paSourceIp, sourceIp, INET6_ADDRSTRLEN);
    
    // vynuluj string
    memset(command, 0, 200);
    
    //teraz musime pridat zaznam v linuxe
    sprintf(command, "sudo ip -6 route add %s/%d via %s dev %s metric %d", ip, route->prefixLen, sourceIp, route->nextHopInt, route->metric);
    // posli prikaz do linuxu
    system(command);
    
    return route;
}

// vypisanie pomocnej smerovacej tabluky | len pre testovacie uceli
void printRouteTable(struct routeTable * paTable) {
    if(paTable == NULL) {
        printf("ERROR: Tabulka neexistuje");
	return;
    }
    
    struct Route * route = NULL;
    char command[200];
    route = paTable->head;

    printf("\n*************** IPv6 ROUTING TABLE ***************\n");

    while(route != NULL) {
            char prefix[INET6_ADDRSTRLEN];
            char nextHop[INET6_ADDRSTRLEN];
            
            memset(command, 0, 200);
            
            // vyrob string z IPv6
            inet_ntop(AF_INET6, &route->prefix, prefix, sizeof(prefix));
            
            // vyrob string z IPv6
            inet_ntop(AF_INET6, &route->prefixNextHop, nextHop, sizeof(prefix));
    
            // poskladaj smerovaci zaznam
            sprintf(command, "%s/%d via %s dev %s metric %d timestamp %ld sec.", prefix, route->prefixLen, nextHop, route->nextHopInt, route->metric, (time(NULL) - route->time));
            printf("%s\n", command);

            route = route->next;
    }
    printf("\n**************************************************\n");
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