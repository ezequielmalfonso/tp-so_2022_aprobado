/*
 * estructuras.h
 *
 *  Created on: 12 sept 2022
 *      Author: utnso
 */

/*
  PREGUNTAR POR LONGITUD DE CHAR DE la estructura de instruccion????
 */
typedef struct instruccion{
	char comando[10];
	char parametro[10];
	char parametro2[5];
} INSTRUCCION;

typedef struct pcb{
	int id_pcb;
	t_list* instrucciones;
	int program_counter;
	int registros_cpu;
	int tabla_segmentos;
	//float estimacion_rafaga;  --> NO esta en la docuemntacion pero podria servir en la palanificacion
}PCB;


