/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "interfaceTable.h"


struct intTable * createIntTable() {
    struct intTable * table;
    table = (struct intTable *)malloc(sizeof(struct intTable));
    
    if(table == NULL) {
        printf("ERROR: Nepodarilo sa vytvorit tabulku");
	return NULL;
    }
    
    memset(table, 0, sizeof(struct intTable));

    return table;
}

struct Interface * addInterface(struct intTable * paTable, unsigned int paIntIndex, char * paIntName, struct in6_addr paPrefix, uint8_t paPrefixLen, struct in6_addr paPrefixLL, bool paRip, bool paPassive) {
    
    if(paTable == NULL) {
        printf("ERROR: Tabulka neexistuje");
	return NULL;
    }
    
    struct Interface * interface;
    
    interface = (struct Interface *)malloc(sizeof(struct Interface));
    
    if (interface == NULL) {
	printf("ERROR: Nepodarilo sa vytvorit interface");
	return NULL;
    }
    
    interface->intId = paIntIndex;
    strcpy(interface->intName, paIntName);
    interface->prefix = paPrefix;
    interface->prefixLen = paPrefixLen;
    interface->prefixLL = paPrefixLL;
    interface->rip = paRip;
    interface->passiveInt = paPassive;
    
    
    struct Interface * entry = paTable->head;
    
    // ak v tabulke este nic nieje ako dalsi prvok pridaj novy interface
    // ak v tabulke uz je interface tak svoj novy vloz na zaciatok a odkaz na dalsi daj do next
    if(entry == NULL) {
        paTable->head = interface;
        interface->next = NULL;
    } else  {
        interface->next = paTable->head;
        paTable->head = interface;
    }
    
    paTable->count++;
    
    return interface;
}

void printIntTable(struct intTable * paTable) {

	if(paTable == NULL) {
		printf("ERROR: Tabulka neexistuje");
		return;
	}

	struct Interface * interface = NULL;
	interface = paTable->head;

	printf("| Name |         Address        | Length | RIP | Passive \n");

	while(interface != NULL) {
                char prefix[INET6_ADDRSTRLEN];
            
		printf("| %s |", interface->intName);

		inet_ntop(AF_INET6, &interface->prefix, prefix, sizeof(prefix));
                printf(" %s |", prefix);
                
		printf("   %hhu   |", interface->prefixLen);
                
                printf("  %d  |", interface->rip);
                
                printf("  %d \n", interface->passiveInt);

		interface = interface->next;
	}

}

void destroyIntTable(struct intTable * paTable) {
    if(paTable == NULL) {
        printf("ERROR: Tabulka neexistuje");
	return;
    }
    struct Interface * interface;
    struct Interface * pom;
    
    interface = paTable->head;
    
    while(interface != NULL) {
	pom = interface;
	interface = interface->next;
	free(pom);
    }

    free(paTable);
}
