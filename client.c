#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFSZ 1024

void usage(char **argv){
	printf("usage: %s <server IP> <port> -type <temperature|humidity|air_quality> -coords <x> <y>\n", argv[0]);
	exit(EXIT_FAILURE);
}

int startConnection(int argc, char **argv, int s){
	if (argc < 8){
        printf("Error: Invalid number of arguments\n");
		usage(argv);
	}else if(strcmp(argv[3],"-type") != 0){
        printf("Error: Expected'-type' argument\n");
		usage(argv);
    }else if(strcmp("temperature",argv[4]) != 0 && strcmp(argv[4],"air_quality") != 0 && strcmp(argv[4],"humidity") != 0){
        printf("Error: Invalid sensor type\n");
        usage(argv);
    }else if(strcmp(argv[5],"-coords") != 0){
        printf("Error: Expected'-coords' argument");
        usage(argv);
    }else if(argv[6][1] != 0 || argv[7][1] != 0){
        printf("Error: Coordinates must be in the range 0-9");
        usage(argv);
    }

	struct sockaddr_storage storage;
	if(0 != addrparse(argv[1], argv[2], &storage)){
		usage(argv);
	}

	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if(s == -1){
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if(0 != connect(s, addr, sizeof(storage))){
		logexit("connect");
	}
	return s;
}


int main(int argc, char **argv){
	int s;
	char buf[BUFSZ];
    struct sensor_message Sensor;
    int count = 0;
    int time_elapsed = 0;
    int base = 0;
    int diferenca = 0;

	s = startConnection(argc, argv, s); // Faz socket(), bind(), listen() e accept()

    strcpy(Sensor.type,argv[4]);
    Sensor.coords[0] = argv[6][0] - 48;
    Sensor.coords[1] = argv[7][0]- 48;

    if(strcmp(Sensor.type,"temperature")== 0){
        time_elapsed = 5;
        base = 20;
        diferenca = 20;

    }else if(strcmp(Sensor.type,"humidity") == 0){
        time_elapsed = 7;
        base = 10;
        diferenca  = 80;
    }else if(strcmp(Sensor.type,"air_quality") == 0){
        time_elapsed = 10;
        base = 15;
        diferenca = 15;
    }
    while(1){
        printf("Inicio\n");
        Sensor.measurement = (float)rand()/RAND_MAX;
        Sensor.measurement = base + Sensor.measurement * diferenca;

        count = send(s, &Sensor, sizeof(Sensor), 0); // Envia comando star ao servidor
        if (count != sizeof(Sensor)){
            logexit("send");
        }

        printf("Enviado\n");

        count = recv(s, &Sensor, sizeof(Sensor), 0); // Recebe os movimentos possíveis do jogador
        if (count != sizeof(Sensor)){
            logexit("recv");
        }
        printf("Medicao: %.2f Coordenadas: %i %i\n",Sensor.measurement, Sensor.coords[0],Sensor.coords[1]);
        sleep(time_elapsed);
    }
}