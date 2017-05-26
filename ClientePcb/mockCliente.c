/*
 * mock.c

 *
 *  Created on: 12/4/2017
 *      Author: utnso
 */

#include "primitivas.h"
#include "src/serialize-adm.h"
#include <parser/metadata_program.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>
#define IP "127.0.0.1"
#define PUERTO "6667"
#define MAXUSERNAME 30
#define MAX_MESSAGE_SIZE 300

AnSISOP_funciones functions = {
		.AnSISOP_definirVariable =funciones_definirVariable,
		.AnSISOP_obtenerPosicionVariable =funciones_obtenerPosicionVariable,
		.AnSISOP_dereferenciar =funciones_dereferenciar,
		.AnSISOP_asignar = funciones_asignar,
		.AnSISOP_obtenerValorCompartida =funciones_obtenerValorCompartida,
		.AnSISOP_asignarValorCompartida = funciones_asignarValorCompartida,
		.AnSISOP_irAlLabel = funciones_irAlLabel,
		.AnSISOP_llamarConRetorno = funciones_llamarConRetorno,
		.AnSISOP_finalizar =funciones_finalizar,
		.AnSISOP_retornar = funciones_retornar,

};

AnSISOP_kernel kernel_functions = { .AnSISOP_wait = kernel_wait,
		.AnSISOP_signal = kernel_signal,
         .AnSISOP_reservar= kernel_reservar,
		 .AnSISOP_liberar=kernel_liberar,
		 .AnSISOP_abrir=kernel_abrir,
		 .AnSISOP_borrar=kernel_borrar,
		 .AnSISOP_cerrar=kernel_cerrar,
		 .AnSISOP_moverCursor=kernel_moverCursor,
		 .AnSISOP_escribir=kernel_escribir,
		 .AnSISOP_leer=kernel_leer,
};
void mostrarPCB(pcb* unPCB) {
	printf("\n\t\tIDENTIFICADOR:%d\n", unPCB->pid);
	printf("\t\tCANTIDAD_DE_INSTRUCCIONES: %d\n", unPCB->cant_instrucciones);
	printf("\t\tCANTIDAD_DE_NIVELES_DE_STACK: %d\n", unPCB->cantidadDeNivelesStack);
	printf("\t\tINDICE_DE_STACK: %d\n", list_size(unPCB->indiceDeStack));
	printf("\t\tPAGINA_DE_CODIGO: %d\n", unPCB->paginaDeCodigo);
	printf("\t\tPAGINA_DE_STACK: %d\n", unPCB->paginaDeStack);
	printf("\t\tPROGRAM_COUNTER:%d\n", unPCB->program_counter);
	printf("\t\tQUANTUM: %d\n", unPCB->quantum);
	printf("\t\tTAMAÑO ETIQUETAS: %d\n", unPCB->tamano_etiquetas);
	printf("\t\tEXIT CODE: %d\n", unPCB->exitCode);
}


typedef struct posicionMemoria_t {
	uint32_t pagina;
	uint32_t offset;
	uint32_t tamanio;

} posicionMemoria;

typedef struct contenido_t {
	int retPos;
	posicionMemoria *retVar;
	t_dictionary *vars;
	t_dictionary *args;
} nivelDeStack;


char* charToString(char element) {
	char* new = malloc(2);
	*new = element;
	*(new + 1) = '\0';
	return new;
}
typedef struct mensaje_CPU_KERNEL_t {
	uint32_t payloadPCB;
} mensaje_CPU_KERNEL;

