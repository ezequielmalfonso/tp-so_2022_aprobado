/*
 * main.c
 *
 *  Created on: Sep 27, 2022
 *      Author: utnso
 */

#include "main.h"

int cpuServerDispatch, cpuServerInterrupt;
pthread_mutex_t mx_hay_interrupcion = PTHREAD_MUTEX_INITIALIZER;
uint16_t tam_pagina;
uint16_t cant_ent_por_tabla;
t_list* tlb;
bool hay_interrupcion;
int memoria_fd;
int main(){
	cargarConfiguracion();
	hay_interrupcion=false;
	tlb=list_create();
    inicializar_tlb();

	//sem_init(&sem,0,1);

	//CLIENTE

	generar_conexion(&memoria_fd, configuracion);
	recv(memoria_fd, &tam_pagina, sizeof(uint16_t), 0);
	recv(memoria_fd, &cant_ent_por_tabla, sizeof(uint16_t), 0);

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
	pthread_t hilo_interrupciones;


	pthread_create(&dispatch_id,NULL,(void*) dispatchCpu,NULL);
	pthread_create(&interrupt_id,NULL,(void*) interruptCpu,NULL);
	pthread_create(&hilo_interrupciones,NULL,(void*)interrupcion,NULL);

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
			usleep(configuracion->RETARDO_INSTRUCCION);

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
		return INTERRUPT;
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
		if(opcode==INTERRUPT){
		pthread_mutex_lock(&mx_hay_interrupcion);
		hay_interrupcion = true;
		pthread_mutex_unlock(&mx_hay_interrupcion);
		}
	}
}

int ejecutarMOV_IN(uint32_t dir_logica){
	marco_t dir_fisica;
	uint32_t valor=0;
	dir_fisica = traducir_direccion(dir_logica);
	if(dir_fisica.marco==-1){
		return -1;
	}

	op_code cop = MOV_IN;
	send(memoria_fd, &cop, sizeof(op_code),0);
	send(memoria_fd, &dir_fisica.marco, sizeof(uint32_t),0);
	send(memoria_fd, &dir_fisica.desplazamiento, sizeof(uint16_t),0);

	recv(memoria_fd, &valor, sizeof(uint32_t), 0);
	log_info(logger, "Dato leido = %d", valor);
	return valor;
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
		uint32_t valor;
		uint32_t dir_logica= atoi(instruccion_ejecutar->parametro2);
		valor=ejecutarMOV_IN(dir_logica);
		if (valor==-1){
			return PAGEFAULT;

		}
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
	}else if(!strcmp(instruccion_ejecutar->comando,"I/O")){
		log_info(logger,"Ejecutando IO");
		return IO;
	}else if(!strcmp(instruccion_ejecutar->comando,"EXIT")){
		log_info(logger, "a dormir");
		sleep(5);
		log_info(logger,"Ejecutando EXIT");
		return EXIT;
	}else{
		log_error(logger,"Hubo un error en el ciclo de instruccion");
	}

	return CONTINUE;
}

//TLB
void inicializar_tlb(){

	for(int i=0;i< configuracion->ENTRADAS_TLB;i++){
		TLB_t *tlb_aux = crear_entrada_tlb(-1,-1,-1);
		list_add(tlb, tlb_aux);

	}

}


void limpiar_tlb(){
	list_clean_and_destroy_elements(tlb,free);
	inicializar_tlb(tlb);
}

TLB_t *crear_entrada_tlb(uint32_t segmento, uint32_t pagina, uint32_t marco){
	TLB_t *tlb_entrada= malloc(sizeof(TLB_t));

	tlb_entrada->pid=process_get_thread_id();
	tlb_entrada->marco = marco;
	tlb_entrada->pagina = pagina;
	tlb_entrada->segmento=segmento;
	tlb_entrada->ultima_referencia = clock();
	return tlb_entrada;
}

