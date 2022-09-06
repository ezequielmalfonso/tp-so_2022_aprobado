/*
 * protocolo.h
 *
 *  Created on: Sep 6, 2022
 *      Author: utnso
 */

#ifndef SRC_PROTOCOLO_H_
#define SRC_PROTOCOLO_H_

#include <inttypes.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <commons/collections/list.h>


typedef enum {
    DEBUG = 69,
} op_code;

bool send_debug(int fd);

#endif /* SRC_PROTOCOLO_H_ */
