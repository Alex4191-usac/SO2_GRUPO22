# Laboratorio Sistemas Operativos 2 - Proyecto 

**Miembros del grupo #22**

| Nombre                            | Carnet    |
| --------------------------------- | --------- |
| Bryan Alexander Portillo Alvarado | 201602880 |
| Pablo Andres Axpuac Arevalo       | 201900096 |

### Descripcion

El manejo eficiente de la memoria es un aspecto crucial en el rendimiento y la estabilidad de cualquier sistema operativo. En el entorno de Linux, entender cómo se asigna y libera la memoria por parte de los procesos es fundamental para optimizar el uso de los recursos del sistema y garantizar su buen funcionamiento bajo diversas cargas de trabajo.

El proyecto "Manejo de Memoria" se enfoca en desarrollar una aplicación capaz de monitorear el uso de memoria de cada proceso abierto en un sistema Linux. Esta aplicación proporciona a los usuarios una herramienta valiosa para entender cómo interactúan los procesos con el sistema operativo en términos de asignación y liberación de memoria, así como para identificar posibles cuellos de botella y optimizar la asignación de recursos.

Este manual técnico tiene como objetivo proporcionar una guía completa para entender, configurar y utilizar la aplicación desarrollada. Se describen los componentes principales de la aplicación, las funcionalidades que ofrece y las instrucciones paso a paso para su instalación y uso.



### Arquitectura
![alt text](assets/image.png)

### memory_request.stp
Este script SystemTap (memory_request.stp) está diseñado para monitorear las llamadas al sistema mmap2 y munmap en un sistema Linux. Estas llamadas están relacionadas con la asignación y liberación de memoria por parte de los procesos del sistema operativo. El script registra información como el PID del proceso, el nombre del proceso, la fecha y hora de la solicitud, así como el tamaño del segmento de memoria involucrado en la llamada.

* Este script captura las llamadas al sistema mmap2, que se utilizan para asignar un nuevo segmento de memoria virtual en el espacio de direcciones de un proceso.

* Registra información sobre el proceso que realiza la llamada, como el PID, el nombre del proceso, la fecha y hora de la solicitud, y el tamaño del segmento de memoria. Manejo de llamadas al sistema munmap:

* El script también intercepta las llamadas al sistema munmap, que se utilizan para liberar un segmento de memoria previamente asignado.
Registra información similar a las llamadas al sistema mmap2.

### Estructura del código:

*Encabezado y declaración global:*

