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

sem_t s_pasaje_a_ready, s_ready_execute,s_cpu_desocupado,s_cont_ready,s_multiprogramacion_actual;

t_queue* cola_new;
t_list* lista_ready;
t_list* cola_blocked;

void fifo_ready_execute(){
    sem_init(&s_ready_execute,0,0);
    sem_init(&s_cpu_desocupado, 0, 1);
	sem_init(&s_multiprogramacion_actual, 0, configuracion->GRADO_MAX_MULTIPROGRAMACION);
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
	//	pthread_mutex_unlock(&mx_log);
	//	send_proceso(conexion_cpu_dispatch, proceso, PROCESO);
	//	pcb_destroy(proceso);
	//	sem_post(&s_esperar_cpu);
	}
}
//miren la funcion esperar CPU ES BASTANTE ROBable jajaj de octocats
//si, es robable xd
//esta funcion capaz es mas en cuando esperamos respuesta de CPU, igual pero seria el planificador de largo plazo cuando finiquita un proceso esto
//si, es de cuando recibe la respuesta del cpu y ya cerro todo
void execute_a_exit(){
    //pthread_mutex_lock(/*mx_log*/);
    //log_info(logger_cambio_de_estado,"“PID: %d - Estado Anterior: EXECUTE - Estado Actual: EXIT", proceso->pid);
    //pthread_mutex_unlock(&mx_log);
    sem_post(&s_multiprogramacion_actual);//cuando se finaliza
    //liberar_espacio_de_memoria(PCB); Liberamos las estructructuras de memoria
    //pcb_destroy(PCB);
    //avisar_consola_finalizacion(); Funcion que le avisa a la consola que se finalizo correctamente
}

//Para terminar el pcb hay una de destroy en pcb.c.
void inicializarPlanificacion(){
	sem_init(&s_ready_execute,0,0);
	sem_init(&s_cpu_desocupado, 0, 1);
	sem_init(&s_multiprogramacion_actual, 0, configuracion->GRADO_MAX_MULTIPROGRAMACION);

}


/****Hilo NEW -> READY */



