/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   structures.h
 * Author: student
 *
 * Created on April 13, 2020, 2:22 PM
 */

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <netinet/in.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    

// ROZHRANIE
struct Interface {
    struct Interface * next;
    unsigned int intId; //ID INTERFACU
    char intName[10]; // MENU INTRFACU
    struct in6_addr prefix; // IPv6 ADRESA
    uint8_t prefixLen; // ak budeme pouzivat len 128 dlzky, tento atribut nam zrejme netreba
    struct in6_addr prefixLL; // IPv6 ADRESA LINK LOCAL
    struct in6_addr prefixNetwork; // IPv6 ADRESA LINK LOCAL
    uint8_t prefixNetworkLen;
    bool rip; // JE, ALEBO NIEJE ZAPNUTE RIP NA DANOM INTERFACE
    bool passiveInt; // JE ROZHRANIE PASIVNE ?
};

// TABULKA ROZHRANI
struct intTable {
    struct Interface * head;
    unsigned int count;
};

// SMEROVACI ZAZNAM
struct Route {
    struct Route * next;
    char origin; // ODKADIAL CESTA POCHADZA, R - RIP, C - CONNECTED, '-' - INE
    struct in6_addr prefix; // IPv6 ADRESA
    uint8_t prefixLen; // ak budeme pouzivat len 128 dlzky, tento atribut nam zrejme netreba
    struct in6_addr prefixNextHop;
    char nextHopInt[10]; // interface k nexthopu z kade prisiel packet
    uint8_t metric; // METRIKA
    time_t time; // kolko je zaznam v tabulke
};

// SMEROVACIA TABULKA
struct routeTable {
    struct Route * head;
    unsigned int count;
};

// TELO RIPng
struct ripEntry {
    struct in6_addr prefix;
    uint16_t routeTag;
    uint8_t prefixLen;
    uint8_t  metric;
}__attribute__((packed));

// RIPng HLAVICKA
struct ripHdr {
    uint8_t cmd;
    uint8_t ver;
    uint16_t empty;
    struct ripEntry entry[0];
}__attribute__((packed));

// struktura, ktora obsahuje parametre pre vlakna
struct threadParams {
    struct intTable * interfaces; // tabulka rozhrani
    struct routeTable * routes; // smerovacia tabulka
    char intName[10]; // MENU INTRFACU
    struct in6_addr prefixLL; // LL adresa interfacu
    struct in6_addr prefixNetwork; // adresa siete kam patri interface
    uint8_t prefixNetworkLen; // dlzka
    int socketParam; // bindnuty socket
    pthread_mutex_t lock; // mutex
};



#ifdef __cplusplus
}
#endif

#endif /* STRUCTURES_H */