t_list *generarListaConCantEnVars(pcb *pcb){
			t_list* listaAuxiliar = list_create();

				void contarVariables(void* data) {
					nivelDeStack* unNivel = (nivelDeStack*) data;
					int aux = dictionary_size(unNivel->vars);
					list_add(listaAuxiliar, (void*) aux);
				}
				list_iterate(pcb->indiceDeStack, contarVariables);
				return listaAuxiliar;

		}
		t_list *generarListaConCantEnArgs(pcb *pcb){
			t_list* listaAuxiliar = list_create();

			void contarVariables(void* data) {
			nivelDeStack* unNivel = (nivelDeStack*) data;
			int aux = dictionary_size(unNivel->args);
			list_add(listaAuxiliar, (void*) aux);
						}
			list_iterate(pcb->indiceDeStack, contarVariables);
			return listaAuxiliar;

				}
		int listSum(t_list* listaAuxiliar) {
					int contador = 0;

					void sumarNumeros(void* data) {
						int i = (int) data;
						contador += i;
					}
					list_iterate(listaAuxiliar, sumarNumeros);
					return contador;
				}
		int tamanioBufferDeUnPCB(pcb* pcbPunteroListGet, t_list* listaParaVars,t_list*listaParaArgs) {
			int tamanioBuffer = 7 * sizeof(uint32_t) + 2 * sizeof(t_size) +sizeof(int)
					+ pcbPunteroListGet->cant_instrucciones * sizeof(t_intructions)
					+ pcbPunteroListGet->tamano_etiquetas
					+ listSum(listaParaVars) * (sizeof(posicionMemoria) + 2)
					+ listSum(listaParaArgs) * (sizeof(posicionMemoria) + 2)
					+ (sizeof(posicionMemoria) + sizeof(int))
							* list_size(pcbPunteroListGet->indiceDeStack)

					+ list_size(listaParaVars) * sizeof(int)
					+ list_size(listaParaArgs) * sizeof(int);
			return tamanioBuffer;
		}

		char* serializarPcb(pcb *unPcb,t_list* listaVars,t_list*listaArgs,mensaje_CPU_KERNEL *mensajeAEnviar){
						int payloadserializado=0;
						char* buffer =malloc(7 * sizeof(uint32_t) + 2 * sizeof(t_size) +sizeof(int)
									+ unPcb->cant_instrucciones * sizeof(t_intructions)
									+ unPcb->tamano_etiquetas
									+ listSum(listaVars) * (sizeof(posicionMemoria) + 2)
									+ listSum(listaArgs) * (sizeof(posicionMemoria) + 2)
									+ (sizeof(posicionMemoria) + sizeof(int))
											* list_size(unPcb->indiceDeStack)

									+ list_size(listaVars) * sizeof(int)
									+ list_size(listaArgs) * sizeof(int));
						memcpy(buffer,mensajeAEnviar,sizeof(uint32_t));
						payloadserializado +=sizeof(uint32_t);
						memcpy(buffer + payloadserializado,unPcb,sizeof(uint32_t) * 6 + sizeof(t_size) * 2+ sizeof(int));
						payloadserializado +=sizeof(uint32_t) * 7 + sizeof(t_size) * 2+ sizeof(int);
						if(unPcb->cant_instrucciones>0){
							memcpy(buffer+payloadserializado,unPcb->instrucciones,unPcb->cant_instrucciones *sizeof(t_intructions));
							payloadserializado =+unPcb->cant_instrucciones*sizeof(t_intructions);
						}
						if (unPcb->tamano_etiquetas > 0) {
						memcpy(buffer + payloadserializado, unPcb->etiquetas,
								unPcb->tamano_etiquetas);
						payloadserializado += unPcb->tamano_etiquetas;
							}
						if (unPcb->cantidadDeNivelesStack > 0) {

								void serializarVariablesPorNivel(void* data) {
									int auxiliar = (int) data;
									memcpy(buffer + payloadserializado, &auxiliar, sizeof(int));
									payloadserializado += sizeof(int);
								}
								list_iterate(listaVars, serializarVariablesPorNivel);
								list_iterate(listaArgs, serializarVariablesPorNivel);

								void serializarIndiceDeStack(void* data) {
									nivelDeStack* unNivel = (nivelDeStack*) data;

									void serializarDiccionarioDeVariables(char* key, void* data) {
										memcpy(buffer + payloadserializado, key, 2);
										payloadserializado += 2;
										memcpy(buffer + payloadserializado, data,
												sizeof(posicionMemoria));
										payloadserializado += sizeof(posicionMemoria);
									}
									dictionary_iterator(unNivel->vars,
											serializarDiccionarioDeVariables);
									dictionary_iterator(unNivel->args,
											serializarDiccionarioDeVariables);
									memcpy(buffer + payloadserializado, &(unNivel->retPos),
											sizeof(int));
									payloadserializado += sizeof(int);
									memcpy(buffer + payloadserializado, &(unNivel->retVar),
											sizeof(posicionMemoria));
									payloadserializado += sizeof(posicionMemoria);
								}
								list_iterate(unPcb->indiceDeStack, serializarIndiceDeStack);
							}
						list_destroy(listaVars);
						list_destroy(listaArgs);
						return buffer;
					}//

		int enviarPcb(int socket,pcb *unPcb,mensaje_CPU_KERNEL *mensajeAEnviar){
			char* bufferAEnviar;

			int tamanioBuffer;
					t_list* listaConVars=generarListaConCantEnVars(unPcb);
					t_list*listaConArgs=generarListaConCantEnArgs(unPcb);
					tamanioBuffer=tamanioBufferDeUnPCB(unPcb,listaConVars,listaConArgs);
					mensajeAEnviar->payloadPCB=tamanioBuffer;
					if(tamanioBuffer>0){
					bufferAEnviar=serializarPcb(unPcb,listaConVars,listaConArgs,mensajeAEnviar);
					}
					int resultado = send(socket, bufferAEnviar, tamanioBuffer,
							0);
					return resultado;
				}




