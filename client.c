#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024

void usage(char **argv) //IVerifica a conexão do servidor
{
	printf("usage: %s <server IP> <port> -type <temperature|humidity|air_quality> -coords <x> <y>\n", argv[0]);
	exit(EXIT_FAILURE);
}

int startConnection(int argc, char **argv, int s) //Implementa a conexão com o servidor socket() e connect()
{
	if (argc < 8)
	{
        printf("Error: Invalid number of arguments\n");
		usage(argv);
	}
    else if(strcmp(argv[3],"type") != 0){
        printf("%s\n", argv[3]);
        printf("Error: Expected'-type' argument\n");
		usage(argv);
    }

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage))
	{
		usage(argv);
	}

	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1)
	{
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage)))
	{
		logexit("connect");
	}
	return s;
}


int main(int argc, char **argv)
{
	int s;
	char buf[BUFSZ];
    struct sensor_message Sensor;
    int count = 0;

	s = startConnection(argc, argv, s); // Faz socket(), bind(), listen() e accept()

    strcpy(Sensor.type,argv[4]);
    Sensor.coords[0] = argv[6][0] - 48;
    Sensor.coords[1] = argv[7][0]- 48;

    count = send(s, &Sensor, sizeof(Sensor), 0); // Envia comando star ao servidor
	if (count != sizeof(Sensor))
	{
		logexit("send");
	}

}