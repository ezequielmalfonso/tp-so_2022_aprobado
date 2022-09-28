/*
 * main.c
 *
 *  Created on: Sep 27, 2022
 *      Author: utnso
 */

#include "main.h"

int main() {
	cargarConfiguracion();

	char* puerto= string_itoa(configuracion->PUERTO_ESCUCHA);
	//INICIO SERVIDORES
    memoriaServer = iniciar_servidor(logger,"memoria_sv","127.0.0.1",puerto);

	pthread_t hilo_kernel;
	pthread_t hilo_cpu;

	pthread_create(&hilo_kernel, NULL, (void*) escuchar_kernel, NULL);
	pthread_create(&hilo_cpu, NULL, (void*) escuchar_cpu, NULL);

	pthread_join(hilo_kernel,NULL);
	pthread_join(hilo_cpu, NULL);

	return 0;

}

void escuchar_kernel() {
	while(kernel_escuchar("memoria_sv",memoriaServer));
}
void escuchar_cpu() {
	while(cpu_escuchar("memoria_sv",memoriaServer));
}
