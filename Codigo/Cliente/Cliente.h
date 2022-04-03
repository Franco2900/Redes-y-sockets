#ifndef CLIENTE_H_INCLUDED
#define CLIENTE_H_INCLUDED
#include <winsock.h>
#include <iostream>
#include <string.h>
#include <fstream>

using namespace std;

class Cliente{
public:

    //Atributos
    WSADATA wsa;
    SOCKADDR_IN servidorAddr;
    SOCKET socketCliente;
    int puerto = 5010;
    char buffer[1024];

    //Constructor
    Cliente()
    {
        WSAStartup(MAKEWORD(2,2), &wsa);

        socketCliente = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        servidorAddr.sin_family = AF_INET;
        servidorAddr.sin_port = htons(puerto);
        servidorAddr.sin_addr.S_un.S_addr = inet_addr("192.255.255.0"); //Dirección IP de ejemplo
    }

    //Destructor
    ~Cliente(){}

    //Metodos
    string recibir(){
        recv(socketCliente, buffer, sizeof(buffer), 0);
        cout << buffer;
        string auxbuffer = buffer;
        memset(buffer, '\0', strlen(buffer) );
        return auxbuffer;
    }


    void enviar(){
        gets(buffer);
        fflush(stdin);
        send(socketCliente, buffer, sizeof(buffer), 0);
        memset(buffer, '\0', strlen(buffer) );
    }


    void enviarInforme(){ //Sirve para informar al servidor de las acciones del cliente y que se prepare para realizar distintas acciones según el estado del cliente
        send(socketCliente, buffer, sizeof(buffer), 0);
        memset(buffer, '\0', strlen(buffer) );
    }


    void cerrarSocket(){
        closesocket(socketCliente);
        WSACleanup();
    }

};

//Funciones para que el cliente se conecte al servidor
void conectarseAlServidor(Cliente cliente);
boolean login(Cliente cliente);

//Funciones para navegar a través del menu y los submenus, y para solicitar tareas al servidor
void menu(Cliente cliente);
void submenu_TipoDeMedicamento(Cliente cliente);
void submenu_Medicamento(Cliente cliente);
void verActividad(Cliente cliente);

#endif // CLIENTE_H_INCLUDED
