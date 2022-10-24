/*
 * main.h
 *
 *  Created on: Sep 27, 2022
 *      Author: utnso
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include "socket.h"
#include "cpuConfig.h"
#include "comunicacion.h"

int cpuServerInterrupt;
int cpuServerDispatch;

void dispatchCpu();
void interruptCpu ();

op_code iniciar_ciclo_instruccion(PCB_t* pcb);
INSTRUCCION* fetch(t_list* instrucciones, uint32_t pc);
int decode(INSTRUCCION* instruccion_ejecutar );
int check_interrupt();
void interrupcion();
int execute(INSTRUCCION* instruccion_ejecutar,REG_USO_GRAL_CPU* registros);


#endif /* SRC_MAIN_H_ */
