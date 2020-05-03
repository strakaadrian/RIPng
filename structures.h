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
    struct Interface * next; // dalsi prvok
    unsigned int intId; //id interfacu
    char intName[10]; // Meno interfacu
    struct in6_addr prefix; // IPv6 adresa
    uint8_t prefixLen; // dlzka IPv6 adresy
    struct in6_addr prefixLL; // IPv6 ADRESA LINK LOCAL
    struct in6_addr prefixNetwork; // IPv6 ADRESA siete kam interface patri
    uint8_t prefixNetworkLen; // dlzka prefixu pre siet rozhrania
    bool rip; // JE, ALEBO NIEJE ZAPNUTE RIP NA DANOM INTERFACE
    bool passiveInt; // JE ROZHRANIE PASIVNE ?
};

// TABULKA ROZHRANI
struct intTable {
    struct Interface * head; // prvy zaznam typu Interface v tabulke
    unsigned int count; // pocet rozhrani v tabulke
};

// SMEROVACI ZAZNAM
struct Route {
    struct Route * next;
    char origin; // ODKADIAL CESTA POCHADZA, R - RIP, C - CONNECTED, '-' - INE
    struct in6_addr prefix; // IPv6 ADRESA
    uint8_t prefixLen; // dlzka IPv6 adresy
    struct in6_addr prefixNextHop; // next hop IPv6 adresa
    char nextHopInt[10]; // meno interfacu k nexthopu z kade prisiel packet
    uint8_t metric; // METRIKA
    time_t time; // kolko je zaznam v tabulke
};

// SMEROVACIA TABULKA
struct routeTable {
    struct Route * head; // prvy zaznam typu Route v tabulke
    unsigned int count; // pocet smerovacich zaznamov v tabulke
};

// TELO RIPng
struct ripEntry {
    struct in6_addr prefix; // IPv6 adresa, ktoru posielam
    uint16_t routeTag; // route tag
    uint8_t prefixLen; // dlzka IPv6 adresy
    uint8_t  metric; // metrika
}__attribute__((packed));

// RIPng HLAVICKA
struct ripHdr {
    uint8_t cmd; // command
    uint8_t ver; // version
    uint16_t empty; // nic
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

