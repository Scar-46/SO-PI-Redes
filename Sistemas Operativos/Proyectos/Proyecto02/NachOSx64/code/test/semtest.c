
#include "syscall.h"


void Verifica();

int main() {

    Write("Bienvenido a prueba de semaforos\n", 33, 1 );
    Fork( Verifica );
    int semaforo = SemCreate ( 0 );
    SemWait( semaforo );
    Write("Hilo principal (Debe ser el segundo)\n", 38, 1 );
    Yield();
    SemDestroy( semaforo );
}

void Verifica() {

    Write( "Hilo secundario (Debe ser el primero)\n", 38, 1 );
    SemSignal( 0 );
}