marco_t  traducir_direccion(uint32_t dir_logica){
	marco_t dire_fisica;
	uint32_t nro_marco = 145;

	uint32_t tam_max_segmento = cant_ent_por_tabla * tam_pagina;
	uint32_t num_segmento = floor(dir_logica / tam_max_segmento);
	uint32_t desplazamiento_segmento = dir_logica % tam_max_segmento;
	uint32_t num_pagina = floor(desplazamiento_segmento  / tam_pagina);
	uint32_t desplazamiento_pagina = desplazamiento_segmento % tam_pagina;
	uint16_t pid_actual=process_get_thread_id();
	//Aca hay que fijarse si esta en la tlb
	nro_marco = presente_en_tlb(num_segmento, num_pagina);

	if(nro_marco==-1){
		log_info(logger, "TLB MISS (pagina %d)", num_pagina);

		op_code cop = SOLICITUD_NRO_MARCO;
		send(memoria_fd, &cop, sizeof(op_code),0);
		send(memoria_fd, &pid_actual, sizeof(uint16_t),0);
		send(memoria_fd, &num_segmento, sizeof(int32_t),0);
		send(memoria_fd, &num_pagina, sizeof(uint32_t),0);
		recv(memoria_fd, &nro_marco, sizeof(uint32_t), 0);
		//CASO PAGE FAULT
		if(nro_marco==-1){
			dire_fisica.marco = nro_marco;
			dire_fisica.desplazamiento =desplazamiento_pagina;
			segmento=num_segmento;
			pagina=num_pagina;

			return dire_fisica;
		}
		log_info(logger, "Recibido numero de marco: %d", nro_marco);

		if(!marco_en_tlb(nro_marco,num_segmento,num_pagina)){
			if(strcmp(configuracion->REEMPLAZO_TLB,"LRU") == 0)
				reemplazo_tlb_LRU(num_segmento,num_pagina, nro_marco);
			else if(strcmp(configuracion->REEMPLAZO_TLB,"FIFO") == 0)
				reemplazo_tlb_FIFO(num_segmento,num_pagina,nro_marco);
		}
	}

	dire_fisica.marco = nro_marco;
	dire_fisica.desplazamiento =desplazamiento_pagina;

	return dire_fisica;
}

uint32_t presente_en_tlb(uint32_t numero_segmento, uint32_t numero_pagina){

	for(int i=0;i< configuracion->ENTRADAS_TLB;i++){
		TLB_t *tlb_aux = tlb_aux=list_get(tlb,i);
//		log_info(logger,"Numero pagina: %d",tlb_aux->pagina);
//		log_info(logger,"Numero marco: %d", tlb_aux->marco);
//		log_info(logger,"Ciclo cpu: %d",(int)tlb_aux->ultima_referencia);
		if(tlb_aux->pagina==numero_pagina && tlb_aux->segmento==numero_segmento){
			tlb_aux->ultima_referencia = clock();
			log_info(logger,"TLB HIT (segmento %d ,pagina %d) ", numero_segmento, numero_pagina);
			return tlb_aux->marco;
		}


	}

	return -1;
}

void reemplazo_tlb_FIFO(uint32_t numero_segmento, uint32_t numero_pagina, uint32_t marco ){
	TLB_t* tlb_entrada_0 = list_get(tlb,0);
	log_info(logger,"Segmento %d pagina %d asignada al marco %d", numero_segmento, numero_pagina, marco);
	if (tlb_entrada_0->pagina != -1 && tlb_entrada_0->segmento != -1)
		log_info(logger, "Segmento %d Pagina %d y marco %d reemplazados",tlb_entrada_0->segmento, tlb_entrada_0->pagina, tlb_entrada_0->marco);
	list_remove_and_destroy_element(tlb,0,free);
	TLB_t *tlb_entrada = crear_entrada_tlb(numero_segmento,numero_pagina,marco);
	list_add(tlb,tlb_entrada);

}

bool menor(TLB_t* a,TLB_t* b){
	return a->ultima_referencia<b->ultima_referencia;
}

void reemplazo_tlb_LRU(uint32_t numero_segmento, uint32_t numero_pagina, uint32_t marco ){
	list_sort(tlb,*menor); //Preguntar como se supone que funciona esto
	TLB_t* tlb_entrada_0 = list_get(tlb,0);
	log_info(logger,"Segmento %d pagina %d asignada al marco %d", numero_segmento, numero_pagina, marco);
	if (tlb_entrada_0->pagina != -1 && tlb_entrada_0->segmento != -1)
		log_info(logger, "Segmento %d Pagina %d y marco %d reemplazados",tlb_entrada_0->segmento, tlb_entrada_0->pagina, tlb_entrada_0->marco);
	list_remove_and_destroy_element(tlb,0,free);
	TLB_t *tlb_entrada = crear_entrada_tlb(numero_segmento, numero_pagina,marco);
	list_add(tlb,tlb_entrada);
}

bool marco_en_tlb(uint32_t marco,uint32_t segmento, uint32_t pagina){//Para encontrar la lista a actualizar
	for (int i=0;i<configuracion->ENTRADAS_TLB ;i++){
		TLB_t *tlb_aux = list_get(tlb,i);
		if(tlb_aux->marco==marco){
			list_remove(tlb,i);
			tlb_aux->pagina = pagina;
			tlb_aux->segmento=segmento;
			tlb_aux->ultima_referencia = clock();
			list_add(tlb,tlb_aux);
			return true;
			}
	}
	return false;
}

