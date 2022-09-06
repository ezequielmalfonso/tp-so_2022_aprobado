/*
 * consolaConfig.h
 *
 *  Created on: Sep 6, 2022
 *      Author: utnso
 */

#ifndef SRC_CONSOLACONFIG_H_
#define SRC_CONSOLACONFIG_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/collections/list.h"

typedef struct {
	char* IP_KERNEL;
	uint32_t PUERTO_KERNEL;
	t_list* SEGMENTOS;
} t_config_consola;

t_config_consola * configuracion;
t_config * fd_configuracion;
t_log * logger;

int cargarConfiguracion();

#endif /* SRC_CONSOLACONFIG_H_ */
