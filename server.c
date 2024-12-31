#include "common.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include <sys/types.h>


#define BUFSZ 1024

void usage(int argc, char **argv){
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int startConnection(int argc, char **argv, char *straddr){
    if(argc < 3){
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if(0 != server_sockaddr_init(argv[1], argv[2], &storage)){
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1){
        logexit("socket");
    }

    int enable = 1;
    if(0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))){
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(0 != bind(s, addr, sizeof(storage))){
        logexit("bind");
    }

    if(0 != listen(s, 10)){
        logexit("listen");
    }

    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    int sock = accept(s, caddr, &caddrlen);
    if (sock == -1){
        logexit("accept");
    }
    printf("Client connected\n");

    char traddr[BUFSZ];
    addrtostr(caddr, straddr, BUFSZ);

    return sock;
}


int main(int argc, char ** argv){

    int csock;
    char caddrstr[BUFSZ];
    struct sensor_message Sensor;
    int count = 0;

    csock = startConnection(argc, argv, caddrstr);//Conecta ao cliente

    while(1){
        count = recv(csock,&Sensor,sizeof(Sensor),0); //Recebe Mensagens
        if(count != sizeof(Sensor)){
            logexit("recv");
        }

        printf("Medicao: %.2f Coordenadas: %i %i\n",Sensor.measurement, Sensor.coords[0],Sensor.coords[1]);
        Sensor.coords[1]++;

        count = send(csock, &Sensor, sizeof(Sensor), 0); // Envia comando star ao servidor
        if (count != sizeof(Sensor)){
            logexit("send");
        }
    }

   //printf("%s %i %i\n",Sensor.type, Sensor.coords[0],Sensor.coords[1]);


    return 0;
}