El script comienza con el shebang (#!) que indica la ubicación del intérprete de SystemTap.
Se declara una variable global last_call para almacenar la última llamada registrada, evitando duplicados. 

*Manejadores de llamadas al sistema:*

* probe syscall.mmap2: Maneja las llamadas al sistema mmap2, formatea la información relevante y la imprime si es diferente de la última llamada registrada.
* probe syscall.munmap: Maneja las llamadas al sistema munmap, similar a mmap2.

```bash
# Handler para la llamada al sistema mmap2
probe syscall.mmap2 {
    current_call = sprintf("mmap,%d,%s,%s,%d\n", pid(), execname(), tz_ctime(gettimeofday_s()), length/(1024 * 1024))
    if (current_call != last_call && length/ (1024 * 1024)>0) {
        printf("%s", current_call)
        last_call = current_call
    }
}

# Handler para la llamada al sistema munmap
probe syscall.munmap {
    current_call = sprintf("munmap,%d,%s,%s,%d\n", pid(), execname(), tz_ctime(gettimeofday_s()), length/(1024 * 1024))
    if (current_call != last_call && length/ (1024 * 1024)>0) {
        printf("%s", current_call)
        last_call = current_call
    }
}

```


### main.c:

El archivo main.c es un programa en lenguaje C que se encarga de conectar con una base de datos MySQL, ejecutar un script SystemTap para monitorear las llamadas al sistema relacionadas con el manejo de memoria en Linux, procesar la salida de dicho script y almacenar la información en la base de datos.

#### *Funcionalidad:*

*Conexión a la base de datos MySQL:*

El programa se conecta a una base de datos MySQL utilizando la biblioteca libmysqlclient.
Se establecen los parámetros de conexión como la dirección del servidor, el nombre de usuario, la contraseña y el nombre de la base de datos.

*Ejecución del script SystemTap:*

Utiliza popen para ejecutar el script SystemTap memory_requests.stp con privilegios de superusuario (sudo).
El script monitorea las llamadas al sistema mmap2 y munmap, capturando información sobre el PID del proceso, el nombre del proceso, la fecha y hora de la solicitud, y el tamaño del segmento de memoria involucrado.

*Procesamiento de la salida del script SystemTap:*

Lee la salida del script línea por línea utilizando fgets.
Divide cada línea en tokens utilizando la coma como delimitador.
Extrae la información relevante, como el tipo de llamada, el PID del proceso, el nombre del proceso, la fecha y hora de la solicitud, y el tamaño del segmento de memoria.

*Almacenamiento en la base de datos:*

Utiliza la biblioteca MySQL para ejecutar consultas SQL e insertar la información obtenida en la base de datos.
Crea una consulta SQL dinámica utilizando la información procesada y la ejecuta con mysql_query.


### API

la api fue desarrollada en el lenguaje de Javascript con el framework de NODE.JS con express
esta tiene la finalidad de recolectar los datos que fueron almacenados en la base de datos
de MYSQL y que fueron llamados-almacenados por ordenes de main.c;
dentro esta interfaz se tienen dos rutas a las cuales hace enfasis, los cuales son:

```Javascript
router.get('/allData', kernelController.getAllData);
router.get('/getByProceso', kernelController.getByProceso);
```
las funciones dentro de los controllador son las siguientes

#### `getAllData`

Esta función recupera todos los datos de la tabla `Data` en la base de datos.

- **Ruta**: `/getAllData`
- **Método**: `GET`
- **Descripción**: Ejecuta una consulta para seleccionar todos los registros de la tabla `Data` y devuelve el resultado en formato JSON.
- **Código**:
    ```javascript
    async function getAllData(req, res) {
        try {
            const query = 'SELECT * FROM Data';
            const result = await db.query(query);
            console.log(result[0]);
            res.json(result[0]);
        } catch (error) {
            console.log(error);
        }
    }
    ```

### `getByProceso`

Esta función recupera datos agrupados por el proceso (`pid`) y el nombre, sumando el tamaño total (`tamano`) de cada grupo.

- **Ruta**: `/getByProceso`
- **Método**: `GET`
- **Descripción**: Ejecuta una consulta que selecciona el `pid`, el `nombre`, y la suma del tamaño (`tamano`) agrupados por `pid` y `nombre`, devolviendo el resultado en formato JSON.
- **Código**:
    ```javascript
    async function getByProceso(req, res) {
        try {
            const query = `select pid, nombre, SUM(tamano) as total_tamanio
                            from Data
                            GROUP BY pid, nombre;`;
            const result = await db.query(query);
            console.log(result[0]);
            res.json(result[0]);
        } catch (error) {
            console.log(error);
        }
    }
    ```



### Instrucciones para la Ejecución

1. Clona el repositorio.
2. Instala las dependencias necesarias utilizando `npm install` en la carpeta *api*.
3. Instala las dependencias necesarias para levantar el cliente utilizando `npm install` dentro de la carpeta
*dashboard-app*
4. Configura la conexión a la base de datos en el archivo de configuración correspondiente (env) para la api.
5. Inicia el servidor con `npm start`.
6. Inicia el cliente con `npm run dev`.
7. Asegurate de tener instalado la libreria para conexion de mysql para el archivo main.c
8. genera el ejecutable `gcc main -o main.c`
9. ejecutalo como `./main`