#include"utils.h"

t_log* logger;

int iniciar_servidor(void)
{
	int socket_servidor=0, ok=0, seteoSock=1;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PUERTO, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	
	//p es puntero aux para recorer la lista de sockets a probar (ver si estan disponibles)
	for(p = servinfo; p!=NULL; p = p->ai_next)
	{
		socket_servidor = socket(p->ai_family,
		p->ai_socktype,
		p->ai_protocol);
		
		if(socket_servidor == -1){continue;}

		// Asociamos el socket a un puerto
		ok = setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &seteoSock, sizeof(int));
		if(ok != 0)
		{
			perror("Error en setsockopt");
			printf("No se seteo el socket...\n");
			close(socket_servidor);
			continue;
		}
		
		ok = bind(socket_servidor, p->ai_addr, p->ai_addrlen);
		if(ok != 0)
		{
			perror("Error en bind");
			printf("No se pudo bindear el server...\n");
		}else{break;}
		
		close(socket_servidor);
		
	}
	
	if (p == NULL) {
		printf("Error: No se pudo conectar a ninguna dirección disponible\n");
		freeaddrinfo(servinfo);
		exit(-1);
	}
	// Escuchamos las conexiones entrantes
	
	ok = listen(socket_servidor, SOMAXCONN);
	if(ok != 0)
	{
		perror("Error en listen");
		printf("El server no esta escuchando...\n");
		exit(-1);
	}


	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	
	// Aceptamos un nuevo cliente
	int socket_cliente;
	socket_cliente = accept(socket_servidor, NULL, NULL);
	if(socket_cliente == -1)
	{
		printf("No se acepto al cliente...\n");
		exit(-1);
	}
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
