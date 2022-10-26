/*
 * main.c
 *
 *  Created on: Sep 27, 2022
 *      Author: utnso
 */

#include "main.h"

int cpuServerDispatch, cpuServerInterrupt;
pthread_mutex_t mx_hay_interrupcion = PTHREAD_MUTEX_INITIALIZER;

bool hay_interrupcion;

int main(){
	cargarConfiguracion();
	hay_interrupcion=false;
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

//EJECUCION DE INSTRUCCIONES
op_code iniciar_ciclo_instruccion(PCB_t* pcb){
	op_code estado = CONTINUE;
	while (estado == CONTINUE){ // Solo sale si hay una interrupcion, un pedido de I/O, o fin de ejecucion
		INSTRUCCION* instruccion_ejecutar = fetch(pcb->instrucciones, pcb->pc);

		if(decode(instruccion_ejecutar)){
			log_info(logger,"En CPU");
			//usleep(configuracion->RETARDO_INSTRUCCION);

		}
		estado = execute(instruccion_ejecutar,pcb->registro_cpu);
		if(estado == CONTINUE){
			estado = check_interrupt();
		}
		pcb->pc++;
	}
	return estado;
}

INSTRUCCION* fetch(t_list* instrucciones, uint32_t pc){
	return list_get(instrucciones,pc);
}

int decode(INSTRUCCION* instruccion_ejecutar ){
	return (!strcmp(instruccion_ejecutar->comando,"SET")|| !strcmp(instruccion_ejecutar->comando,"ADD"));
}

int check_interrupt(){
	pthread_mutex_lock(&mx_hay_interrupcion);
	if (hay_interrupcion){
		hay_interrupcion = false;
		pthread_mutex_unlock(&mx_hay_interrupcion);
		return INTERRUPTION;
	}
	pthread_mutex_unlock(&mx_hay_interrupcion);
	return CONTINUE;
}

void interrupcion(){
	//cambia hay_interrupcion a true
	while(1){
		op_code opcode;
		recv(cpuServerInterrupt, &opcode, sizeof(op_code), 0);
//		log_info(logger, "Interrupcion recibida");
		pthread_mutex_lock(&mx_hay_interrupcion);
		hay_interrupcion = true;
		pthread_mutex_unlock(&mx_hay_interrupcion);
	}
}

int execute(INSTRUCCION* instruccion_ejecutar,uint32_t registros[4]){

	if(!strcmp(instruccion_ejecutar->comando,"SET")){
		log_info(logger,"Ejecutando SET");
		int i=0;
		uint32_t valor= atoi(instruccion_ejecutar->parametro2);
		switch(instruccion_ejecutar->parametro[0]){
		case 'A':
			registros[0]=valor;
			break;
		case 'B':
			registros[1]=valor;
			break;
		case 'C':
			registros[2]=valor;
			break;
		case 'D':
			registros[3]=valor;
			break;
		}
	}else if(!strcmp(instruccion_ejecutar->comando,"ADD")){
		log_info(logger,"Ejecutando ADD");
		int i=0, destino, origen;
		switch(instruccion_ejecutar->parametro[0]){
		case 'A':
			destino=0;
			break;
		case 'B':
			destino=1;
			break;
		case 'C':
			destino=2;
			break;
		case 'D':
			destino=3;
			break;
		}
		switch(instruccion_ejecutar->parametro2[0]){
		case 'A':
			origen=0;
			break;
		case 'B':
			origen=1;
			break;
		case 'C':
			origen=2;
			break;
		case 'D':
			origen=3;
			break;
		}
		registros[destino]+=registros[origen];

	}else if(!strcmp(instruccion_ejecutar->comando,"MOV_OUT")){
		log_info(logger,"Ejecutando MOV_OUT");

		//log_info(logger,"Valor leido %d",ejecutarRead(instruccion_ejecutar->arg1,tabla_paginas));

	}else if(!strcmp(instruccion_ejecutar->comando,"MOV_IN")){
		log_info(logger,"Ejecutando MOV_IN");
		//ejecutarWrite(instruccion_ejecutar->arg1,instruccion_ejecutar->arg2,tabla_paginas);
	}else if(!strcmp(instruccion_ejecutar->comando,"IO")){
		log_info(logger,"Ejecutando IO");
		return BLOCKED;
	}else if(!strcmp(instruccion_ejecutar->comando,"EXIT")){
		log_info(logger,"Ejecutando EXIT");
		return EXIT;
	}else{
		log_error(logger,"Hubo un error en el ciclo de instruccion");
	}

	return CONTINUE;
}

