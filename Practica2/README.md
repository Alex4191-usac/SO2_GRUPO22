# Laboratorio Sistemas Operativos 2 - Practica #2

**Miembros del grupo #22**

| Nombre                            | Carnet    |
| --------------------------------- | --------- |
| Bryan Alexander Portillo Alvarado | 201602880 |
| Pablo Andres Axpuac Arevalo       | 201900096 |

### Descripcion

La práctica consiste en realizar una aplicación en consola en C que permita almacenar los datos de usuario de un banco, así como poder realizar operaciones monetarias como depósitos, retiros y transacciones.

### Descripcion de componentes

#### Archivos disponibles:

#### cJSON
cJSON es una librería en C para manipular JSON (JavaScript Object Notation). JSON es un formato ligero de intercambio de datos fácil de leer y escribir tanto para humanos como para máquinas. cJSON facilita el trabajo con JSON en C proporcionando funciones para crear, parsear, y manipular JSON de manera sencilla.

 se utilizara los archivos con extencion `.c` & `.h` con nombre principal cJSON del repositorio `https://github.com/DaveGamble/cJSON`

#### Archivo testp
en este archivo se encuentra toda la logica correspondiente a la aplicacion de consola que simula la manipulacion de usuarios en un banco.

#### Archivos generables:

####  “operaciones_ yyyy_MM_dd-HH_mm_ss .log”
Cuando se realice una carga masiva de operaciones se realizará de manera automática un reporte de las operaciones realizadas en un archivo llamado “operaciones_ yyyy_MM_dd-HH_mm_ss .log”, teniendo la fecha en la que se generó el reporte, en el se detallan un desglose de las operaciones realizadas, así como el total de operaciones realizadas por cada hilo. Ademas  se deberán listar los errores en las operaciones si en caso existieran.

#### Reporte de carga de usuarios
se realizará de manera automática al terminar la carga masiva de usuarios en un 
archivo llamado “carga_ yyyy_MM_dd-HH_mm_ss .log”, teniendo la fecha en la que se generó el 
reporte, en el se describen un desglose de cuantos usuarios cargo en el sistema cada hilo, así como el total de usuarios cargados. Ademas de un listado de errores en caso se detecten en el proceso.

#### Estado de cuentas

Desde el menú, un operador podrá generar un reporte que mostrara la información de los usuarios, este 
será escrito en un JSON, el nombre del archivo es `estado_cuentas.json`


### Comandos utilizados

Para la compilacion de las soluciones se utilizaron los siguientes comandos:

el archivo testp.c debe ser generado en un ejecutable para ello podemos utilizar la siguiente expresion:

```bash
gcc testp.c -o testp
```

y ejecutarlo en la ruta relativa:

```bash
./testp
```

### Resumen de Funciones y Estructuras
#### Estructuras

 * Usuario
Estructura para almacenar la información de un usuario.
```c
typedef struct {
    int no_cuenta;
    char nombre[100];
    double saldo;
} Usuario;
```

* Operacion
Estructura para almacenar la información de las operaciones.
```c
typedef struct {
    int operacion;
    int cuenta1;
    int cuenta2;
    double monto;
} Operacion;

```

* OperacionThreadData
Estructura para pasar datos a los hilos que ejecutarán operaciones.

```c
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

```

* ThreadData
Estructura para pasar datos a los hilos que cargarán usuarios.

```c
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

```

#### Funciones

* read_file
Función para leer el contenido de un archivo.

```c
char *read_file(const char *filename);
```

* parse_json
Función para parsear el JSON y extraer los usuarios.

```c
cJSON *parse_json(const char *json_string);
```

* agregar_usuario
Función para agregar un usuario a la estructura compartida.

```c
void agregar_usuario(Usuario usuario);
```

* cargar_usuarios
Función que ejecutarán los hilos para cargar usuarios.

```c
void *cargar_usuarios(void *arg);
```

* buscar_usuario
Función para buscar un usuario por número de cuenta.

```c
Usuario* buscar_usuario(int no_cuenta);
```

* deposito
Función para realizar un depósito.

```c
void deposito();
```

* retiro
Función para realizar un retiro.

```c
void retiro();
```

* consultar_cuenta
Función para consultar una cuenta.

```c
void consultar_cuenta();
```
* transaccion
Función para realizar una transacción.

```c
void transaccion();

```

* operaciones_individuales
Función para manejar el menú de operaciones individuales.

```c
void operaciones_individuales();
```

* masivo_deposito
Función para realizar un depósito masivo.

```c
int masivo_deposito(int no_cuenta, double monto);
```

* ejecutar_operaciones
Función que ejecutarán los hilos para ejecutar operaciones masivas.

```c
void *ejecutar_operaciones(void *arg);
```

* carga_masiva_operaciones
Función para manejar la carga masiva de operaciones desde un archivo JSON.

```c
void carga_masiva_operaciones();
```

* estado_cuenta
Función para guardar el estado de las cuentas en un archivo JSON.

```c
void estado_cuenta();
```

#### Mutex y Semáforos

pthread_mutex_t lock : Mutex para sincronización general.
pthread_mutex_t usuarios_mutex : Mutex para controlar acceso a la estructura de usuarios.
sem_t semaforo : Semáforo para sincronización en operaciones masivas.

```c
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t usuarios_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t semaforo;

```

