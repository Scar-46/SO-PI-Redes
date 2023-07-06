# Memoria virtual en NachOS

En el proyecto de memoria virtual en NachOS, se llevó a cabo una serie de modificaciones en el sistema operativo con el objetivo de habilitar y aprovechar la funcionalidad de la memoria virtual, permitiendo así la utilización de una cantidad de memoria superior a la que se encuentra físicamente disponible. Este avance significativo se logró mediante la implementación de un sistema de paginación altamente eficiente y el empleo del algoritmo de reemplazo de páginas conocido como "segunda oportunidad" (second chance).

La memoria virtual es una técnica que permite que los programas en ejecución tengan acceso a una extensión de memoria más amplia que la memoria física instalada en el sistema. En lugar de cargar todos los datos de un programa en la memoria principal de una vez, la memoria virtual divide la información en pequeñas unidades llamadas páginas. Estas páginas se almacenan tanto en la memoria física como en un espacio de almacenamiento en disco llamado archivo de intercambio (swap file).

El sistema de paginación implementado en NachOS se encarga de gestionar el mapeo entre las páginas virtuales y las páginas físicas, permitiendo que cada proceso en ejecución pueda acceder a sus páginas requeridas de manera transparente. Cuando un programa necesita acceder a una página que no se encuentra en la memoria física, se produce un fallo de página (page fault) y el sistema operativo se encarga de traer dicha página desde el archivo de intercambio a la memoria principal.

El algoritmo de reemplazo de páginas "segunda oportunidad" (second chance) es utilizado para decidir qué página de la memoria física debe ser reemplazada cuando se produce un fallo de página y no hay espacio suficiente para cargar una nueva página. Este algoritmo se basa en el principio de dar una "segunda oportunidad" a las páginas que no han sido referenciadas recientemente. Cada página tiene un bit de referencia asociado que se establece en 1 cada vez que la página es accedida. Un puntero recorre todas las páginas en un ciclo y si encuentra una página con el bit de referencia en 0, la reemplaza. Si el bit de referencia es 1, se lo pone en 0 y se continúa con la siguiente página en el ciclo. Este proceso se repite hasta encontrar una página candidata para ser reemplazada.

## Implementación

La implementación de la memoria virtual en NachOS se realizó en dos etapas. En primer lugar, se implementó el sistema de paginación, el cual se encarga de gestionar el mapeo entre las páginas virtuales y las páginas físicas. En segundo lugar, se implementó el algoritmo de reemplazo de páginas "segunda oportunidad" (second chance) para decidir qué página de la memoria física debe ser reemplazada cuando se produce un fallo de página y no hay espacio suficiente para cargar una nueva página.

Para implementar la memoria virtual en NachOS se realizaron modificaciones en los siguientes archivos:

- `machine/system.cc`
- `machine/system.h`
- `threads/thread.cc`
- `threads/thread.h`
- `userprog/addrspace.cc`
- `userprog/addrspace.h`
- `userprog/exception.cc`
- `userprog/exception.h`

Se agregaron los siguientes archivos:

- `vm/swap.cc`
- `vm/swap.h`
- `vm/invertedpagetable.cc`
- `vm/invertedpagetable.h`

Por el lado de la paginación por demanda, se implementó el algoritmo de reemplazo de páginas "segunda oportunidad" (second chance) en el archivo `vm/invertedpagetable.cc` y en la tlb.

Para la administración de la memoria se utilizó la tabla de páginas invertida que es una estructura de datos centralizada que almacena información sobre la ubicación de las páginas virtuales en la memoria física. A diferencia de una tabla de páginas convencional, donde cada proceso tiene su propia tabla de páginas, la tabla de páginas invertida es compartida entre todos los procesos en ejecución.

Cada entrada de la tabla de páginas invertida contiene información relevante, como el número de página virtual, el número de página física correspondiente y otros atributos necesarios para gestionar eficientemente el mapeo entre las páginas virtuales y físicas. Al utilizar esta tabla, el sistema operativo puede consultar rápidamente la ubicación física de una página virtual cuando se produce una referencia a dicha página.

