/*
 * planificacion.h
 *
 *  Created on: Sep 29, 2022
 *      Author: utnso
 */

#ifndef SRC_PLANIFICACION_H_
#define SRC_PLANIFICACION_H_

#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<semaphore.h>
#include<commons/collections/queue.h>
#include<commons/collections/list.h>
#include<commons/collections/dictionary.h>
#include "protocolo.h"
#include "main.h"

extern pthread_mutex_t mx_cola_new;
extern pthread_mutex_t mx_lista_ready;
extern pthread_mutex_t mx_lista_block;
extern pthread_mutex_t mx_lista_new;
extern pthread_mutex_t mx_log;

extern sem_t s_pasaje_a_ready, s_ready_execute,s_cpu_desocupado,s_cont_ready,s_multiprogramacion_actual;

extern t_queue* cola_new;
extern t_list* lista_ready;
extern t_list* cola_blocked;
//t_dictionary* iteracion_blocked; no se que chota es

void esperar_cpu();

#endif /* SRC_PLANIFICACION_H_ */
