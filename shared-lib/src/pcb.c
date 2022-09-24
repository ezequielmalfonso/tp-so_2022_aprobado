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
	//TODO ver como iniciar los valores de REG_USO_GRAL_CPU y TABLA_SEGMENTOS
	return pcb;
}

void pcb_set(PCB_t* pcb, uint32_t pid, t_list* instrucciones, uint32_t pc, REG_USO_GRAL_CPU registro_cpu, TABLA_SEGMENTOS tabla_segmentos){

	pcb->pid = pid;
	pcb->instrucciones = instrucciones;
	pcb->pc = pc;
	//antes de cambiar de puntero, destruyo toda existencia de la anterior
	list_destroy_and_destroy_elements(pcb->instrucciones,free);
	pcb->registro_cpu = registro_cpu;
	pcb->tabla_segmentos = tabla_segmentos;
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

