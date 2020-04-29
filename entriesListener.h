/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   entriesListener.h
 * Author: student
 *
 * Created on April 22, 2020, 8:12 PM
 */

#ifndef ENTRIESLISTENER_H
#define ENTRIESLISTENER_H

#include "structures.h"
#include "routeTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void *entriesListener(void *par);


#ifdef __cplusplus
}
#endif

#endif /* ENTRIESLISTENER_H */