int main(){
	/*
	* Obtendo el username, que despues utilizare para "identificarme" con el server (lo envio en el paquete).
	*/
//	char *username = malloc(MAXUSERNAME);
//	get_Username(&username);
	/*
	* ¿Quien soy? ¿Donde estoy? ¿Existo?
	*
	* Estas y otras preguntas existenciales son resueltas getaddrinfo();
	*
	* Obtiene los datos de la direccion de red y lo guarda en serverInfo.
	*
	*/
	struct addrinfo hints;
	struct addrinfo *serverInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM; // Indica que usaremos el protocolo TCP
	getaddrinfo(IP, PUERTO, &hints, &serverInfo); // Carga en serverInfo los datos de la conexion
//	/*
//	* Ya se quien y a donde me tengo que conectar... ¿Y ahora?
//	* Tengo que encontrar una forma por la que conectarme al server... Ya se! Un socket!
//	*
//	* Obtiene un socket (un file descriptor -todo en linux es un archivo-), utilizando la estructura serverInfo que generamos antes.
//	*
//	*/
	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	/*
	* Perfecto, ya tengo el medio para conectarme (el archivo), y ya se lo pedi al sistema.
	* Ahora me conecto!
	*
	*/
	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo); // No lo necesitamos mas

	printf("Conectado al servidor.\n");




		FILE *archivo;
		archivo = fopen("/home/utnso/git/ansisop-parser/programas-ejemplo/completo.ansisop", "r");
		int letra, cantidadDeLetras = 0;
		while (!(feof(archivo))) {
			letra = getc(archivo);
			cantidadDeLetras++;
		}
		rewind(archivo);
		cantidadDeLetras--;
		char* mensajeAenviar = malloc(cantidadDeLetras + 1);
		char* script = malloc(cantidadDeLetras);
		fread(script, cantidadDeLetras, 1, archivo);
		memcpy(mensajeAenviar, script, cantidadDeLetras);
		mensajeAenviar[cantidadDeLetras] = '\0';
		printf("%s",mensajeAenviar);




	 //creo el pcb

	pcb* nuevoPCB=malloc(sizeof(pcb));

