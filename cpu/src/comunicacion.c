/*
 * comunicacion.c
 *
 *  Created on: Sep 27, 2022
 *      Author: utnso
 */

#include "comunicacion.h"

//PROCESAR CONEXIONES SEPARADAS
typedef struct {
	int fd;
	char* server_name;
} t_procesar_conexion_args;

int cliente_socket, segmento, pagina;

static void procesar_conexion(void* void_args) {
 t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
 int cliente_socket = args->fd;
 char* server_name = args->server_name;
 free(args);

 op_code cop;
 while (cliente_socket != -1) {

 if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
 log_info(logger, "Se ha finalizado la conexion");
 return;
 }

 switch (cop) {
 case DEBUG:
 log_info(logger, "debug");
 break;
 case DISPATCH: {
	 PCB_t* proceso= pcb_create();
	 recv_proceso(cliente_socket,proceso);
	 log_info(logger,"recibi pcb");
	 limpiar_tlb();
	 op_code codigo=iniciar_ciclo_instruccion(proceso);
	 send_proceso(cliente_socket,proceso,codigo);
	 if(codigo==PAGEFAULT){
	   log_error(logger,"Page Fault PID: %d - Segmento: %d - Pagina: %d",proceso->pid,segmento,pagina);
	   send(cliente_socket, &segmento, sizeof(int),0);
	   send(cliente_socket, &pagina, sizeof(int),0);
	   segmento=0;
	   pagina=0;
	 }
	 break;
 }
 // Errores
 case -1:
 log_error(logger, "Cliente desconectado de %s...", server_name);
 return;
 default:
 log_error(logger, "Algo anduvo mal en el server de %s", server_name);
 log_info(logger, "Cop: %d", cop);
 return;
 }
 }

 log_warning(logger, "El cliente se desconecto de %s server", server_name);
 return;
 }

int server_escuchar(char* server_name, int server_socket){
	cliente_socket = esperar_cliente(logger,server_name, server_socket);
	//sem_wait(&sem);
	if (cliente_socket != -1 ) {
			t_procesar_conexion_args* argsSev = malloc(sizeof(t_procesar_conexion_args));
			argsSev->fd = cliente_socket;
			argsSev->server_name = server_name;
			procesar_conexion(argsSev);
	        return 1;
	       // sem_post(&sem);
		}
	return 0;

}

//CLIENTE
//MEMORIA
int generar_conexion(int* memoria_fd, t_config_cpu* configuracion) {
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
