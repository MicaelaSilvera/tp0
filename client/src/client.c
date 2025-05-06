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

	log_info(logger, "Hola! Soy un log"); // sirve para que un logger de tipo info pueda emitir un mensaje, el primer parámetro es el log que quiero y el segundo su mensaje

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	
	valor = config_get_string_value(config, "CLAVE");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// Loggeamos el valor de config
	
	log_info(logger, valor);
	log_info(logger, ip);
	log_info(logger, puerto);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	//conexion = crear_conexion(ip, puerto); ACÁ NO FUNCIONA PORQUE PRIMERO SE EJECUTA EL LEER_CONSOLA(LOGGER) DE ARRIBA Y NO SALE DEL WHILE HASTA QUE RECIBA UN ENTER VACÍO, AHÍ RECIÉN SE CONECTARÍA EL CLIENTE PERO NO ME PERMITIRÍA ESCRIBIR NADA ASI QUE DEBE SER AL REVÉS

	conexion = crear_conexion(ip, puerto); //funciona solo si es antes del leer_consola, en donde se encontraba antes (parte 3) se explica mejor

	size_t bytes;

	int32_t handshake = 1;
	int32_t result;

	t_log* log = log_create ("tp0.log", "Handshake", true, LOG_LEVEL_INFO);

	bytes = send(conexion, &handshake, sizeof(int32_t), 0);
	bytes = recv(conexion, &result, sizeof(int32_t), MSG_WAITALL);

	if (result == 0) {
		log_info(log, "Todo joya :)");
	} else {
		log_error(log, "TODO PARA EL OJETE HICISTE");
	}

	// Enviamos al servidor el valor de CLAVE como mensaje
	
	enviar_mensaje(valor, conexion);
	
	// Armamos y enviamos el paquete
	paquete(conexion);

	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	liberar_conexion(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* logger = log_create	("tp0.log", // ruta hacia el archivo donde se van a generar los logs, debe tener extensión .log. Guarda una especie de historial de los logs hechos
						"los logs", //El nombre a ser mostrado en los logs
						true, // 	Si lo que se loguea debe mostrarse por consola
						LOG_LEVEL_INFO  // El nivel de detalle máximo a loguear (ver definición de t_log_level)
						);

	return logger;
}

t_config* iniciar_config(void)
{
	t_config* config = config_create("cliente.config");

	return config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	leido = readline("te leo> "); // Muestra un prompt ">" y espera que el usuario escriba algo. Primer lectura
	
	// El resto, las vamos leyendo y logueando hasta recibir un string vacío

	while (*leido != '\0') //mientras que el primer caracter de leido no sea el caracter nulo.
	{									       
		log_info(logger, leido);   // Loguea la línea
        free(leido);               // Libera la memoria
        leido = readline("> ");    // Lee la siguiente línea
	}

	free(leido);

	// ¡No te olvides de liberar las lineas antes de regresar!
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete;

	//creo el paquete
	paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
	leido = readline("> ");

	while (*leido != '\0') //mientras que el primer caracter de leido no sea el caracter nulo.
	{									       
		agregar_a_paquete(paquete, leido, strlen(leido) + 1); //recibe el paquete creado, lo que ingrese el usuario por consola y la longitud de ese mensaje + 1 ya que no cuenta el caracter centinela.
        free(leido);               // Libera la memoria
        leido = readline("> ");    // Lee la siguiente línea
	}
	free(leido);

	// Envía el paquete	
	enviar_paquete(paquete, conexion);
	
	//Elimina el paquete
	eliminar_paquete(paquete);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	  log_destroy(logger);
	  config_destroy(config);
}
