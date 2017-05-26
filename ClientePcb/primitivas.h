/*
 * primitivas.h
 *
 *  Created on: 12/4/2017
 *      Author: utnso
 */

#ifndef PROYECTOMOCKPARSER_PRIMITIVAS_H_
#define PROYECTOMOCKPARSER_PRIMITIVAS_H_
#include <parser/metadata_program.h>
#include <stdio.h>

t_puntero funciones_definirVariable(t_nombre_variable variable);

t_puntero funciones_obtenerPosicionVariable(t_nombre_variable variable);
t_valor_variable funciones_dereferenciar(t_puntero puntero);
void funciones_asignar(t_puntero puntero, t_valor_variable variable);
//no las tiene
void funciones_imprimir(t_valor_variable valor);
void funciones_imprimirTexto(char* texto);
///

t_valor_variable funciones_obtenerValorCompartida(t_nombre_compartida variable);
t_valor_variable funciones_asignarValorCompartida(t_nombre_compartida variable,
		t_valor_variable valor);
void funciones_irAlLabel(t_nombre_etiqueta t_nombre_etiqueta);
void funciones_llamarConRetorno(t_nombre_etiqueta etiqueta,
		t_puntero donde_retornar);
void funciones_finalizar(void);
void funciones_retornar(t_valor_variable retorno);


void kernel_wait(t_nombre_semaforo variable);
void kernel_signal(t_nombre_semaforo variable);
t_puntero kernel_reservar(t_valor_variable espacio);

void kernel_liberar(t_puntero puntero);
t_descriptor_archivo kernel_abrir(t_direccion_archivo direccion, t_banderas flags);

void kernel_borrar(t_descriptor_archivo direccion);



void kernel_cerrar(t_descriptor_archivo descriptor_archivo);


void kernel_moverCursor(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion);


void kernel_escribir(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio);

void kernel_leer(t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio);

#endif /* PROYECTOMOCKPARSER_PRIMITIVAS_H_ */
