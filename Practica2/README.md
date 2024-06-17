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

