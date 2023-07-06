
#include "syscall.h"


void Verifica();

int main() {

    Write("Bienvenido a pruebas\n", 21, 1 );
    Fork( Verifica );
    Yield();
}

void Verifica() {

    char buffer[100];

    Write( "Inserte una palabra:\n", 21, 1 );
    int i = 0;
    do {
        Read( &buffer[i], 100, 0 );
    } while( buffer[i++] != '\n' );
    
   
    Write( "Su palabra es:\n", 15, 1 );
    Write( buffer, 100, 1 );

}


