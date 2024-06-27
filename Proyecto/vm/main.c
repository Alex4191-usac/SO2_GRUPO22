#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql/mysql.h>

int main() {

    // Conecta con la base de datos
    MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	
	char* server = "db-sopes2.cjkicso8e2af.us-east-1.rds.amazonaws.com";
	char* user = "admin";
	char* password = "6j5QaYFGTZDCRQ3fMMF4";
	char* database = "BaseUsoGeneral";
	
	conn = mysql_init(NULL);	

	
	/* Conexión a la base de datos */
	if (!mysql_real_connect(conn, server, user, password, 
                                      database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
    printf("Conexion con mysql exitosa \n");

    FILE *fp;
    char buffer[1024];

    // Abrir el script de SystemTap con sudo para ejecución y lectura
    fp = popen("sudo stap memory_requests.stp", "r");
    if (fp == NULL) {
        perror("Error al abrir el script de SystemTap");
        exit(EXIT_FAILURE);
    }

    // Leer la salida del script de SystemTap y procesarla línea por línea
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Dividir la cadena utilizando la coma como delimitador
        char *token = strtok(buffer, ",");
        
        // Variable para almacenar el PID del proceso
        int pid;
        // Variable para almacenar el nombre del proceso
        char *nombreProceso;
        // Variable para almacenar la llamada
        char *llamada;
        // Variable para almacenar el tamaño del segmento de memoria
        int tamaño;
        // Variable para almacenar la fecha y hora de la solicitud
        char *fecha;

        // Asignar cada parte a las variables correspondientes
        llamada = token;
        token = strtok(NULL, ",");
        pid = atoi(token);
        token = strtok(NULL, ",");
        nombreProceso = token;
        token = strtok(NULL, ",");
        fecha = token;
        token = strtok(NULL, ",");
        tamaño = atoi(token);
        //tamaño[strcspn(tamaño, "\n")] = '\0';

        // Mostrar la información en consola 
        printf("\n***********************************************\n");
        printf("Llamada: %s\n", llamada);
        printf("PID del proceso: %d\n", pid);
        printf("Nombre del proceso: %s\n", nombreProceso);
        printf("Fecha y hora de la solicitud: %s\n", fecha);
        printf("Tamaño del segmento de memoria: %d\n", tamaño);
    

        //mysql_query(conn, "INSERT INTO Data (llamada, pid, nombre, fecha, tamano) values("mmap", 1, 'Hola', '2024', 1)")) {
        
    

        char query[2000];
        sprintf(query, "INSERT INTO BaseUsoGeneral.Data (llamada, pid, nombre, fecha, tamano) values('%s', '%d', '%s', '%s', '%d')", llamada, pid, nombreProceso, fecha, tamaño);
        //printf(query);
        mysql_query(conn, query);
    
    }

    // Cerrar el file pointer
    pclose(fp);

    return 0;
}