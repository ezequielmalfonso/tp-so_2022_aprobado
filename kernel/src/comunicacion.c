/*
 * comunicacion.c
 *
 *  Created on: Sep 12, 2022
 *      Author: utnso
 */

#include "comunicacion.h"
#include "pcb.h"

typedef struct {
    int fd;
    char* server_name;
} t_procesar_conexion_args;

static void procesar_conexion(void* void_args) {
	t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char* server_name = args->server_name;
	free(args);

	t_instrucciones* mensaje=malloc(sizeof(t_instrucciones));
	mensaje=recibir_instrucciones(cliente_socket);
	//printf("Segmento % \n", &mensaje->segmentos[0]);

	log_info(logger, "La consola se desconecto de %s server", server_name);


	PCB_t* proceso = pcb_create();

	//TODO pasarle los valores de inicializacion al PCB
	//pcb_set(proceso, pid_actual, mensaje,      0,  registro_cpu,  tabla_segmentos);
	       //( pcb,       pid,  instrucciones,  pc,  registro_cpu,  tabla_segmentos);


	//pcb=crear_pcb(mensaje);
	//AGREGAR A COLA DE NEWS
	//enviar_pcb(dispatch_fd, pcb);
	//log_info(logger,"llegue");

	//liberar_conexion(cliente_socket);

	return;
}

int server_escuchar(char* server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);


    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}

//CLIENTE
//CPU
int generar_conexiones(int* interrupt_fd, int* dispatch_fd, t_config_kernel* configuracion) {
    char* port_dispatch = string_itoa(configuracion->PUERTO_CPU_DISPATCH);
    char* port_interrupt = string_itoa(configuracion->PUERTO_CPU_INTERRUPT);

    *dispatch_fd = crear_conexion(
            logger,
            "CPU DISPATCH",
            configuracion->IP_CPU,
            port_dispatch
    );

    free(port_dispatch);

    *interrupt_fd = crear_conexion(
            logger,
            "CPU INTERRUPT",
            configuracion->IP_CPU,
            port_interrupt
    );

    free(port_interrupt);

    return *interrupt_fd != 0 && *dispatch_fd != 0;
}

//MEMORIA
int generar_conexion_memoria(int* memoria_fd, t_config_kernel* configuracion) {
    char* port_memoria = string_itoa(configuracion->PUERTO_MEMORIA);

    *memoria_fd = crear_conexion(
            logger,
            "MEMORIA",
            configuracion->IP_MEMORIA,
            port_memoria
    );

    free(port_memoria);

    return *memoria_fd != 0;
}

