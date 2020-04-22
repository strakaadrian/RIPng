/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "recvRoutes.h"

#define PORT 520
#define IF "tap0"

void * recvRoutes(void *par) {
    // dostaneme nasu strukturu s parametrami, ktore sme poslali vlaknu
    struct threadParams * paThrParams = (struct threadParams *) par;
    
    // vytvor socket na ktorom budes pocuvat
    int sock;
    // priprav si premennu pre adresu IPV6
    struct sockaddr_in6 addr;
    
    // VYTVORENIE SOCKETU
    sock = socket(AF_INET6, SOCK_DGRAM, 0);
    
    // ak sa nepodari vytvorit socket na pocuvanie, cely smerovac padne
    if(sock == -1)
    { 
	printf("Nepodarilo sa vytvorit socket pre pocuvanie");
        pthread_mutex_lock(&paThrParams->lock); // zamkni mutex
        paThrParams->exitStatus = true;
        pthread_mutex_unlock(&paThrParams->lock);
	exit(EXIT_FAILURE);
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family  = AF_INET6;
    addr.sin6_port = htons(PORT);
    addr.sin6_addr = in6addr_any; // pocuvaj na vsetkych rozhraniach
    
    // priradenie parametrov vysie nastavenych k socketu
    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
	printf("Nepodarilo sa priradit socket k adrese");
        pthread_mutex_lock(&paThrParams->lock); // zamkni mutex
        paThrParams->exitStatus = true;
        pthread_mutex_unlock(&paThrParams->lock);
	exit(EXIT_FAILURE);
    }
    
    // potrebujeme pridat do multicastovej skupiny
    struct ipv6_mreq multicast;
    memset(&multicast, 0, sizeof(multicast));
    multicast.ipv6mr_multiaddr = in6addr_any; // chceme pocuvat na hociktorom rozhrani
    
    
    multicast.ipv6mr_interface = 0; // vsetky rozhrania
    
    // chceme sa joinut do tejto multicastovej skupiny, lebo tam RIP posiela pakety
    if(inet_pton(AF_INET6, "ff02::9", &multicast.ipv6mr_multiaddr) == 0) {
        printf("Nepodarilo sa joinut do multicastovej skupiny");
        pthread_mutex_lock(&paThrParams->lock); // zamkni mutex
        paThrParams->exitStatus = true;
        pthread_mutex_unlock(&paThrParams->lock);
	exit(EXIT_FAILURE);
    }
    
    // nastavenie parametrov pre SOCK, to su nejake dodatocne parametre, ktore vieme pre socket nastavit
    if(setsockopt(sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &multicast, sizeof(struct ipv6_mreq) ) == -1) {
        printf("Nepodarilo sa priradit dodatocne parametre socketu");
        pthread_mutex_lock(&paThrParams->lock); // zamkni mutex
        paThrParams->exitStatus = true;
        pthread_mutex_unlock(&paThrParams->lock);
	exit(EXIT_FAILURE);
    }
    
    // teraz ideme pocuvat pakety
    for(;;) {
        // ukoncenie pocuvania
        if(paThrParams->exitStatus == true) {
            
        }
        
    }
    
    
    
}