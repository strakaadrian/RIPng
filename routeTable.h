/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   routeTable.h
 * Author: student
 *
 * Created on April 22, 2020, 7:38 PM
 */

#ifndef ROUTETABLE_H
#define ROUTETABLE_H

#include "structures.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// funkcia pre vytvorenie smerovacej tabulky
struct routeTable * createRouteTable();
// pridanie do pomocnej smerovacej tabulky
struct Route * addRoute(struct routeTable * paTable, char paOrigin, struct in6_addr paPrefix, uint8_t paPrefixLen, uint8_t paMetric, struct in6_addr paSourceIp, char *paNextHopInt, pthread_mutex_t paLock);
// vypisanie pomocnej smerovacej tabulky
void printRouteTable(struct routeTable * paTable);
// znicenie pomocnej smerovacej tabulky
void destroyRouteTable(struct routeTable * paTable);


#ifdef __cplusplus
}
#endif

#endif /* ROUTETABLE_H */

