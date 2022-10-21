/*
 * pcb.c
 *
 *  Created on: 24 sept 2022
 *      Author: utnso
 */

#include "pcb.h"


PCB_t* pcb_create(){
	PCB_t* pcb = malloc(sizeof(PCB_t));
	pcb->instrucciones = list_create();
	pcb->segmentos=list_create();
	//TODO ver como iniciar los valores de REG_USO_GRAL_CPU y TABLA_SEGMENTOS
	return pcb;
}

void pcb_set(PCB_t* pcb, uint16_t pid, t_list* instrucciones, uint32_t pc, REG_USO_GRAL_CPU* registro_cpu, t_list* segmentos){

	//list_destroy_and_destroy_elements(pcb->instrucciones,free);
	list_add_all(pcb->instrucciones,instrucciones);
	pcb->pid=pid;
	pcb->pc = pc;
	//antes de cambiar de puntero, destruyo toda existencia de la anterior
	pcb->registro_cpu = registro_cpu;
	//list_destroy_and_destroy_elements(pcb->segmentos,free);
    list_add_all(pcb->segmentos,segmentos);
}


int pcb_find_index(t_list* lista, uint16_t pid){
	for (int i = 0; i < list_size(lista); i++){
		PCB_t* otro_pcb = list_get(lista,i);
		if (otro_pcb->pid == pid)
			return i;
	}
	return -1;
}

void pcb_destroy(PCB_t* pcb){
	list_destroy_and_destroy_elements(pcb->instrucciones,free);
	free(pcb);
}

