#include<stdio.h>
#include <winsock.h> //Libreria para trabajar con sockets
#include <string.h>
#include <iostream>
#include <ctype.h>
#include "Servidor.h"
#include <cstring>  //Para converir string a char array

#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main(int argc , char *argv[])
{
    setlocale(LC_ALL, ""); //Para que me reconozca tildes y otras cosas del espa�ol
    int puerto = 5010;
    registrarInicioServidor(puerto);


    while(true){
        Servidor servidor(puerto);     //Creo un objeto de la clase servidor

        if(login(servidor) ){ //Si el cliente pasa el login, ya puede empezar a pedir tareas al servidor

            char opcion = ' ';
            while(opcion != '4'){

                opcion = servidor.recibirInforme();
                switch(opcion){ //Este es el menu principal. De ac�, seg�n las opciones que ingrese el cliente, se puede ir al submenu de los tipos de medicamento, al submenu de los medicamentos, al registro de actividades o simplemente cerrar sesi�n
                                //Al ingresar en distintos casos de uso, el servidor sabe que acciones tiene que llevar a cabo cuando el cliente lo solicite

                    case '1':   //El cliente esta en la opci�n 1: Tipo de medicamento. De ah� puede crear, administrar, ver toda la info o volver
                            atenderTipoMedicamento(servidor);
                            break;

                    case '2':   //El cliente esta en la opci�n 2: Medicamento. De ah� puede crear, administrar, ver toda la info o volver
                            atenderMedicamento(servidor);
                            break;

                    case '3':   //El cliente esta en la opci�n 3: Ver actividad. Se le pasa el registro de la actividad al usuario y se muestra toda su actividad en pantalla
                            transmitirArchivoDeActividades(servidor);
                            break;

                    case '4':   //El cliente esta en la opci�n 4: Cerrar sesi�n
                            servidor.cerrarSocket();
                            break;

                    default:
                        opcion = '4';
                        break;
                }

            }


        }

        servidor.~Servidor(); //Destruyo el objeto de la clase servidor
        printf("\n\n");
    }

    system("pause");
	return 0;
}
