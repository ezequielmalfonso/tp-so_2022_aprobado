/*
 * main.c
 *
 *  Created on: Sep 6, 2022
 *      Author: utnso
 */

#include "main.h"

//t_config_kernel* configuracion;

int main(){
	cargarConfiguracion();
	char* puerto = string_itoa(configuracion->PUERTO_ESCUCHA);




	//INICIO SERVIDOR
		int kernelServer= iniciar_servidor(logger,"kernel server","127.0.0.1",puerto);
		free(puerto);
		while (server_escuchar("KERNEL_SV", kernelServer));

	limpiarConfiguracion();
	return 0;
}
