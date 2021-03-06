#ifndef SERVIDOR_H
#define SERVIDOR_H

#include<stdio.h>
#include <winsock.h> //Libreria para trabajar con sockets en windows
#include <string.h>
#include <iostream>
#include <ctype.h>
#include <locale.h>
#include <ctime>    //Para saber la fecha actual
#include <fstream>  //Para trabajar con archivos

using namespace std;

class Servidor{
public:

    //Atributos
    WSADATA wsa;
    SOCKADDR_IN servidorAddr, clienteAddr;
    SOCKET socketServidor, socketCliente;
    char buffer[1024];

    //Constructor
    Servidor(int puerto)
    {
        WSAStartup(MAKEWORD(2,2),&wsa); //Inicializar WinSock
        socketServidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Crear socket

        servidorAddr.sin_family = AF_INET;                      //FAMILIA
        servidorAddr.sin_port = htons(puerto);                  //PUERTO
        servidorAddr.sin_addr.s_addr = htonl(INADDR_ANY);       //IP

        bind(socketServidor, (SOCKADDR *)&servidorAddr, sizeof(servidorAddr) ); //Asocia socket con el puerto

        if(listen(socketServidor, 1) == -1) printf("Solo puede haber un cliente conectado a la vez\n"); //Indico que se puede conectar al servidor como maximo un cliente
        printf("Escuchando por conexiones\n");

        //Si se conecta alg?n cliente
        int clienteAddrLen = sizeof(clienteAddr);
        socketCliente = accept(socketServidor, (SOCKADDR *)&clienteAddr, &clienteAddrLen);
        if(socketCliente != INVALID_SOCKET){
            printf("IP del cliente conectado: %s \n\n",inet_ntoa(clienteAddr.sin_addr) ); //Para saber el IP del cliente
            closesocket(socketServidor); //Cierro el socket servidor para que no escuche por m?s clientes; el objeto servidor va a seguir funcionando
        }


        //Manda mensaje al cliente
        strcpy(buffer,"Bienvenido. Ingrese usuario y contrase?a \n");
        enviar();
    }


    //Destructor
    ~Servidor(){};

    //M?todos
    string recibir(){

        //Codigo para desconectar al cliente si no hay actividad en 2 minutos
        fd_set fd;
        struct timeval tv;
        tv.tv_sec = 120; //Indico que se espere hasta 120 segundos (2 minutos)
        tv.tv_usec = 0;

        FD_ZERO(&fd);
        FD_SET(socketCliente, &fd);

        if (select(socketCliente, &fd, NULL, NULL, &tv) == 0){ //Si se supero los 120 segundos de espera, se desconecta el cliente
            cout << "Tiempo de espera excedido: Cliente desconectado \n";
            cerrarSocket();
        }

        recv(socketCliente, buffer, sizeof(buffer), 0); //La funci?n recv recibe informacion y la almacena en el buffer
        string auxBuffer = buffer;                      //Se traslado el contenido del buffer a un string porque un string es mucho m?s facil de manejar que un char array
        memset(buffer,'\0',strlen(buffer) );            //Se usa memset para vaciar el buffer
        return auxBuffer;
    }


    void enviar(){
        send(socketCliente, buffer, sizeof(buffer), 0); //Como nadie va a escribir desde el lado del servidor, puedo simplemente copiar los mensajes directamente en el bufer del servidor y env?arlos al cliente
        memset(buffer,'\0',strlen(buffer) );
    }


    char recibirInforme(){ //Este metodo sirve para que el servidor se pueda manejar entre los casos de uso. No tiene un control de tiempo como el otro m?todo de recibir porque el programa del cliente es quien envia el mensaje de reporte, no el cliente

        //Codigo para desconectar al cliente si no hay actividad en 2 minutos
        fd_set fd;
        struct timeval tv;
        tv.tv_sec = 120; //Indico que se espere hasta 120 segundos (2 minutos)
        tv.tv_usec = 0;

        FD_ZERO(&fd);
        FD_SET(socketCliente, &fd);

        if (select(socketCliente, &fd, NULL, NULL, &tv) == 0){ //Si se supero los 120 segundos de espera, se desconecta el cliente
            cout << "Tiempo de espera excedido: Cliente desconectado \n";
            cerrarSocket();
        }

        recv(socketCliente, buffer, sizeof(buffer), 0);
        char auxBuffer = buffer[0];
        memset(buffer,'\0', 1); //Como los informes solo estan compuestos por un caracter, borro solo el contenido del primer caracter
        return auxBuffer;
    }

    void cerrarSocket(){
        closesocket(socketCliente);
        WSACleanup();
    }

};

//Funciones que sirven para los registros
string fechaHoraActual();
void registrarInicioServidor(int puerto); //Para registrar cuando el servidor empieza sus actividades

//Funciones para cuando un cliente intenta hacer login en el servidor
boolean chequearUsuarioYContrasenia(string usuarioIngresado, string contraseniaIngresada);
boolean login(Servidor servidor);

//Funciones del submenu Tipo de medicamento (opcion 1 del menu principal)
void atenderTipoMedicamento(Servidor servidor);
void crearTipoMedicamento(Servidor servidor);
void administarTipoMedicamento(Servidor servidor);
void verTodoTipoMedicamento(Servidor servidor);

string convertirAMayusculas(string s); //Funcion que se usa en otros metodos

//Funciones del submenu Medicamento (opcion 2 del menu principal)
void atenderMedicamento(Servidor servidor);
void crearMedicamento(Servidor servidor);
bool validador_de_codigo(char codigo[15]);
void administarMedicamento(Servidor servidor);
void verTodoMedicamento(Servidor servidor);

//Funcion para mandar el archivo al cliente (opcion 3 del menu principal)
void transmitirArchivoDeActividades(Servidor servidor);

#endif // SERVIDOR_H
