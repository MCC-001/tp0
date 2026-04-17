#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;
	

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente X
	// Escribi: "Hola! Soy un log" X
	log_info(logger, "Hola! Soy un log");


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();
	if(config == NULL)
	{
		printf("No se pudo acceder al config!");
		abort();
	}

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor' X
	valor = config_get_string_value(config, "CLAVE");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// Loggeamos el valor de config X
	log_info(logger, "La clave es: %s", valor);


	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(valor, conexion);
	
	
	/* ---------------- LEER DE CONSOLA ---------------- */

	// Armamos y enviamos el paquete
	paquete(conexion, logger);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("tp0.log", "clienteLog", true, LOG_LEVEL_INFO);
	if(nuevo_logger == NULL)
	{
		printf("No se pudo crear el log!");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("../cliente.config");
	if (nuevo_config == NULL)
	{
		printf("No se pudo cargar el config!");
		abort();
	}
	

	return nuevo_config;
}

void leer_consola(t_log* logger, t_paquete* paquete)
{
	char* leido;
	bool vacio = false;
	// La primera te la dejo de yapa

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while(vacio==false)
	{
		leido = readline("> ");
		if(leido==NULL || strcmp(leido, "")==0)
		{
			free(leido);
			vacio = true;
		}else{
		log_info(logger,"%s", leido);
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
		}
	}
	//--Sale de la funcion
	// ¡No te olvides de liberar las lineas antes de regresar!

}

void paquete(int conexion, t_log* logger)
{
	// Ahora toca lo divertido!
	t_paquete* mi_paquete = crear_paquete();
	
	// Leemos y esta vez agregamos las lineas al paquete
	leer_consola(logger, mi_paquete);
	enviar_paquete(mi_paquete, conexion);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	eliminar_paquete(mi_paquete);
	
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	  
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
	
}
