#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <semaphore.h>
#include "cJSON.h"
#include "cJSON.c"

// Estructura para almacenar la información de un usuario
typedef struct {
    int no_cuenta;
    char nombre[100];
    double saldo;
} Usuario;

// Estructura para almacenar la informacion de las operaciones
typedef struct {
    int operacion;
    int cuenta1;
    int cuenta2;
    double monto;
} Operacion;

typedef struct {
    Operacion* operaciones;
    int start;
    int end;
    int thread_id;
    int *cant_retiros;
    int *cant_depositos;
    int *cant_transferencias;
    char *errores;
    FILE *report_file;
    pthread_mutex_t *error_mutex; // Mutex para controlar acceso a errores
} OperacionThreadData;

// Estructura para pasar datos a los hilos
typedef struct {
    Usuario *usuarios;
    int start;
    int end;
    int thread_id;
    int *num_users_loaded;
    FILE *report_file; // Para escribir directamente en el reporte
    bool *cuentas_existentes; // Array para verificar cuentas existentes
    char *errores; // Cadena de caracteres para almacenar errores
    pthread_mutex_t *error_mutex; // Mutex para controlar acceso a errores
} ThreadData;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t usuarios_mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t semaforo;

int num_usuarios = 0;
Usuario *usuarios = NULL;

// Función para leer el contenido de un archivo
char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(length + 1);
    if (data) {
        fread(data, 1, length, file);
        data[length] = '\0';
    }
    fclose(file);

    return data;
}

// Función para parsear el JSON y extraer los usuarios
cJSON *parse_json(const char *json_string) {
    cJSON *json = cJSON_Parse(json_string);
    if (!json) {
        fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }
    return json;
}

// Función para agregar un usuario a la estructura compartida
void agregar_usuario(Usuario usuario) {
    pthread_mutex_lock(&usuarios_mutex);
    usuarios = (Usuario *)realloc(usuarios, (num_usuarios + 1) * sizeof(Usuario));
    usuarios[num_usuarios++] = usuario;
    pthread_mutex_unlock(&usuarios_mutex);
}

