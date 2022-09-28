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
#include "protocolo.h"
#include "comunicacion.h"

int cpuServerInterrupt;
int cpuServerDispatch;

void dispatchCpu();
void interruptCpu ();

#endif /* SRC_MAIN_H_ */