La adopción de una tabla de páginas invertida presenta varias ventajas. En primer lugar, reduce la cantidad de memoria necesaria para almacenar las tablas de páginas individuales de cada proceso, ya que solo se requiere una única tabla compartida. Además, simplifica la gestión del mapeo de páginas, ya que todas las referencias a páginas virtuales pueden ser resueltas de manera eficiente a través de una búsqueda en esta tabla centralizada.

## Manual de uso

Para poder compilar los programas debe ejecutar el comando `make` en la línea de
comandos en la carpeta de `vm` para obtener el ejucutable de NachOS.

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

### Salidas de los programas

Los programas de prueba tienen salidas diferentes, por ejemplo el programa `halt` no tiene salida.

El programa `addrspacetest` tiene la siguiente salida:

```bash
./nachos -x ../test/addrspacetest

abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxyz{abcdefghijklmnopqrstuvwxy
No threads ready or runnable, and no pending interrupts.
Assuming the program completed.
Machine halting!

Ticks: total 30812, idle 0, system 10, user 30802
Disk I/O: reads 0, writes 0
Console I/O: reads 0, writes 1024
Paging: faults 36
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

Ticks: total 173, idle 0, system 70, user 103
Disk I/O: reads 2, writes 1
Console I/O: reads 0, writes 0
Paging: faults 10
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

No threads ready or runnable, and no pending interrupts.
Assuming the program completed.
Machine halting!

Ticks: total 623, idle 0, system 190, user 433
Disk I/O: reads 0, writes 0
Console I/O: reads 0, writes 91
Paging: faults 66
Network I/O: packets received 0, sent 0

Cleaning up...
```

El programa `socket` tiene la siguiente salida:

```bash
./nachos -x ../test/socket
HTTP/1.1 200 OK
Date: Sat, 08 Jul 2023 06:14:05 GMT
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
      <meta property="og:description" content="Recursos para desarrollo de cursos para la ECCI-UCR, profeso
No threads ready or runnable, and no pending interrupts.
Assuming the program completed.
Machine halting!

Ticks: total 129, idle 0, system 40, user 89
Disk I/O: reads 1, writes 1
Console I/O: reads 0, writes 512
Paging: faults 27
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

No threads ready or runnable, and no pending interrupts.
Assuming the program completed.
Machine halting!

Ticks: total 177, idle 0, system 50, user 127
Disk I/O: reads 0, writes 0
Console I/O: reads 21, writes 157
Paging: faults 27
Network I/O: packets received 0, sent 0

Cleaning up...
```
El programa `shell` tiene la siguiente salida:

```bash
./nachos -x ../test/shell
--../test/todos
Espera por el copy ...
Verificando ...

Bien! Perfecto muchachos!!

--../test/halt

No threads ready or runnable, and no pending interrupts.
Assuming the program completed.
Machine halting!

Ticks: total 1930, idle 0, system 800, user 1130
Disk I/O: reads 2, writes 1
Console I/O: reads 27, writes 72
Paging: faults 68
Network I/O: packets received 0, sent 0

Cleaning up...
```
El programa `matmult20` tiene la siguiente salida:

```bash
./nachos -x ../test/matmult20

No threads ready or runnable, and no pending interrupts.
Assuming the program completed.
Machine halting!

Ticks: total 657186, idle 0, system 10, user 657176
Disk I/O: reads 0, writes 0
Console I/O: reads 0, writes 0
Paging: faults 58826
Network I/O: packets received 0, sent 0

Cleaning up...
```

El programa `sort` tiene la siguiente salida:

```bash
./nachos -x ../test/sort

No threads ready or runnable, and no pending interrupts.
Assuming the program completed.
Machine halting!

Ticks: total 17905679, idle 0, system 10, user 17905669
Disk I/O: reads 0, writes 0
Console I/O: reads 0, writes 0
Paging: faults 820223
Network I/O: packets received 0, sent 0
```

## Créditos

### Autores

* Oscar Mario Fernández Jiménez (oscar.fernandezjimenez@ucr.ac.cr)
