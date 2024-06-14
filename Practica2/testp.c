#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include "cJSON.h"
#include "cJSON.c"

// Estructura para almacenar la información de un usuario
typedef struct {
    int no_cuenta;
    char nombre[100];
    double saldo;
} Usuario;

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

    printf("Información de la cuenta:\n");
    printf("Número de cuenta: %d\n", usuario->no_cuenta);
    printf("Nombre: %s\n", usuario->nombre);
    printf("Saldo: %.2f\n", usuario->saldo);
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
                // deposito();
                break;
            case 2:
                // retiro();
                break;
            case 3:
                // transaccion();
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

void carga_masiva_operaciones() {
    printf("Carga masiva de operaciones\n");
}

void estado_cuenta() {
    printf("Estado de Cuentas:\n");
    for (int i = 0; i < num_usuarios; ++i) {
        printf("Número de cuenta: %d, Nombre: %s, Saldo: %.2f\n", usuarios[i].no_cuenta, usuarios[i].nombre, usuarios[i].saldo);
    }
}

int main() {
    const char *filename = "usuarios.json";
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
            fprintf(report_file, "Reporte de carga:\n");
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
