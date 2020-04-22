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

struct Route * addPomRoute(struct routeTable * paTable, char paOrigin, struct in6_addr paPrefix, uint8_t paPrefixLen, uint8_t paMetric) {
   
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
    
    struct Route * entry = paTable->head;
    
    if(entry == NULL) {
        paTable->head == route;
        route->next = NULL;
    } else {
        route->next = paTable->head;
        paTable->head = route;
    }
    
    paTable->count++;
    
    return route;
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