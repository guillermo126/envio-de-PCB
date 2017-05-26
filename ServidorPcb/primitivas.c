/*
 * primitivas.c
 *
 *  Created on: 12/4/2017
 *      Author: utnso
 */
#include "primitivas.h"

t_puntero funciones_definirVariable(t_nombre_variable variable) {

	printf( "se ejecuta la primitiva DEFINIR VARIABLE y se define la variable:%c \n", variable);

	return 1;

}

t_puntero funciones_obtenerPosicionVariable(t_nombre_variable variable) {

	//log_trace(log,"el valor es: %d \n",variableABuscar->offset);
	printf( "se ejecuta la primitiva OBTENER POSICION VARIABLE de: %c\n ",variable);
	return 1;

}

t_valor_variable funciones_dereferenciar(t_puntero puntero) {

	printf(
			"se ejecuta la primitiva DESREFERENCIAR de %d\n ",puntero);
	return 1;
}

void funciones_asignar(t_puntero puntero, t_valor_variable variable) {

	printf("se ejecuta la primitiva ASIGNAR a: %d el valor de: %d\n ", puntero, variable);

}

void funciones_imprimir(t_valor_variable valor) {

	printf( "se ejecuto la primitiva IMPRIMIR y el valor a imprimir es: %d\n", valor);
}

void funciones_imprimirTexto(char* texto) {



	printf( "se ejecuto la primitiva IMPRIMIR TEXTO y el texto es: %s \n", texto);
}

t_valor_variable funciones_obtenerValorCompartida(t_nombre_compartida variable) {

		printf(
				"se ejecuto la primitva OBTENER VALOR COMPARTIDA de: %s\n",variable);


	return 1;
}

t_valor_variable funciones_asignarValorCompartida(t_nombre_compartida variable,
		t_valor_variable valor) {

		printf(
				"se ejecuto la primitiva ASIGNAR VALOR COMPARTIDA A :%s CON VALOR:%d\n",variable,valor);
//


	return valor;
}

void funciones_irAlLabel(t_nombre_etiqueta t_nombre_etiqueta) {


	printf(
			"se ejecuta la primitiva IR AL LABEL de ETIQUETA: %s\n",t_nombre_etiqueta);
}

void funciones_llamarConRetorno(t_nombre_etiqueta etiqueta,
		t_puntero donde_retornar) {

	printf( "se ejecuta la primitiva LLAMAR CON RETORNO a:%s y retorna en: %d \n", etiqueta,
			donde_retornar);
}


//preguntar si est bien
void funciones_finalizar() {
	printf( "se ejecuto la primitiva FINALIZAR\n");
}

void funciones_retornar(t_valor_variable retorno) {

	printf("se ejecuto la primitiva RETORNAR con el valor:%d\n ", retorno);
}

void kernel_wait(t_nombre_semaforo identificador_semaforo) {
	printf("funcion wait, el nombre del semaforo es:%s\n",identificador_semaforo);
}
//
void kernel_signal(t_nombre_semaforo identificador_semaforo) {
	printf("funcion signal, el nombre del semaforo es:%s\n",identificador_semaforo);
//
}

t_puntero kernel_reservar(t_valor_variable espacio){
	printf("syscall RESERVAR y reserva:%d\n",espacio);
	return 1;
}

void kernel_liberar(t_puntero puntero){
	printf("syscall LIBERAR, libero el puntero:%d\n",puntero);
}
t_descriptor_archivo kernel_abrir(t_direccion_archivo direccion, t_banderas flags){
	printf("syscall ABRIR, LA DIRECCION ES :%s\n",direccion);
	return 1;
}

void kernel_borrar(t_descriptor_archivo direccion){
	printf("syscall BORRAR, EL ARCHIVO: %d\n",direccion);
}



void kernel_cerrar(t_descriptor_archivo descriptor_archivo){
	printf("syscall CERRAR, EL ARCHIVO: %d\n",descriptor_archivo);
}


void kernel_moverCursor(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion){
	printf("syscall MOVER CURSOR, DEL ARCHIVO: %d A: %d \n",descriptor_archivo,posicion);
}


void kernel_escribir(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio){
	printf("syscall ESCRIBIR AL ARCHIVO: %d con un tamaño de: %d\n",descriptor_archivo,tamanio);

}

void kernel_leer(t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio){
	printf("syscall LEER ARCHIVO:%d, con informacion en:%d y tamanio:%d\n",descriptor_archivo,informacion,tamanio);
}

int devuelvePagina(int dividendo, int divisor) {
	return (int) (dividendo / divisor);

}
int devuelveOffset(int dividendo, int divisor) {
	return dividendo % divisor;
}

