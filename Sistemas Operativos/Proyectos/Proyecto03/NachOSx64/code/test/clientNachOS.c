#include "syscall.h"

void main () {
    int id, i, j, readM = 512;
	char buffer[20], req[50], resp[512];

    for (i = 0; i < 20; ++i) {
        buffer[i] = '\0';
    }

    Write("Inserte figura:", 15, 1);
    Read(buffer, 20, 0);

    for (i = 18; i < 38; ++i) {
        req[i] = buffer[i - 18];
        if (buffer[i - 18] == '\0') {
            i --;
            break;
        }
    }

    req[0] = 'G';
    req[1] = 'E';
    req[2] = 'T';
    req[3] = ' ';
    req[4] = '/';
    req[5] = 'l';
    req[6] = 'e';
    req[7] = 'g';
    req[8] = 'o';
    req[9] = '/';
    req[10] = 'N';
    req[11] = 'f';
    req[12] = 'i';
    req[13] = 'g';
    req[14] = 'u';
    req[15] = 'r';
    req[16] = 'e';
    req[17] = '=';
    // Here goes the figure //
    req[i] = ' ';
    req[i+1] = 'H';
    req[i+2] = 'T';
    req[i+3] = 'T';
    req[i+4] = 'P';
    req[i+5] = '/';
    req[i+6] = '1';
    req[i+7] = '.';
    req[i+8] = '1';
    req[i+9] = '\r';
    req[i+10] = '\n';
    req[i+11] = '\r';
    req[i+12] = '\n';
    req[i+13] = '\0';

    id = Socket( AF_INET_NachOS, SOCK_STREAM_NachOS );
    Connect( id, "127.0.0.1", 3141 );
    Write(req, 50, id);
    while(readM > 0) {
        readM = Read(resp, readM, id);
        Write(resp, readM, 1);
    }

    Close(id);
} 	
