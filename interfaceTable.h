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

struct intTable * createIntTable();
struct Interface * addInterface(struct intTable * paTable, unsigned int paIntIndex, char * paIntName, struct in6_addr paPrefix, uint8_t paPrefixLen, bool paRip, bool paPassive);
void printIntTable(struct intTable * paTable);
void destroyIntTable(struct intTable * paTable);

#ifdef __cplusplus
}
#endif

#endif /* INTERFACETABLE_H */

