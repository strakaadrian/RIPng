/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   interfaceTable.h
 * Author: student
 *
 * Created on April 13, 2020, 4:59 PM
 */

#ifndef INTERFACETABLE_H
#define INTERFACETABLE_H

#include "structures.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

// funkcia pre vytvorenie smerovacej tabulky
struct intTable * createIntTable();
// funkcia pre pridanie rozhrania
struct Interface * addInterface(struct intTable * paTable, unsigned int paIntIndex, char * paIntName, struct in6_addr paPrefix, uint8_t paPrefixLen, struct in6_addr paPrefixLL, struct in6_addr paPrefixNetwork, uint8_t paPrefixNetworkLen, bool paRip, bool paPassive);
// funkcia pre vypisanie tabulky rozhrani
void printIntTable(struct intTable * paTable);
//funkcia pre uvolnenie pamete a znicenie tabulky rozhrani
void destroyIntTable(struct intTable * paTable);

#ifdef __cplusplus
}
#endif

#endif /* INTERFACETABLE_H */

