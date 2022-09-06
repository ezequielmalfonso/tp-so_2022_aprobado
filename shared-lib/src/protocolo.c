/*
 * protocolo.c
 *
 *  Created on: Sep 6, 2022
 *      Author: utnso
 */


#include "protocolo.h"

// DEBUG
bool send_debug(int fd) {
    op_code cop = DEBUG;
    if (send(fd, &cop, sizeof(op_code), 0) != sizeof(op_code)){
        return false;
    }
    return true;
}
