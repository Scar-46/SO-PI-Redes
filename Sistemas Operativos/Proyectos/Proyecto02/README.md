# Llamados al sistema de NachOS

Este proyecto se centra en modificar el sistema de memoria de NachOS, así como en la construcción de los llamados al sistema necesarios para la ejecución de hilos y procesos.

Para hacer funcionar la memoria se debe modificar las clases "AddrSpace" en los archivos "addrspace.h" y "addrspace.cc", para darle soporte a la "paginación" en donde los elementos de memoria son manipulados como páginas.
Vamos a dividir la memoria de NachOS en 32 páginas de 128 bytes cada una (ambas constantes) y colocar las estructuras de datos necesarias para realizar la administración de memoria.

Por otro lado, se construyeron los distintos llamados al sistema necesarios para realizar varias tareas en NachOS como los llamados para la ejecución de hilos y procesos.

## Manual de uso

Para poder compilar los programas debe ejecutar el comando `make` en la línea de
comandos en la carpeta de `userprog` para obtener el ejucutable de NachOS.

```bash
make
```

Una vez compilado el programa se puede ejecutar con el comando `./nachos` y los argumentos que se deseen pasar al programa.


### Ejecución

Una vez compilado el programa se puede ejecutar diversos programas de prueba, los cuales se encuentran en la carpeta `test`. Para ejecutarlos se debe utilizar el comando:

```bash
./nachos -x <programa>
```

Por ejemplo `./nachos -x ../test/matmult` ejecutará el programa de multiplicación de matrices.

*Nota. los test 'sort' y 'matmult' no funcionan correctamente debido a que no se implementó la memoria virtual.*

### Salidas de los programas

Los programas de prueba tienen salidas diferentes, por ejemplo el programa `halt` no tiene salida.

El programa `addrspacetest` tiene la siguiente salida:

```bash
./nachos -x ../test/addrspacetest

abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyMachine halting!

Ticks: total 30802, idle 0, system 30, user 30772
Disk I/O: reads 0, writes 0
Console I/O: reads 0, writes 1024
Paging: faults 0
Network I/O: packets received 0, sent 0

Cleaning up...
```
El programa `copy` tiene la siguiente salida:

(se copia el archivo `../userprog/nachos1` en el archivo `../userprog/nachos2`):
    
```bash
./nachos -x ../test/copy
No threads ready or runnable, and no pending interrupts.
Assuming the program completed.
Machine halting!

Ticks: total 164, idle 0, system 70, user 94
Disk I/O: reads 2, writes 1
Console I/O: reads 0, writes 0
Paging: faults 0
Network I/O: packets received 0, sent 0

Cleaning up...
```

El programa `pingPong` tiene la siguiente salida:

```bash
./nachos -x ../test/pingPong
Hola 1
Hola 2
Hola 1
Hola 2
Hola 1
Hola 2
Hola 1
Hola 2
Hola 1
Hola 2
Fin de
Main  
Fin de
Machine halting!

Ticks: total 746, idle 0, system 390, user 356
Disk I/O: reads 0, writes 0
Console I/O: reads 0, writes 91
Paging: faults 0
Network I/O: packets received 0, sent 0

Cleaning up...
```

El programa `socket` tiene la siguiente salida:

```bash
./nachos -x ../test/socket
HTTP/1.1 200 OK
Date: Wed, 31 May 2023 21:33:19 GMT
Server: Apache
Last-Modified: Wed, 20 Jul 2022 22:41:51 GMT
ETag: "1cf-5e4444e9f1f17"
Accept-Ranges: bytes
Content-Length: 463
Connection: close
Content-Type: text/html; charset=UTF-8

<HTML>
   <HEAD>
      <meta property="og:url" content="https://os.ecci.ucr.ac.cr/">
      <meta property="og:title" content="UCR/ECCI Servidor de recursos">
      <meta property="og:description" content="Recursos para desarrollo de cursos para la ECCI-UCR, profesoMachine halting!

Ticks: total 140, idle 0, system 70, user 70
Disk I/O: reads 1, writes 1
Console I/O: reads 0, writes 512
Paging: faults 0
Network I/O: packets received 0, sent 0

Cleaning up...
```

El programa `alltest` tiene la siguiente salida:

```bash
./nachos -x ../test/alltest
Bienvenido a pruebas
Inserte una palabra:
Hola y buenas tardes
Su palabra es:
Hola y buenas tardes
Machine halting!

Ticks: total 238, idle 0, system 140, user 98
Disk I/O: reads 0, writes 0
Console I/O: reads 21, writes 157
Paging: faults 0
Network I/O: packets received 0, sent 0

Cleaning up...
```

## Créditos

### Autores

* Oscar Mario Fernandez Jimenez (oscar.fernandezjimenez@ucr.ac.cr)
