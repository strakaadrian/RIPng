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