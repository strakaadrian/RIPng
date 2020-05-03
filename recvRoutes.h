/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   recvRoutes.h
 * Author: student
 *
 * Created on April 22, 2020, 9:29 PM
 */

#ifndef RECVROUTES_H
#define RECVROUTES_H

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

// funkcia pre primanie smerovacich zaznamov
void * recvRoutes(void *par); 


#ifdef __cplusplus
}
#endif

#endif /* RECVROUTES_H */

