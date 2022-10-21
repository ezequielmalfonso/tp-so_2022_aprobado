/*
 * main.c
 *
 *  Created on: Sep 6, 2022
 *      Author: utnso
 */

#include "main.h"

int interrupt_fd, dispatch_fd, memoria_fd;

int main(){

	cargarConfiguracion();
	inicializarPlanificacion();
	char* puerto = string_itoa(configuracion->PUERTO_ESCUCHA);



	//CLIENTE
	//CPU
	generar_conexiones(&interrupt_fd, &dispatch_fd, configuracion);
	//MEMORIA
	generar_conexion_memoria(&memoria_fd, configuracion);

	//INICIO SERVIDOR
		int kernelServer= iniciar_servidor(logger,"kernel server","127.0.0.1",puerto);
		free(puerto);
		while (server_escuchar("KERNEL_SV", kernelServer));

	limpiarConfiguracion();
	return 0;
}
