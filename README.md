# Laboratorio Sistemas Operativos 2 - Practica #1

**Miembros del grupo #22**

| Nombre                            | Carnet    |
| --------------------------------- | --------- |
| Bryan Alexander Portillo Alvarado | 201602880 |
| Pablo Andres Axpuac Arevalo       | 201900096 |

# Manual Tecnico

Para la realizacion de la practica se desarrollaron los requerimientos como fueron solicitados, se elaboraron diferentes archivos en lenguaje C que sirven para generar los objetos binarios que seran ejecutados. Asi mismo se creo un archivo con extension STP que sera utilizado por el SystemTap para poder realizar la monitorizacion de los procesos hijos durante la ejecucion del proceso padre.

## Archivos y metodos utilizados

A continuacion se detallaran mejor los componentes elaborados para la solucion de la practica:

### Parent.c

Este archivo contiene la logica necesaria para que el proceso padre pueda crear a sus 2 procesos hijos y posteriormente los supervise por medio del SystemTap ejecutando un script elaborado para esta tarea especifica. Asi mismo este se encarga de la creacion y limpieza de los archivos "syscalls.log" y "practica1.txt" al inicio de cada ejecucion.

#### Metodos empleados

1. **print_syscall_summary:** Metodo utilizado para presentar el informe final de llamadas al sistema por parte de los procesos hijos.
2. **sigint_handler:** Esta funcion define el comportamiento del proceso al recibir un signal del tipo SIGINT el cual indicaria la instruccion para la terminacion de la ejecucion del proceso.
3. **main:** En este metodo se definio tota la logica necesaria para que el proceso padre pudiese crear a sus dos procesos hijos, asi como para que este pueda monitorizar las llamadas al sistema por medio del SystemTap que ejecuta con una llamada al sistema.
4. **analyze_syscalls_file:** Metodo utilizado para el analisis de los datos recopilados sobre los llamados al sistema por parte de los procesos hijos.
5. **kill:** Metodo utilizado para poder terminar un proceso hijo por medio de un signal SIGTERM.
6. **waitpid:** Metodo utilizado para frenar el flujo del proceso padre hasta que los procesos hijos terminen su ejecucion.
7. **fork:** Metodo utilizado para la creacion de procesos hijos.
8. **clean_file:** Metodo utilizado para la limpieza de los archivos utilizados (syscalls.log y practica1.txt).
9. **system:** Metodo utilizado para la ejecucion del script STP en el Shell.
10. **perror:** Metodo utilizado para el registro de errores en la terminal.

### Child.c

Este archivo contiene la logica necesaria para que cada proceso hijo pueda acceder al archivo "practica1.txt" y realice las operaciones de lectura y escritura sobre este archivo.

#### Metodos empleados

1. **main:** En este metodo de definio toda la logica necesaria para que los procesos hijos pudiesen realizar de forma aleatoria la seleccion de la tarea a realizar (entre leer y escribir en el archivo). Asi mismo se definio la logica para que el tiempo entre estas actividades fuera de forma aleatoria entre el rango de 1 a 3 segundos.
2. **sigterm_handler:** Esta funcion define el comportamiento del proceso al recibir un signal del tipo SIGTERM el cual indicaria la instruccion para la terminacion de la ejecucion del proceso.
3. **signal:** Metodo que envia un signal y un callback que indica que comportamiento se debe tener al recibir dicho signal.
4. **srand:** Metodo que se utiliza para obtener una semilla para numeros aleatorios en base a un parametro.
5. **open:** Metodo utilizado para abrir un archivo.
6. **write:** Metodo utilizado para escribir un buffer de caracteres a un archivo.
7. **read:** Metodo utilizado para leer un buffer de datos de un archivo.
8. **close:** Metodo utilizado para cerrar la conexion del proceso al archivo utilizado.
9. **sleep:** Metodo utilizado para realizar una interrupcion temporal en el flujo del proceso.
10. **exit:** Metodo utilizado para delimitar la terminacion del proceso.

### Trace.stp

En este script se definio la logica que sera utilizada por el SystemTap para poder realizar el registro de logs de las llamadas al sistema realizadas por los procesos hijos.

## Comandos utilizados

Para la compilacion de las soluciones se utilizaron los siguientes comandos.

Comando utilizado para la compilacion del codigo fuente del proceso padre:

```shell
cd <ruta del proyecto> 
gcc ./parent.c -o parent
```

Comando utilizado para la compilacion del codigo fuente del proceso hijo:

```shell
cd <ruta del proyecto> 
gcc ./child.c -o child.bin
```

Ejecucion del proceso padre:

```shell
cd <ruta del proyecto> 
./parent
```

## Herramientas utilizadas

Para la realizacion de la solucion se utilizaron las siguientes herramientas de software:

1. Sistema operativo Linux Mint Cinnamon version 21.2.
2. Compilador GCC version 11.4.0.
3. Paquete Build-Essentials (Complementos de compilacion).
4. VMWare Workstation (Como plataforma de virtualizacion del SO).
5. Visual Studio Code.
6. SystemTap.
