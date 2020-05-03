/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   sendRoutes.h
 * Author: root
 *
 * Created on April 29, 2020, 9:47 PM
 */

#ifndef SENDROUTES_H
#define SENDROUTES_H

#include "structures.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#ifdef __cplusplus
extern "C" {
#endif

// funkcia pre posielanie zaznamov zo smerovacje tabulky
void * sendRoutes(void *par); 

#ifdef __cplusplus
}
#endif

#endif /* SENDROUTES_H */