// Función que ejecutarán los hilos
void *cargar_usuarios(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int users_loaded = 0;
    char thread_error[200]; // Buffer para almacenar errores del hilo

    for (int i = data->start; i < data->end; ++i) {
        Usuario usuario = data->usuarios[i];
        // Validaciones
        bool error = false;
        pthread_mutex_lock(&lock);
        if (usuario.no_cuenta <= 0) {
            sprintf(thread_error, "Error en hilo %d: Número de cuenta no es un entero positivo (línea %d)\n", data->thread_id, i + 1);
            error = true;
        } else if (data->cuentas_existentes[usuario.no_cuenta]) {
            sprintf(thread_error, "Error en hilo %d: Número de cuenta %d ya existe (línea %d)\n", data->thread_id, usuario.no_cuenta, i + 1);
            error = true;
        } else {
            data->cuentas_existentes[usuario.no_cuenta] = true;
        }
        pthread_mutex_unlock(&lock);

        if (usuario.saldo < 0) {
            pthread_mutex_lock(&lock);
            sprintf(thread_error, "Error en hilo %d: Saldo no es un número real positivo (línea %d)\n", data->thread_id, i + 1);
            pthread_mutex_unlock(&lock);
            error = true;
        }

        if (error) {
            pthread_mutex_lock(data->error_mutex);
            strcat(data->errores, thread_error);
            pthread_mutex_unlock(data->error_mutex);
            continue;
        }

        // Agregar usuario válido a la estructura compartida
        agregar_usuario(usuario);
        users_loaded++;
    }

    pthread_mutex_lock(&lock);
    *(data->num_users_loaded) += users_loaded;
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

// Función para buscar un usuario por número de cuenta
Usuario* buscar_usuario(int no_cuenta) {
    for (int i = 0; i < num_usuarios; ++i) {
        if (usuarios[i].no_cuenta == no_cuenta) {
            return &usuarios[i];
        }
    }
    return NULL;
}

//Funcion para realizar un Deposito
void deposito() {
    int no_cuenta;
    double monto;
    printf("Ingrese el número de cuenta: ");
    scanf("%d", &no_cuenta);

    Usuario *usuario = buscar_usuario(no_cuenta);
    if (usuario == NULL) {
        printf("Error: El número de cuenta no existe.\n");
        return;
    }

    printf("/n");
    printf("Ingrese el monto a depositar: ");
    scanf("%lf", &monto);
    //validar monto
    if (monto <= 0) {
        printf("Error: El monto debe ser un número positivo.\n");
        return;
    }


    pthread_mutex_lock(&lock);
    usuario->saldo += monto;
    pthread_mutex_unlock(&lock);
    printf("Depósito realizado con éxito.\n");
}

//Funcion para realizar un Retiro
void retiro() {
    int no_cuenta;
    double monto;
    printf("Ingrese el número de cuenta: ");
    scanf("%d", &no_cuenta);

    Usuario *usuario = buscar_usuario(no_cuenta);
    if (usuario == NULL) {
        printf("Error: El número de cuenta no existe.\n");
        return;
    }

    printf("/n");
    printf("Ingrese el monto a retirar: ");
    scanf("%lf", &monto);
    //validar monto
    if (monto <= 0) {
        printf("Error: El monto debe ser un número positivo.\n");
        return;
    }

    if (usuario->saldo < monto) {
        printf("Error: Saldo insuficiente.\n");
        return;
    }

    pthread_mutex_lock(&lock);
    usuario->saldo -= monto;
    pthread_mutex_unlock(&lock);
    printf("Retiro realizado con éxito.\n");
}

// Función para consultar una cuenta
void consultar_cuenta() {
    int no_cuenta;
    printf("Ingrese el número de cuenta: ");
    scanf("%d", &no_cuenta);

    Usuario *usuario = buscar_usuario(no_cuenta);
    if (usuario == NULL) {
        printf("Error: El número de cuenta no existe.\n");
        return;
    }
    printf(" \n");
    printf("Información de la cuenta:\n");
    printf("Número de cuenta: %d\n", usuario->no_cuenta);
    printf("Nombre: %s\n", usuario->nombre);
    printf("Saldo: %.2f\n", usuario->saldo);
}

//Funcion para realizar una transaccion
void transaccion(){
    int no_cuenta;
    printf("Ingrese el número de cuenta: ");
    scanf("%d", &no_cuenta);

    //Buscar cuenta
    Usuario *usuario = buscar_usuario(no_cuenta);
    if (usuario == NULL) {
        printf("Error: El número de cuenta no existe.\n");
        return;
    }

    //Cuenta destino
    int no_cuenta_destino;
    printf("Ingrese el número de cuenta destino: ");
    scanf("%d", &no_cuenta_destino);

    //Buscar cuenta destino
    Usuario *usuario_destino = buscar_usuario(no_cuenta_destino);
    if (usuario_destino == NULL) {
        printf("Error: El número de cuenta destino no existe.\n");
        return;
    }

    //Monto a transferir
    double monto;
    printf("Ingrese el monto a transferir: ");
    scanf("%lf", &monto);
    //validar monto
    if (monto <= 0) {
        printf("Error: El monto debe ser un número positivo.\n");
        return;
    }

    //Validar saldo
    if (usuario->saldo < monto) {
        printf("Error: Saldo insuficiente.\n");
        return;
    }

    //Realizar transaccion
    pthread_mutex_lock(&lock);
    usuario->saldo -= monto;
    usuario_destino->saldo += monto;
    pthread_mutex_unlock(&lock);
    printf("Transacción realizada con éxito.\n");

}

void operaciones_individuales() {
    int opcion;
    while (1) {
        printf("\nOperaciones Individuales:\n");
        printf("1. Depósito\n");
        printf("2. Retiro\n");
        printf("3. Transacción\n");
        printf("4. Consultar cuenta\n");
        printf("5. Volver al menú principal\n");
        printf("Ingrese el número de opción deseada: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                deposito();
                break;
            case 2:
                retiro();
                break;
            case 3:
                transaccion();
                break;
            case 4:
                consultar_cuenta();
                break;
            case 5:
                return;
            default:
                printf("Opción no válida. Inténtelo de nuevo.\n");
                break;
        }
    }
}

//Funcion para realizar un Deposito
int masivo_deposito(int no_cuenta, double monto) {


    Usuario *usuario = buscar_usuario(no_cuenta);
    if (usuario == NULL) {
        return -1;
    }

    //validar monto
    if (monto <= 0) {
        return -2;
    }

    sem_wait(&semaforo);
    usuario->saldo += monto;
    sem_post(&semaforo);

    return 0;
}

//Funcion para realizar un Retiro
int  masivo_retiro(int no_cuenta, double monto) {


    Usuario *usuario = buscar_usuario(no_cuenta);
    if (usuario == NULL) {
        return -1;
    }

    //validar monto
    if (monto <= 0) {
        return -2;
    }

    if (usuario->saldo < monto) {
        return -3;
    }

    sem_wait(&semaforo);
    usuario->saldo -= monto;
    sem_post(&semaforo);

    return 0;
}

//Funcion para realizar una transaccion
int masivo_transaccion(int cuenta1, int cuenta2, double monto){


    //Buscar cuenta
    Usuario *usuario = buscar_usuario(cuenta1);
    if (usuario == NULL) {
        //printf("Error: El número de cuenta no existe.\n");
        return -1;
    }

    //Buscar cuenta destino
    Usuario *usuario_destino = buscar_usuario(cuenta2);
    if (usuario_destino == NULL) {
        return -2;
    }

    //validar monto
    if (monto <= 0) {
        return -3;
    }

    //Validar saldo
    if (usuario->saldo < monto) {
        return -4;
    }

    //Realizar transaccion
    sem_wait(&semaforo);
    usuario->saldo -= monto;
    usuario_destino->saldo += monto;
    sem_post(&semaforo);
    return 0;
}

void *ejecutar_operaciones(void *arg)
{
    OperacionThreadData *datos = (OperacionThreadData *)arg;
    int cant_depositos = 0;
    int cant_retiros = 0;
    int cant_transferencias = 0;
    char thread_error[200];

    for (int i = datos->start; i < datos->end; i++)
    {
        Operacion op = datos->operaciones[i];
        bool error = false;
        switch (op.operacion)
        {
        case 1:
            // Deposito
            int result = masivo_deposito(op.cuenta1, op.monto);
            if (result == -1)
            {
                sprintf(thread_error, "Error en hilo %d: El número de cuenta no existe.(línea %d)\n", datos->thread_id, i + 1);
                error = true;
            }
            else if (result == -2)
            {
                sprintf(thread_error, "Error en hilo %d: El monto debe ser un número positivo.(línea %d)\n", datos->thread_id, i + 1);
                error = true;
            }
            {
                cant_depositos++;
            }
            break;
        case 2:
            // Retiro
            masivo_retiro(op.cuenta1, op.monto);

            if (result == -1)
            {
                sprintf(thread_error, "Error en hilo %d: El número de cuenta no existe.(línea %d)\n", datos->thread_id, i + 1);
                error = true;
            }
            else if (result == -2)
            {
                sprintf(thread_error, "Error en hilo %d: El monto debe ser un número positivo.(línea %d)\n", datos->thread_id, i + 1);
                error = true;
            }
            else if (result == -3)
            {
                sprintf(thread_error, "Error en hilo %d:  Saldo insuficiente.(línea %d)\n", datos->thread_id, i + 1);
                error = true;
            }
            {
            cant_retiros++;
            }
            break;
        case 3:
            // Transferencia
            masivo_transaccion(op.cuenta1, op.cuenta2, op.monto);
            if (result == -1)
            {
                sprintf(thread_error, "Error en hilo %d: El número de cuenta origen no existe.(línea %d)\n", datos->thread_id, i + 1);
                error = true;
            }
            else if (result == -2)
            {
                sprintf(thread_error, "Error en hilo %d: El número de cuenta destino no existe.línea %d)\n", datos->thread_id, i + 1);
                error = true;
            }
            else if (result == -3)
            {
                sprintf(thread_error, "Error en hilo %d: El monto debe ser un número positivo.(línea %d)\n", datos->thread_id, i + 1);
                error = true;
            }
            else if (result == -4)
            {
                sprintf(thread_error, "Error en hilo %d:  Saldo insuficiente.(línea %d)\n", datos->thread_id, i + 1);
                error = true;
            }
            {
            cant_transferencias++;
            }

            break;
        default:
            sprintf(thread_error, "Error en hilo %d:  La operacion indicada no existe. (línea %d)\n", datos->thread_id, i + 1);
            error = true;
            break;
        }

        if (error) {
            pthread_mutex_lock(datos->error_mutex);
            strcat(datos->errores, thread_error);
            pthread_mutex_unlock(datos->error_mutex);
            continue;
        }
    }

    pthread_mutex_lock(&lock);
    *(datos->cant_depositos) += cant_depositos;
    *(datos->cant_retiros) += cant_retiros;
    *(datos->cant_transferencias) += cant_transferencias;
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

void carga_masiva_operaciones()
{
    printf("Carga masiva de operaciones\n");

    const char *filename = "operaciones.json";
    char *json_string = read_file(filename);

    if (json_string)
    {
        cJSON *json = parse_json(json_string);
        if (json)
        {
            int total_operaciones = cJSON_GetArraySize(json);
            Operacion *operaciones_temporales = (Operacion *)malloc(total_operaciones * sizeof(Operacion));

            // Extraer las operaciones del JSON
            for (int i = 0; i < total_operaciones; i++)
            {
                cJSON *item = cJSON_GetArrayItem(json, i);
                operaciones_temporales[i].operacion = cJSON_GetObjectItem(item, "operacion")->valueint;
                operaciones_temporales[i].cuenta1 = cJSON_GetObjectItem(item, "cuenta1")->valueint;
                operaciones_temporales[i].cuenta2 = cJSON_GetObjectItem(item, "cuenta2")->valueint;
                operaciones_temporales[i].monto = cJSON_GetObjectItem(item, "monto")->valuedouble;
            }

            sem_init(&semaforo, 0, 1);

            int num_hilos = 4;
            pthread_t hilos[num_hilos];
            OperacionThreadData datos_hilo[num_hilos];

            int cant_depositos[4] = {0,0,0,0};
            int cant_retiros[4] = {0,0,0,0};
            int cant_transferencias[4] = {0,0,0,0};

                        // Inicializar estructuras para manejo de errores
            pthread_mutex_t error_mutex = PTHREAD_MUTEX_INITIALIZER;
            char *errores = (char *)malloc(1000 * sizeof(char)); // Tamaño inicial arbitrario
            errores[0] = '\0'; // Inicializar cadena vacía

            int operaciones_por_hilo = total_operaciones / num_hilos;

            for (int i = 0; i < num_hilos; i++)
            {
                datos_hilo[i].operaciones = operaciones_temporales;
                datos_hilo[i].start = i * operaciones_por_hilo;
                if (i == num_hilos - 1)
                {
                    datos_hilo[i].end = total_operaciones;
                }
                else
                {
                    datos_hilo[i].end = (i + 1) * operaciones_por_hilo;
                }
                datos_hilo[i].cant_depositos = &cant_depositos[i];
                datos_hilo[i].cant_retiros = &cant_retiros[i];
                datos_hilo[i].thread_id = i + 1;
                datos_hilo[i].cant_transferencias = &cant_transferencias[i];
                datos_hilo[i].errores = errores;
                datos_hilo[i].error_mutex = &error_mutex;

                pthread_create(&hilos[i], NULL, ejecutar_operaciones, &datos_hilo[i]);
            }

            // Esperar a que los hilos terminen
            for (int i = 0; i < num_hilos; ++i)
            {
                pthread_join(hilos[i], NULL);
            }

            // Generacion de reporte
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char report_filename[100];
            char time_buffer[20];

            int operaciones_hilo_1 = cant_depositos[0] + cant_retiros[0] + cant_transferencias[0];
            int operaciones_hilo_2 = cant_depositos[1] + cant_retiros[1] + cant_transferencias[1];
            int operaciones_hilo_3 = cant_depositos[2] + cant_retiros[2] + cant_transferencias[2];
            int operaciones_hilo_4 = cant_depositos[3] + cant_retiros[3] + cant_transferencias[3];

            int total_depositos = cant_depositos[0] + cant_depositos[1] + cant_depositos[2]+ cant_depositos[3];
            int total_retiros = cant_retiros[0] + cant_retiros[1] + cant_retiros[2] + cant_retiros[3];
            int total_transferencias = cant_transferencias[0] + cant_transferencias[1] + cant_transferencias[2]+ cant_transferencias[3];


            strftime(report_filename, sizeof(report_filename) - 1, "operaciones_%Y_%m_%d-%H_%M_%S.log", t);

            FILE *report_file = fopen(report_filename, "w");
            if (!report_file) {
                perror("Could not open report file");
                return;
            }

            fprintf(report_file, "---------- Resumen de operaciones ----------\n\n");

            strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", t);
            fprintf(report_file, "Fecha: %s\n\n", time_buffer);

            fprintf(report_file, "Operaciones realizadas:\n");
            fprintf(report_file, "Retiros: %d\n", total_retiros);
            fprintf(report_file, "Depositos: %d\n", total_depositos);
            fprintf(report_file, "Transferencias: %d\n", total_transferencias);
            fprintf(report_file, "Total: %d\n\n", total_depositos+ total_retiros + total_transferencias);

            fprintf(report_file, "Operaciones por hilo:\n");
            fprintf(report_file, "Hilo #1: %d\n", operaciones_hilo_1);
            fprintf(report_file, "Hilo #2: %d\n", operaciones_hilo_2);
            fprintf(report_file, "Hilo #3: %d\n", operaciones_hilo_3);
            fprintf(report_file, "Hilo #4: %d\n", operaciones_hilo_4);
            fprintf(report_file, "Total: %d\n\n", operaciones_hilo_1 + operaciones_hilo_2 + operaciones_hilo_3 + operaciones_hilo_4);

            fprintf(report_file, "Errores encontrados:\n%s\n", errores);

            fclose(report_file);

            free(operaciones_temporales);
            free(errores);
            sem_destroy(&semaforo);
            cJSON_Delete(json);
        }
    }
}

void estado_cuenta() {
    /*printf("Estado de Cuentas:\n");
    for (int i = 0; i < num_usuarios; ++i) {
        printf("Número de cuenta: %d, Nombre: %s, Saldo: %.2f\n", usuarios[i].no_cuenta, usuarios[i].nombre, usuarios[i].saldo);
    }*/

    cJSON *json = cJSON_CreateArray();
    for (int i = 0; i < num_usuarios; ++i) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddNumberToObject(item, "no_cuenta", usuarios[i].no_cuenta);
        cJSON_AddStringToObject(item, "nombre", usuarios[i].nombre);
        cJSON_AddNumberToObject(item, "saldo", usuarios[i].saldo);
        cJSON_AddItemToArray(json, item);
    }

    char *json_string = cJSON_Print(json);
    cJSON_Delete(json);

    FILE *file = fopen("estado_cuenta.json", "w");
    if (!file) {
        perror("Could not open file");
        return;
    }

    fprintf(file, "%s", json_string);
    fclose(file);
    free(json_string);

    printf("Estado de cuenta guardado en estado_cuenta.json\n");

}

int main() {

    char buffer[250];
    //const char *filename = "usuarios.json";
    char *filename;


    printf("Ingrese la ruta del archivo de usuarios: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // Elimina el carácter de nueva línea al final de la cadena, si existe
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        filename = buffer;
    } else {
        printf("Error al leer la entrada.\n");
        return 0;
    }

    char *json_string = read_file(filename);

    if (json_string) {
        cJSON *json = parse_json(json_string);
        if (json) {
            int total_usuarios = cJSON_GetArraySize(json);
            Usuario *usuarios_temporales = (Usuario *)malloc(total_usuarios * sizeof(Usuario));

            // Extraer los usuarios del JSON
            for (int i = 0; i < total_usuarios; ++i) {
                cJSON *item = cJSON_GetArrayItem(json, i);
                usuarios_temporales[i].no_cuenta = cJSON_GetObjectItem(item, "no_cuenta")->valueint;
                strcpy(usuarios_temporales[i].nombre, cJSON_GetObjectItem(item, "nombre")->valuestring);
                usuarios_temporales[i].saldo = cJSON_GetObjectItem(item, "saldo")->valuedouble;
            }

            // Crear hilos
            pthread_t threads[3];
            ThreadData thread_data[3];
            int num_users_loaded[3] = {0, 0, 0};

            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char report_filename[100];
            char time_buffer[20];

            strftime(report_filename, sizeof(report_filename) - 1, "carga_%Y_%m_%d-%H_%M_%S.log", t);

            FILE *report_file = fopen(report_filename, "w");
            if (!report_file) {
                perror("Could not open report file");
                return 1;
            }

            // Inicializar el array de cuentas existentes con tamaño suficiente
            int max_no_cuenta = 1000000; // Ajustar según sea necesario
            bool *cuentas_existentes = (bool *)calloc(max_no_cuenta, sizeof(bool));

            // Inicializar estructuras para manejo de errores
            pthread_mutex_t error_mutex = PTHREAD_MUTEX_INITIALIZER;
            char *errores = (char *)malloc(1000 * sizeof(char)); // Tamaño inicial arbitrario
            errores[0] = '\0'; // Inicializar cadena vacía

            int chunk_size = total_usuarios / 3;

            for (int i = 0; i < 3; ++i) {
                thread_data[i].usuarios = usuarios_temporales;
                thread_data[i].start = i * chunk_size;
                thread_data[i].end = (i == 2) ? total_usuarios : (i + 1) * chunk_size;
                thread_data[i].thread_id = i;
                thread_data[i].num_users_loaded = &num_users_loaded[i];
                thread_data[i].report_file = report_file;
                thread_data[i].cuentas_existentes = cuentas_existentes;
                thread_data[i].errores = errores;
                thread_data[i].error_mutex = &error_mutex;

                pthread_create(&threads[i], NULL, cargar_usuarios, (void *)&thread_data[i]);
            }

            for (int i = 0; i < 3; ++i) {
                pthread_join(threads[i], NULL);
            }

            // Generar el reporte
            int total_users_loaded = num_users_loaded[0] + num_users_loaded[1] + num_users_loaded[2];
            fprintf(report_file, "Reporte de carga:\n\n");

            strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", t);
            fprintf(report_file, "Fecha: %s\n\n", time_buffer);

            fprintf(report_file, "Hilo 0: %d usuarios cargados\n", num_users_loaded[0]);
            fprintf(report_file, "Hilo 1: %d usuarios cargados\n", num_users_loaded[1]);
            fprintf(report_file, "Hilo 2: %d usuarios cargados\n", num_users_loaded[2]);
            fprintf(report_file, "Total de usuarios cargados: %d\n", total_users_loaded);

            // Escribir errores al final del archivo
            fprintf(report_file, "\nErrores encontrados:\n%s\n", errores);

            fclose(report_file);
            free(usuarios_temporales);
            free(cuentas_existentes);
            free(errores);
            cJSON_Delete(json);
        }
        free(json_string);
    }

    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&usuarios_mutex);

    // Call menu function
    int option;
    do {
        printf("\n\n");
        printf("1. Operaciones individuales\n");
        printf("2. Carga masiva de operaciones\n");
        printf("3. Estado de cuenta\n");
        printf("4. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &option);
        switch (option) {
            case 1:
                operaciones_individuales();
                break;
            case 2:
                carga_masiva_operaciones();
                break;
            case 3:
                estado_cuenta();
                break;
            case 4:
                break;
            default:
                printf("Opción no válida\n");
        }
    } while (option != 4);

    free(usuarios);
    return 0;
}
