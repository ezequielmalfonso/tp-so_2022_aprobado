/*
 * planificacion.c
 *
 *  Created on: Sep 29, 2022
 *      Author: utnso
 */

#include "planificacion.h"

pthread_mutex_t mx_cola_new = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mx_lista_ready = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mx_lista_block = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mx_lista_new = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mx_log = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mx_cola_blocked = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mx_cpu_desocupado = PTHREAD_MUTEX_INITIALIZER;

sem_t s_pasaje_a_ready, s_ready_execute,s_cpu_desocupado,s_cont_ready,s_multiprogramacion_actual,s_esperar_cpu,s_pcb_desalojado,s_blocked;

t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_blocked;

bool cpu_desocupado;

void fifo_ready_execute(){
	while(1){
		sem_wait(&s_ready_execute);
		sem_wait(&s_cpu_desocupado); // Para que no ejecute cada vez que un proceso llega a ready
		sem_wait(&s_cont_ready); // Para que no intente ejecutar si la lista de ready esta vacia
		pthread_mutex_lock(&mx_lista_ready);
		PCB_t* proceso = list_remove(lista_ready, 0);
		pthread_mutex_unlock(&mx_lista_ready);
		pthread_mutex_lock(&mx_log);
		//log_info(logger,(/*mx_log*/,);
		log_info(logger,"logger_cambio_de_estado","PID: %d - Estado Anterior: READY - Estado Actual: EXECUTE", proceso->pid);
		pthread_mutex_unlock(&mx_log);
		//send_proceso(conexion_cpu_dispatch, proceso); HAY QUE PROBAR ESTO
		pcb_destroy(proceso);
		sem_post(&s_esperar_cpu);
	}
}
//miren la funcion esperar CPU ES BASTANTE ROBable jajaj de octocats
//si, es robable xd
//esta funcion capaz es mas en cuando esperamos respuesta de CPU, igual pero seria el planificador de largo plazo cuando finiquita un proceso esto
//si, es de cuando recibe la respuesta del cpu y ya cerro todo
void execute_a_exit(){

    pthread_mutex_lock(/*mx_log*/);
    log_info(logger,"PID: %d - Estado Anterior: EXECUTE - Estado Actual: EXIT", proceso->pid);
    pthread_mutex_unlock(&mx_log);
    sem_post(&s_multiprogramacion_actual);//cuando se finaliza
    //liberar_espacio_de_memoria(PCB); Liberamos las estructructuras de memoria
    //pcb_destroy(PCB);
    //avisar_consola_finalizacion(); Funcion que le avisa a la consola que se finalizo correctamente
}

//Para terminar el pcb hay una de destroy en pcb.c.
void inicializarPlanificacion(){
	cola_new=queue_create();
	sem_init(&s_ready_execute,0,0);
	sem_init(&s_cpu_desocupado, 0, 1);
	sem_init(&s_esperar_cpu, 0, 1);
	sem_init(&s_multiprogramacion_actual, 0, configuracion->GRADO_MAX_MULTIPROGRAMACION);
	pthread_t corto_plazo;
	pthread_create(&corto_plazo, NULL, (void*) fifo_ready_execute, NULL);
	pthread_t espera_CPU;
	pthread_create(&espera_CPU, NULL, (void*) esperar_cpu NULL);
}


/****Hilo NEW -> READY */


void esperar_cpu(){
	while(1){
		sem_wait(&s_esperar_cpu);
		op_code cop;
		if (recv(conexion_cpu_dispatch, &cop, sizeof(op_code), 0) <= 0) {
			pthread_mutex_lock(&mx_log);
			log_error(logger,"DISCONNECT FAILURE!");
			pthread_mutex_unlock(&mx_log);
			exit(-1);
		}
		PCB_t* pcb = pcb_create();
		if (!recv_proceso(conexion_cpu_dispatch, pcb)) {
			pthread_mutex_lock(&mx_log);
			log_error(logger,"Fallo recibiendo PROGRAMA");
			pthread_mutex_unlock(&mx_log);
			return;
		}
		pthread_mutex_lock(&mx_cpu_desocupado);
		cpu_desocupado = true;
		pthread_mutex_unlock(&mx_cpu_desocupado);
		switch (cop) {
			case EXIT:{
				execute_a_exit();
				sem_post(&s_cpu_desocupado);
				sem_post(&s_ready_execute);

			/*	//Por si la interrupcion se mando cuando se estaba procesando la instruccion EXIT
				pthread_mutex_lock(&mx_hay_interrupcion);
				if(hay_interrupcion){
					//pthread_mutex_unlock(&mx_hay_interrupcion);
					sem_post(&s_pcb_desalojado);
				}
				pthread_mutex_unlock(&mx_hay_interrupcion); NOse que carajo significa pero tiene sentido*/
				break;
			}
			case INTERRUPTION:
				log_info(logger,"PID: %d - Estado Anterior: EXECUTE - Estado Actual: READY", proceso->pid);
				pthread_mutex_lock(&mx_lista_ready);
				push_queue(cola_ready, pcb);
				pthread_mutex_unlock(&mx_lista_ready);
				sem_post(&s_cont_ready);
				sem_post(&s_pcb_desalojado);
					break;

			case BLOCKED:
				pthread_mutex_lock(&mx_cola_blocked);
				push_queue(cola_blocked,pcb);
				pthread_mutex_unlock(&mx_cola_blocked);
				//ver como carajo lo bloqueamos
				//pthread_t hilo_suspendido;
				//pthread_create(&hilo_suspendido,NULL,(void*)suspendiendo,pcb);
				//pthread_detach(hilo_suspendido);
				log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: BLOCKED", proceso->pid);
				sem_post(&s_blocked);
				sem_post(&s_cpu_desocupado);
				sem_post(&s_ready_execute);

				//Por si la interrupcion se mando cuando se estaba procesando la instruccion IO
				/*pthread_mutex_lock(&mx_hay_interrupcion);
				if(hay_interrupcion){
					//pthread_mutex_unlock(&mx_hay_interrupcion);
					sem_post(&s_pcb_desalojado);
				}
				pthread_mutex_unlock(&mx_hay_interrupcion); idem que el caso de exit, tiene sentido pero ni idea*/
				break;

				case PAGEFAULT:
				//hay que implementar
					break;
			default:
				log_error(logger, "AAAlgo anduvo mal en el server del kernel\n Cop: %d",cop);
		}
	}
}
