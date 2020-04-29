/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   routeExpiration.h
 * Author: root
 *
 * Created on April 29, 2020, 10:14 AM
 */

#ifndef ROUTEEXPIRATION_H
#define ROUTEEXPIRATION_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <net/if.h>

#ifdef __cplusplus
extern "C" {
#endif


void * checkExpiration(void * par);
    

#ifdef __cplusplus
}
#endif

#endif /* ROUTEEXPIRATION_H */

