/*
 * protocolo.h
 *
 *  Created on: Sep 6, 2022
 *      Author: utnso
 */

#ifndef SRC_PROTOCOLO_H_
#define SRC_PROTOCOLO_H_

#include <inttypes.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include "estructuras.h"


typedef enum {
    DEBUG = 69,
	SET,
	ADD,
	MOV_IN,
	MOV_OUT,
	IO,
	EXIT
} op_code;

typedef struct{
	int elementosLista;
	t_list* listaInstrucciones;
    char* segmentos[4];
} t_instrucciones;

typedef struct {
    uint32_t size; // Tamaño del payload
    void* stream; // Payload
} t_buffer;

bool send_debug(int fd);

//INSTRUCCIONES CONSOLA-KERNEL
void* serializar_instrucciones_tam(uint32_t size, t_list* lista, char* segmento);
t_instrucciones* deserializar_instrucciones(t_buffer* buffer);
void enviar_instrucciones(int socket_fd, t_list* lista, char* segmentos);
t_instrucciones* recibir_instrucciones(int socket_fd);
uint32_t calcular_instrucciones_buffer_size(t_list* lista);

#endif /* SRC_PROTOCOLO_H_ */
