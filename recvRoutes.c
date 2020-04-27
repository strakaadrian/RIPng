/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "recvRoutes.h"
#include "routeTable.h"

#define PORT 521
#define BUFF_SIZE 2000

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
	exit(EXIT_FAILURE);
    }
    
    // potrebujeme socketu nastavit aby pocuvalo viacero socketov na rovnakom porte
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)
    { 
	printf("Nepodarilo sa priradit moznosti socketu");
	exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin6_family  = AF_INET6;
    addr.sin6_port = htons(PORT);
    
    // nastavime socketu aby pocuval na specifickom interfaci
    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, paThrParams->intName, 10) < 0)
    { 
	printf("Nepodarilo sa priradit moznosti socketu");
	exit(EXIT_FAILURE);
    }
    
    // priradenie parametrov vysie nastavenych k socketu
    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
	printf("Nepodarilo sa priradit socket k adrese");
        perror("Error: ");
	exit(EXIT_FAILURE);
    }
    
    // potrebujeme pridat do multicastovej skupiny
    struct ipv6_mreq multicast;
    memset(&multicast, 0, sizeof(multicast));
    //multicast.ipv6mr_multiaddr = in6addr_any; // chceme pocuvat na hociktorom rozhrani
    
    multicast.ipv6mr_interface = if_nametoindex(paThrParams->intName); // vsetky rozhrania
    
    // chceme sa joinut do tejto multicastovej skupiny, lebo tam RIP posiela pakety
    if(inet_pton(AF_INET6, "ff02::9", &multicast.ipv6mr_multiaddr) == 0) {
        printf("Nepodarilo sa joinut do multicastovej skupiny");
	exit(EXIT_FAILURE);
    }
    
    //TODO musime joinovat podle int tabulky vsetky interfacy ktore su zapojene takze nejake FOR
    
    // nastavenie parametrov pre SOCK, to su nejake dodatocne parametre, ktore vieme pre socket nastavit
    if(setsockopt(sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &multicast, sizeof(struct ipv6_mreq) ) == -1) {
        printf("Nepodarilo sa priradit dodatocne parametre socketu");
	exit(EXIT_FAILURE);
    }
 
    // teraz ideme pocuvat pakety
    for(;;) {
        char buf[BUFF_SIZE];
	int readLen = 0;

	memset(buf, 0, BUFF_SIZE);
	int addr_len = sizeof(addr);
	readLen = recvfrom(sock, buf, BUFF_SIZE, 0, (struct sockaddr *) &addr, &addr_len);
        
        //SPRACOVANIE RIPng HLAVICKY
        // buffer ideme prerobit na RIPng strukturu 
        struct ripHdr *hdr;
        hdr = (struct ripHdr *)buf;
        
        // odcitame od celkoveho buffera velkost hlavicky
        readLen -= sizeof(struct ripHdr);
        
        // teraz ideme spracovavat RIPng smerovacie zaznamy
        struct ripEntry *entry;
        struct ripEntry *nextHop;
        
        //vlozime si do entry prvu polozku
        entry = (struct ripEntry *)hdr->entry;
        
        // teraz ideme v cykle spracovavat RIPng Entries
        while(readLen >= sizeof(struct ripEntry)) {

            addRoute(paThrParams->routes, 'R', entry->prefix, entry->prefixLen, entry->metric, paThrParams->intName, paThrParams->lock);
            
            readLen -= sizeof(struct ripEntry);
            //posuniem sa na dalsiu polozku
            entry++;
        }
        

    }
}