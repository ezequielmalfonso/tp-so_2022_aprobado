/*
 * comunicacion.h
 *
 *  Created on: Sep 27, 2022
 *      Author: utnso
 */

#ifndef SRC_COMUNICACION_H_
#define SRC_COMUNICACION_H_

#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include "protocolo.h"
#include "socket.h"
#include "cpuConfig.h"
#include "main.h"
#include <semaphore.h>

//sem_t sem;

int servers_escuchar(char* server_name, int server_socket, char* server_dispatch_name, int server_dispatch_socket);
int server_escuchar(char* server_name, int server_socket);

extern int cliente_socket;

extern uint32_t segmento, pagina;

#endif /* SRC_COMUNICACION_H_ */
