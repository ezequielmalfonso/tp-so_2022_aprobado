/*
 * comunicacion.c
 *
 *  Created on: Sep 27, 2022
 *      Author: utnso
 */

#include "comunicacion.h"

typedef struct {
    int fd;
    char* server_name;
} t_procesar_conexion_args;

//KERNEL
static void procesar_kernel(void* void_args) {
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
 case CREAR_TABLA:
 {
	uint16_t pid = 123;
	uint16_t id_segmento = 123;
	int cantidad_ids = 0;
	op_code op;



	recv(cliente_socket, &pid, sizeof(uint16_t), 0);
	recv(cliente_socket, &cantidad_ids, sizeof(int), 0);

	log_info(logger, "[KERNEL] Creando tabla para programa %d", pid);

	int i=0;
	while(i<cantidad_ids){
		    int sid = 0;
		    int tamanio = 0;
		    recv(cliente_socket,&sid ,sizeof(int),0);
		    recv(cliente_socket,&tamanio, sizeof(int),0);
		    uint32_t tabla_paginas = crear_tabla(pid, tamanio);
		    log_info(logger, "[KERNEL] Tabla creada del Segmento %d con %d entradas ", sid, tabla_paginas);
	    	list_add(lista_tablas_de_paginas, tabla_paginas);
	    	i++;
	    }
	send(cliente_socket,&op,sizeof(op_code),0);
	break;
 }
 case ELIMINAR_ESTRUCTURAS:
 {
	uint32_t tabla_paginas = 0;
	uint16_t pid = 0;

	recv(cliente_socket, &tabla_paginas, sizeof(uint32_t),0);
	recv(cliente_socket, &pid, sizeof(uint16_t),0);

	log_info(logger, "[KERNEL] Eliminando tablas del proceso %d", pid);
	eliminar_estructuras(tabla_paginas, pid);

	break;
 }
 case PAGEFAULT:
 {
	op_code op = PAGEFAULT;
	send(cliente_socket, &op, sizeof(op_code),0);
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

int kernel_escuchar(char* server_name, int server_socket){
	int cliente_socket = esperar_cliente(logger,server_name, server_socket);
	//sem_wait(&sem);
	if (cliente_socket != -1 ) {
			t_procesar_conexion_args* argsSev = malloc(sizeof(t_procesar_conexion_args));
			argsSev->fd = cliente_socket;
			argsSev->server_name = server_name;
			procesar_kernel(argsSev);
	        return 1;
	       // sem_post(&sem);
		}
	return 0;

}

//CPU
static void procesar_cpu(void* void_args) {
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
 	case INICIALIZAR:
 	{
 		send(cliente_socket, &(configuracion->ENTRADAS_POR_TABLA), sizeof(uint16_t),0);
 		send(cliente_socket, &(configuracion->TAM_PAGINA), sizeof(uint16_t),0);
 		break;
 	}
 	case SOLICITUD_NRO_MARCO:
	 {
 		log_info(logger, "[CPU][ACCESO A MEMORIA] Procesando solicitud de nro marco...");
 		uint32_t nro_pag = 0;
 		uint32_t index = 0;
 		uint16_t pid = 0;

 		recv(cliente_socket, &pid, sizeof(uint16_t),0);
 		recv(cliente_socket, &nro_pag, sizeof(int32_t),0);
 		recv(cliente_socket, &index, sizeof(uint32_t),0);

 		uint32_t nro_marco = obtener_nro_marco_memoria(nro_pag,index);
 		log_info(logger, "[CPU] Numero de marco obtenido = %d", nro_marco);
 		usleep(configuracion->RETARDO_MEMORIA * 1000);
		 send(cliente_socket, &nro_marco, sizeof(uint32_t), 0);
 	break;
 	}
 	case MOV_IN:
 	{
 		log_info(logger, "[CPU][ACCESO A MEMORIA] Procesando lectura...");
 		uint32_t nro_marco;
 		uint32_t desplazamiento;

 		recv(cliente_socket, &nro_marco, sizeof(uint32_t),0);
 		recv(cliente_socket, &desplazamiento, sizeof(uint32_t),0);

		 uint32_t dato = read_en_memoria(nro_marco, desplazamiento);

 		usleep(configuracion->RETARDO_MEMORIA * 1000);
 		send(cliente_socket, &dato, sizeof(uint32_t), 0);

 	break;
 	}
 	case MOV_OUT:
 	{
 		log_info(logger, "[CPU][ACCESO A MEMORIA] Procesando escritura...");
		uint32_t nro_marco;
		 uint32_t desplazamiento;
 		uint32_t dato;

 		recv(cliente_socket, &nro_marco, sizeof(uint32_t),0);
 		recv(cliente_socket, &desplazamiento, sizeof(uint32_t),0);
 		recv(cliente_socket, &dato, sizeof(uint32_t),0);
 		//leer el valor del registro que se guardaria en dato

		 write_en_memoria(nro_marco, desplazamiento, dato);
 		log_info(logger, "[CPU] Dato escrito: '%d' (nro_marco = %d, desplazamiento = %d)", dato, nro_marco, desplazamiento);

		 op_code resultado = ESCRITURA_OK;
 		usleep(configuracion->RETARDO_MEMORIA * 1000);
 		send(cliente_socket, &resultado, sizeof(op_code), 0);

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

int cpu_escuchar(char* server_name, int server_socket){
	int cliente_socket = esperar_cliente(logger,server_name, server_socket);
	if (cliente_socket != -1 ) {
			t_procesar_conexion_args* argsSev = malloc(sizeof(t_procesar_conexion_args));
			argsSev->fd = cliente_socket;
			argsSev->server_name = server_name;
			procesar_cpu(argsSev);
	        return 1;
		}
	return 0;

}
