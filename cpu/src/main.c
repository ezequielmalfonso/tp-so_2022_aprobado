/*
 * main.c
 *
 *  Created on: Sep 27, 2022
 *      Author: utnso
 */

#include "main.h"

int main(){
	cargarConfiguracion();
	//sem_init(&sem,0,1);

	//CLIENTE
	int memoria_fd;
	generar_conexion(&memoria_fd, configuracion);

	char* puertoInterrupt = string_itoa(configuracion->PUERTO_ESCUCHA_INTERRUPT);
    char* puertoDispatch= string_itoa(configuracion->PUERTO_ESCUCHA_DISPATCH);
	//INICIO SERVIDORES
	cpuServerInterrupt = iniciar_servidor(logger,"interrupt server","127.0.0.1",puertoInterrupt);

    cpuServerDispatch = iniciar_servidor(logger,"dispatch server","127.0.0.1",puertoDispatch);

    free(puertoInterrupt);
    free(puertoDispatch);
	//while (servers_escuchar("INTERRUPT_SV", cpuServerInterrupt,"DISPATCH_SV", cpuServerDispatch));

	pthread_t dispatch_id;
	pthread_t interrupt_id;

	pthread_create(&dispatch_id,NULL,(void*) dispatchCpu,NULL);
	pthread_create(&interrupt_id,NULL,(void*) interruptCpu,NULL);

	pthread_join(interrupt_id,0);
	pthread_join(dispatch_id,0);

	limpiarConfiguracion();
	return 0;

}

void dispatchCpu() {

	//cargarConfiguracion();

	//char* puertoDispatch = string_itoa(configuracion->PUERTO_ESCUCHA_DISPATCH);

	//cpuServerDispatch = iniciar_servidor(logger,"dispatch server","127.0.0.1",(char*) puerto);

	//free(puerto);

	while(server_escuchar("DISPATCH_SV",cpuServerDispatch));

}

void interruptCpu() {

	//cargarConfiguracion();

	//char* puertoInterrupt = string_itoa(configuracion->PUERTO_ESCUCHA_INTERRUPT);

	//cpuServerInterrupt = iniciar_servidor(logger,"interrupt server","127.0.0.1",puertoInterrupt);

	//free(puertoInterrupt);

	while(server_escuchar("INTERRUPT_SV",cpuServerInterrupt));
}