t_metadata_program* unProgram = metadata_desde_literal(mensajeAenviar);



	//void crearPCB(pcb* nuevoPCB, mensaje_CONSOLA_NUCLEO mensajeARecibir, int pid) {

		nuevoPCB->paginaDeCodigo = 10;
		if ((10) > 0)
			nuevoPCB->paginaDeCodigo++;
		nuevoPCB->paginaDeStack = 20;
		nuevoPCB->quantum = 4;
		nuevoPCB->pid = 0;
		nuevoPCB->program_counter = unProgram->instruccion_inicio;
		nuevoPCB->cantidadDeNivelesStack = 0;
		nuevoPCB->cant_instrucciones = unProgram->instrucciones_size;
		nuevoPCB->instrucciones = malloc(
				unProgram->instrucciones_size * sizeof(t_intructions));
		memcpy(nuevoPCB->instrucciones, unProgram->instrucciones_serializado,
				unProgram->instrucciones_size * sizeof(t_intructions));
		nuevoPCB->tamano_etiquetas = unProgram->etiquetas_size;
		nuevoPCB->etiquetas = malloc(unProgram->etiquetas_size);
		memcpy(nuevoPCB->etiquetas, unProgram->etiquetas,
				unProgram->etiquetas_size);
		nuevoPCB->exitCode=0;
		nuevoPCB->indiceDeStack = list_create();

	//}




		// agregamos 3 niveles al stack

		char a='a';

		char b='b';

		char c='c';


		posicionMemoria * unaPos1=malloc(sizeof(posicionMemoria));
		unaPos1->offset=0;
		unaPos1->pagina=0;
		unaPos1->tamanio=0;
		posicionMemoria * unaPos2=malloc(sizeof(posicionMemoria));
		unaPos2->offset=1;
		unaPos2->pagina=1;
		unaPos2->tamanio=1;
		posicionMemoria * unaPos3=malloc(sizeof(posicionMemoria));
		unaPos3->offset=2;
		unaPos3->pagina=2;
		unaPos3->tamanio=2;

		nivelDeStack* nivel1=malloc(sizeof(nivelDeStack));
		nivelDeStack* nivel2=malloc(sizeof(nivelDeStack));

		nivel1->args=dictionary_create();
		nivel1->vars=dictionary_create();
		nivel1->retPos=0;
		nivel1->retVar=malloc(sizeof(posicionMemoria));
		nivel1->retVar->offset=3;
		nivel1->retVar->pagina=3;
		nivel1->retVar->tamanio=3;
		dictionary_put(nivel1->args,charToString(a),(void*)unaPos1);
		dictionary_put(nivel1->args,charToString(c),(void*)unaPos3);
		dictionary_put(nivel1->args,charToString(b),(void*)unaPos2);
		dictionary_put(nivel1->vars,charToString(a),(void*)unaPos1);
		dictionary_put(nivel1->vars,charToString(b),(void*)unaPos2);
		dictionary_put(nivel1->vars,charToString(c),(void*)unaPos3);



		nivel2->args=dictionary_create();
		nivel2->vars=dictionary_create();
		nivel2->retPos=0;
		nivel2->retVar=malloc(sizeof(posicionMemoria));
		nivel2->retVar->offset=4;
		nivel2->retVar->pagina=4;
		nivel2->retVar->tamanio=4;
		dictionary_put(nivel2->args,charToString(b),(void*)unaPos1);
		dictionary_put(nivel2->args,charToString(c),(void*)unaPos2);
		dictionary_put(nivel2->vars,charToString(a),(void*)unaPos1);
		dictionary_put(nivel2->vars,charToString(c),(void*)unaPos2);


		list_add(nuevoPCB->indiceDeStack,nivel1);
		nuevoPCB->cantidadDeNivelesStack++;
		list_add(nuevoPCB->indiceDeStack,nivel2);
		nuevoPCB->cantidadDeNivelesStack++;


		mostrarPCB(nuevoPCB);
		mensaje_CPU_KERNEL *mensajeAEnviar=malloc(sizeof(mensaje_CPU_KERNEL));
		enviarPcb(serverSocket,nuevoPCB,mensajeAEnviar);
		free(mensajeAEnviar);




//	 char *lineaCodigo = string_new();
//	string_append(&lineaCodigo, "a = 1");
//
//	analizadorLinea(lineaCodigo, &functions,
//						&kernel_functions);
//
//	free(lineaCodigo);

	return 0;
}

