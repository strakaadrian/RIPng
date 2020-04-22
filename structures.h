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
    struct in6_addr nextHop; // NEXT-HOP smerovac
    uint8_t nextHopLen; // ak budeme pouzivat len 128 dlzky, tento atribut nam zrejme netreba
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



#ifdef __cplusplus
}
#endif

#endif /* STRUCTURES_H */

