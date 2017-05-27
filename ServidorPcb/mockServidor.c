
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
#define PUERTO "6667"
#define BACKLOG 5	// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define MAX_PACKAGE_SIZE 1024	//El servidor no admitira paquetes de mas de 1024 bytes
#define MAXUSERNAME 30
#define MAX_MESSAGE_SIZE 300

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

void desSerializarPcb(kernel_pcb* PCBDESSERIALIZADO, char* buffer) {
	int payloadDesserializado = 0;
	memcpy(&*PCBDESSERIALIZADO, buffer + payloadDesserializado,
			sizeof(uint32_t) * 6 + sizeof(t_size) * 2 + sizeof(int));
	payloadDesserializado += sizeof(uint32_t) * 6 + sizeof(t_size) * 2 + sizeof(int);
	if (PCBDESSERIALIZADO->cant_instrucciones > 0) {
		PCBDESSERIALIZADO->instrucciones = malloc(PCBDESSERIALIZADO->cant_instrucciones * sizeof(t_intructions));
		memcpy(PCBDESSERIALIZADO->instrucciones, buffer + payloadDesserializado,
				PCBDESSERIALIZADO->cant_instrucciones * sizeof(t_intructions));
		payloadDesserializado += PCBDESSERIALIZADO->cant_instrucciones * sizeof(t_intructions);
	}
	if (PCBDESSERIALIZADO->tamano_etiquetas > 0) {
		PCBDESSERIALIZADO->etiquetas = malloc(PCBDESSERIALIZADO->tamano_etiquetas);
		memcpy(PCBDESSERIALIZADO->etiquetas, buffer + payloadDesserializado, PCBDESSERIALIZADO->tamano_etiquetas);
		payloadDesserializado += PCBDESSERIALIZADO->tamano_etiquetas;
	}
	PCBDESSERIALIZADO->indiceDeStack = list_create();
	if (PCBDESSERIALIZADO->cantidadDeNivelesStack > 0) {
		int i;
		//aca tendria q ir para args y vars
		t_list* listAuxVars = list_create();
		t_list* listAuxArgs = list_create();

		//prmero agrego en listaVars
		for (i = 0; i < PCBDESSERIALIZADO->cantidadDeNivelesStack; i++) {
			int auxiliar;
			memcpy(&auxiliar, buffer + payloadDesserializado, sizeof(int));
			payloadDesserializado += sizeof(int);
			list_add(listAuxVars, (void*) auxiliar);
		}
		//despues agrego en listaAgs
		for (i = 0; i < PCBDESSERIALIZADO->cantidadDeNivelesStack; i++) {
			int auxiliar2;
			memcpy(&auxiliar2, buffer + payloadDesserializado, sizeof(int));
			payloadDesserializado += sizeof(int);
			list_add(listAuxArgs, (void*) auxiliar2);
		}
		void desserializarIndiceDeStackConVars(void* data) {
			int auxiliar = (int) data;
			nivelDeStack* unNivelNuevo = malloc(sizeof(nivelDeStack));
			//esto es para vars
			unNivelNuevo->vars = dictionary_create();
			for (i = 0; i < auxiliar; i++) {
				char* auxiliarkey = malloc(2);
				memcpy(auxiliarkey, buffer + payloadDesserializado, 2);
				payloadDesserializado += 2;
				posicionMemoria* unaPos = malloc(sizeof(posicionMemoria));
				memcpy(unaPos, buffer + payloadDesserializado, sizeof(posicionMemoria));
				payloadDesserializado += sizeof(posicionMemoria);
				dictionary_put(unNivelNuevo->vars, auxiliarkey, (void*) unaPos);
			}

			list_add(PCBDESSERIALIZADO->indiceDeStack, (void*) unNivelNuevo);
		}
		int j = 0;
		void desserializarIndiceDeStackConArgs(void*data) {
			int auxiliar = (int) data;
			nivelDeStack* nivelStack = malloc(sizeof(nivelDeStack));
			nivelStack = list_get(PCBDESSERIALIZADO->indiceDeStack, j);
			//esto para args
			nivelStack->args = dictionary_create();
			for (i = 0; i < auxiliar; i++) {
				char* auxiliarkey = malloc(2);
				memcpy(auxiliarkey, buffer + payloadDesserializado, 2);
				payloadDesserializado += 2;
				posicionMemoria* unaPos = malloc(sizeof(posicionMemoria));
				memcpy(unaPos, buffer + payloadDesserializado, sizeof(posicionMemoria));
				payloadDesserializado += sizeof(posicionMemoria);
				dictionary_put(nivelStack->args, auxiliarkey, (void*) unaPos);
			}
			memcpy(&(nivelStack->retPos), buffer + payloadDesserializado, sizeof(int));
			payloadDesserializado += sizeof(int);
			memcpy(&(nivelStack->retVar), buffer + payloadDesserializado, sizeof(posicionMemoria));
			payloadDesserializado += sizeof(posicionMemoria);
			j++;
		}
		list_iterate(listAuxVars, desserializarIndiceDeStackConVars);
		list_iterate(listAuxArgs, desserializarIndiceDeStackConArgs);
		list_destroy(listAuxVars);
		list_destroy(listAuxArgs);
	}
}



	int recibirPcb(int socket,kernel_pcb* unPcb,mensaje_CPU_KERNEL *mensajeARecibir){
		char* buffer = malloc(sizeof(uint32_t));
			int payloadDesserializacion = 0;
			int resultado = recv(socket, buffer,sizeof(uint32_t), 0);
			if(resultado>0){
				memcpy(mensajeARecibir, buffer, sizeof(uint32_t));
				payloadDesserializacion = sizeof(uint32_t);
			}
			if(mensajeARecibir->payloadPCB>0){
				free(buffer);
				buffer = malloc(mensajeARecibir->payloadPCB);
				resultado= recv(socket, buffer, mensajeARecibir->payloadPCB, 0);
				desSerializarPcb(unPcb, buffer);
			}
			free(buffer);

		return resultado;
	}

	void mostrarPCB(kernel_pcb* unPCB) {
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

int main(){
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
	hints.ai_family = AF_UNSPEC; // No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE; // Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM; // Indica que usaremos el protocolo TCP
	getaddrinfo(NULL, PUERTO, &hints, &serverInfo); // Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE
	/*
	* Descubiertos los misterios de la vida (por lo menos, para la conexion de red actual), necesito enterarme de alguna forma
	* cuales son las conexiones que quieren establecer conmigo.
	*
	* Para ello, y basandome en el postulado de que en Linux TODO es un archivo, voy a utilizar... Si, un archivo!
	*
	* Mediante socket(), obtengo el File Descriptor que me proporciona el sistema (un integer identificador).
	*
	*/
	/* Necesitamos un socket que escuche las conecciones entrantes */
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	/*
	* Perfecto, ya tengo un archivo que puedo utilizar para analizar las conexiones entrantes. Pero... ¿Por donde?
	*
	* Necesito decirle al sistema que voy a utilizar el archivo que me proporciono para escuchar las conexiones por un puerto especifico.
	*
	* OJO! Todavia no estoy escuchando las conexiones entrantes!
	*
	*/
	int yes =1;
		setsockopt(listenningSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar
	/*
	* Ya tengo un medio de comunicacion (el socket) y le dije por que "telefono" tiene que esperar las llamadas.
	*
	* Solo me queda decirle que vaya y escuche!
	*
	*/
	listen(listenningSocket, BACKLOG); // IMPORTANTE: listen() es una syscall BLOQUEANTE.
	/*
	* El sistema esperara hasta que reciba una conexion entrante...
	* ...
	* ...
	* BING!!! Nos estan llamando! ¿Y ahora?
	*
	* Aceptamos la conexion entrante, y creamos un nuevo socket mediante el cual nos podamos comunicar (que no es mas que un archivo).
	*
	* ¿Por que crear un nuevo socket? Porque el anterior lo necesitamos para escuchar las conexiones entrantes. De la misma forma que
	* uno no puede estar hablando por telefono a la vez que esta esperando que lo llamen, un socket no se puede encargar de escuchar
	* las conexiones entrantes y ademas comunicarse con un cliente.
	*
	* Nota: Para que el listenningSocket vuelva a esperar conexiones, necesitariamos volver a decirle que escuche, con listen();
	* En este ejemplo nos dedicamos unicamente a trabajar con el cliente y no escuchamos mas conexiones.
	*
	*/
	struct sockaddr_in addr; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);
	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);


	kernel_pcb* unPcb=malloc(sizeof(kernel_pcb));
	mensaje_CPU_KERNEL *mensajeARecibir=malloc(sizeof(mensaje_CPU_KERNEL));
	recibirPcb(socketCliente,unPcb,mensajeARecibir);

	mostrarPCB(unPcb);

	nivelDeStack *nivel1= malloc(sizeof(nivelDeStack));
	nivelDeStack *nivel2=malloc(sizeof(nivelDeStack));

	nivel1=list_get(unPcb->indiceDeStack,0);
	nivel2=list_get(unPcb->indiceDeStack,1);

	//paraverificar en vars
	char a='a';
	posicionMemoria *unPos=malloc(sizeof(posicionMemoria));
	unPos=dictionary_get(nivel1->vars,charToString(a));
	printf("la posicion de A en el nivel 1 de Vars es:%d,%d,%d",unPos->pagina,unPos->offset,unPos->tamanio);

	//para verificar en args
	char c='c';
	posicionMemoria *unPosParaArgs=malloc(sizeof(posicionMemoria));
		unPosParaArgs=dictionary_get(nivel1->args,charToString(c));
		printf("la posicion de C en el nivel 1 de Args es:%d,%d,%d",unPosParaArgs->pagina,unPosParaArgs->offset,unPosParaArgs->tamanio);

//verificar el ret var del nivel 1
		posicionMemoria *unPosRetPvar=malloc(sizeof(posicionMemoria));
		unPosRetPvar=nivel1->retVar;
		printf("la posicion de retVar en el nivel 1 de Args es:%d,%d,%d",unPosRetPvar->pagina,unPosRetPvar->offset,unPosRetPvar->tamanio);

//verificar en retPos

		printf("el retPos del nivel 1 es:%d",nivel1->retPos);

		///ahora para el nivel 2 del stack

		//paraverificar en vars

			posicionMemoria *unPos1=malloc(sizeof(posicionMemoria));
			unPos1=dictionary_get(nivel2->vars,charToString(a));
			printf("la posicion de A en el nivel 2 de Vars es:%d,%d,%d",unPos1->pagina,unPos1->offset,unPos1->tamanio);

			//para verificar en args

			posicionMemoria *unPosParaArgs1=malloc(sizeof(posicionMemoria));
				unPosParaArgs1=dictionary_get(nivel2->args,charToString(c));
				printf("la posicion de C en el nivel 2 de Args es:%d,%d,%d",unPosParaArgs1->pagina,unPosParaArgs1->offset,unPosParaArgs1->tamanio);

		//verificar el ret var del nivel 1
				posicionMemoria *unPosRetPvar1=malloc(sizeof(posicionMemoria));
				unPosRetPvar1=nivel2->retVar;
				printf("la posicion de retVar en el nivel 2 de Args es:%d,%d,%d",unPosRetPvar1->pagina,unPosRetPvar1->offset,unPosRetPvar1->tamanio);

		//verificar en retPos

				printf("el retPos del nivel 2 es:%d",nivel2->retPos);



	return 0;
}


