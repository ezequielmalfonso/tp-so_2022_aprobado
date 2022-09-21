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

//INSTRUCCIONES CONSOLA A KERNEL
void enviar_instrucciones(int socket_fd, t_list* lista){

	uint32_t size = calcular_instrucciones_buffer_size(lista);
	uint32_t sizeBuffer=size+ sizeof(int);
	void* stream = serializar_instrucciones_tam(size, lista);
	t_buffer* buffer=malloc(sizeBuffer);
	buffer->size=size;
	buffer->stream=stream;

	void* a_enviar=malloc(sizeBuffer);
	int offset=0;

	memcpy(a_enviar + offset, &(buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, buffer->stream, buffer->size);

	send(socket_fd, a_enviar,buffer->size+sizeof(uint32_t) ,0);

	free(a_enviar);
    free(buffer->stream);
    free(buffer);

}

t_instrucciones* recibir_instrucciones(int socket_fd)
{
	t_buffer* buffer=malloc(sizeof(t_buffer));

	recv(socket_fd, &(buffer->size), sizeof(int), 0);
	buffer->stream=malloc(buffer->size);
	recv(socket_fd, buffer->stream, buffer->size, 0);

	t_instrucciones* mensaje = deserializar_instrucciones(buffer);

	//free(buffer->stream);


	return mensaje;
}

void* serializar_instrucciones_tam(uint32_t size, t_list* lista) {

    INSTRUCCION* aux;

    aux = list_get(lista,0);
    uint32_t elementosLista= list_size(lista);

    uint32_t offset = 0;
    void* stream = malloc(size);

    memcpy(stream + offset, &elementosLista, sizeof(uint32_t));
    offset+= sizeof(uint32_t);

    t_link_element* aux1 = lista->head;

   while( aux1!=NULL )
	{
		INSTRUCCION* auxl2 = aux1->data;
		printf("Verificamos la lista:\n");
		printf("Comando: %s | Par1: %s | Par2: %s \n\n", auxl2->comando, auxl2->parametro, auxl2->parametro2 );

		memcpy(stream + offset, &auxl2->comando, sizeof(aux->comando));
		offset += sizeof(aux->comando);
		memcpy(stream + offset, &auxl2->parametro, sizeof(aux->parametro));
		offset += sizeof(aux->parametro);
		memcpy(stream + offset, &auxl2->parametro2, sizeof(aux->parametro2));
		offset += sizeof(aux->parametro2);
		aux1 = aux1->next;
	}
    free(aux);
    return stream;
}

t_instrucciones* deserializar_instrucciones(t_buffer* buffer){
    int i=0;
	t_instrucciones* mensaje=malloc(sizeof(t_instrucciones));


	void* stream = buffer->stream;

	memcpy(&(mensaje->elementosLista), stream, sizeof(int));
	stream += sizeof(int);

	mensaje->listaInstrucciones=list_create();
	while(i!=mensaje->elementosLista)
	{
		INSTRUCCION* aux=malloc(sizeof(INSTRUCCION));
		memcpy(&(aux->comando), stream, sizeof(aux->comando));
	    stream += sizeof(aux->comando);
	    memcpy(&(aux->parametro),stream , sizeof(aux->parametro));
	    stream += sizeof(aux->parametro);
	    memcpy(&(aux->parametro2), stream, sizeof(aux->parametro2));
	    stream += sizeof(aux->parametro2);

	    list_add(mensaje->listaInstrucciones,aux);
	    i++;
	}

	free(buffer);

	return mensaje;
}

uint32_t calcular_instrucciones_buffer_size(t_list* lista){
	uint32_t size=0;
	int i=0;
	//t_list_iterator* listaIns = list_iterator_create(lista);
	//INSTRUCCIONES* aux = list_get(lista, 0);

	while(i<=lista->elements_count){
		size += 25;
		i++;
		//aux = list_iterator_next(listaIns);
		//list_iterator_next(listaIns);
	}
	size += sizeof(uint32_t);
	//free(listaIns);
	//free(aux);
	return size;
}

