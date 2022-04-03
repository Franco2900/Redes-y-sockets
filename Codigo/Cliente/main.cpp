#include <winsock.h>
#include <iostream>
#include <conio.h>
#include <string.h>
#include "Cliente.h"
#include <locale.h> //Contiene la funci�n setlocale para poder usar tildes y �

#pragma comment (lib, "ws2_32.lib")


int main()
{
    setlocale(LC_ALL,""); //Para que me reconozca tildes y otras cosas del espa�ol
    Cliente cliente; //Creo un objeto de la clase cliente, este va a ser el que se use para la navegaci�n

    boolean conexionExitosa = false;
    while(!conexionExitosa){
        Cliente auxCliente; //Creo otro objeto de la clase cliente, este auxiliar va a servir solo para conectarme y hacer login

        conectarseAlServidor(auxCliente);
        conexionExitosa = login(auxCliente);

        if(conexionExitosa) { //Si puede hacer login correctamente, clono el objeto auxiliar y luego lo elimino
            cliente = auxCliente;
            auxCliente.~Cliente();
        }else auxCliente.~Cliente(); //Si no puede hacer login correctamente, se borra el objeto auxiliar y se vuelve a intentar

    }

    menu(cliente); //El cliente ingresa al menu y se queda ah� hasta que elige la opci�n Cerrar sesion

    return 0;
}
