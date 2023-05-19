# Programa que juega la papa caliente

El programa consiste en un anillo de procesos (n) simulando la ronda en el juego de la papa caliente (paso de token). El proceso i le pasa la papa (un mensaje) al proceso i+1 o al i-1, el proceso n-1 se la pasará al 0, o el 0 lo hará al n-1 . La papa, que tiene un valor inicial (v), es movida entre los participantes, siguiendo el sentido de rotación, cada participante del juego activo modifica su valor, comprueba si la papa estalló, en cuyo caso “sale” del juego y debe escoger al azar un nuevo valor inicial para la papa (vi), para luego pasar la papa al siguiente participante. Cada participante pasivo pasa el valor de la papa sin modificarlo hasta que el juego termine. Cada participante estará representado por un proceso que tendrá un identificador único que utilizará para comunicarse con su vecino, por ejemplo: 10, 11, 12, 13, ..., 10 + n1.

El programa puede recibir tres parámetros el primero que indica la cantidad de miembros de la ronda (n), el segundo un número inicial para la papa (v) y el tercero el sentido de rotación (r) de la papa (estos valores pueden estar inicializados por omisión: n=100, v=2023, r=[i pasa la papa a i - 1]). Cada proceso recibe el valor de la papa y le aplica las reglas de Collatz, si el resultado es uno, indicativo de que la papa explota, ese proceso “sale” del juego y se convierte en un comunicador pasivo. Gana el último proceso en “salir” del juego, al que le tocará avisar a los demás procesos que el juego terminó,
pasando para ello la papa con un valor negativo (mensaje), para que los demás puedan finalizar su ejecución. El programa al menos debe desplegar el valor del proceso ganador y la manera en que los participantes van saliendo.

## La papa caliente

El juego de la papa caliente es un juego infantil en el que los participantes se sientan en círculo y se pasan una pelota o un objeto mientras suena una música. Cuando la música se detiene, la persona que tenga el objeto en ese momento queda eliminada del juego. El objetivo del juego es evitar quedarse con el objeto en el momento en que la música se detiene.

## Diseño

Este programa hace uso de diversos procesos con el objetivo de simular los diversos jugadores que participan en el juego de la papa caliente.

El programa hace uso de una clase necesaria para su ejecución, dicha clase permite enviar mensajes entre distintos procesos, lo cual permite la comunicación entre los distintos procesos (MailBox) esta clase usa funciones únicamente disponibles en Linux.

*Nota. Debido a que se utilizan dichas librerías de Linux, el programa solo será utilizable en dicho sistema operativo.

## Manual de uso

Para poder compilar el programa debe ejecutar el comando `make` en la línea de
comandos para que el sistema operativo lea y ejecute el sript con el fin de
crear un archivo ejecutable.

```bash
make
```

Este se puede encontrar en la carpeta `bin/` con el nombre de `Sistemas`.

### Ejecución

Para poder poner en marcha el programa debe ingresarse por línea de comandos la ubicación del ejecutable, este puede recibir como primer parámetro indica la cantidad de participantes, el segundo el valor inicial de la para y el tercero es el sentido de rotación de la papa.

Estos datos no son necesarios, pues al ejecutarse el programa sin dichos parámetros utilizara los siguientes por defecto: 100 jugadores, 2023 de valor inicial de la papa y 1 como dirección.

Por ejemplo `bin/Sistemas 100 2023 1` esto ejecutaría el programa por defecto.

*Nota. En la dirección los números positivos indican a la derecha y los negativos a la izquierda.

### Salida del programa

Al ejecutar el programa se obtendra una salida en consola, como la siguiente:

```bash
bin/Sistemas 5 2023 1
...
El jugador 3 tiene la papa 4
El jugador 5 tiene la papa 2
El jugador 1 tiene la papa 1
El jugador 1 se fue del juego
*Han salido 3 jugadores de 5*
El jugador 3 tiene la papa 1978
El jugador 5 tiene la papa 989
El jugador 3 tiene la papa 2968
...
El jugador 5 tiene la papa 8
El jugador 3 tiene la papa 4
El jugador 5 tiene la papa 2
El jugador 3 tiene la papa 1
El jugador 3 se fue del juego
*Han salido 4 jugadores de 5*
El jugador 5 tiene la papa 1978
El jugador 5 es el ganador
```

En dicha salida se puede apreciar cuantos jugadores han salido del juego, que jugador tiene la papa y que valor de la misma tiene la misma luego de aplicar las reglas de Collatz, y al final de la ejecución cuál fue el jugador que gano.

### **Otras consideraciones**

* Por defecto este script realizará un ejecutable
con la capacidad de "debugueo", el cual es más ineficiente. Para obtener un el
programa más optimizado es necesario utilizar el argumento `release`.

```bash
make release 
```

## Créditos

### Autores

* Oscar Mario Fernandez Jimenez (oscar.fernandezjimenez@ucr.ac.cr)
