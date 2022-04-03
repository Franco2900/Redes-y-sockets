#include "Cliente.h"
#include <string.h>
#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;
/****************************************************************************************************/
void conectarseAlServidor(Cliente cliente){

    while(true){

        cout << "Intentando conectar al servidor en el puerto: " << cliente.puerto << endl;

        //Intento conectarme al servidor
        if(connect(cliente.socketCliente, (SOCKADDR *)&cliente.servidorAddr, sizeof(cliente.servidorAddr) ) == 0){ //En caso de que me pueda conectar al servidor
            printf("Conectado al servidor \n");
            break; //Salgo del while inmediatamente
        }
        else printf("\nSolo puede haber un cliente conectado a la vez o el servidor no esta funcionando, por favor inténtelo más tarde \n"); //En caso de que no me pueda conectar al servidor

        cout << "¿Quiere intentar de vuelta?" << endl;

        boolean opcionValida = false;
        while(!opcionValida){

            int opcion = 0;
            cout << "1) Probar conectarse al servidor otra vez" << endl;
            cout << "2) Cerrar programa" << endl;
            scanf("%d",&opcion);
            fflush(stdin);

            switch(opcion){
                case 1:
                    opcionValida = true;
                    break;

                case 2:
                    exit(0);
                    break;

                default:
                    printf("Elija una opción valida \n");
                    break;
            }

        }

    }
}
/****************************************************************************************************/
/****************************************************************************************************/
boolean login(Cliente cliente){

    boolean accesoConcedido = false;
    while(!accesoConcedido){

        string aux = cliente.recibir();
        if(aux == "Acceso concedido\n") { //En caso de que el cliente se haya logueado correctamente
            accesoConcedido = true;
            system("cls");
        }

        if(aux == "Se superó la cantidad máxima de intentos de ingreso\n") { //En caso de que el cliente haya fallado ingresado 3 veces mal el usuario y contraseña
            cliente.cerrarSocket();

            system("pause");
            system("cls");
            break;
        }

        if(!accesoConcedido){ //No borro la pantalla cada vez que se ingresa un usuario o contraseña porque así el cliente donde se equivoco al tipear
            printf("Escribe el nombre de usuario\n");
            cliente.enviar();

            printf("Escribe la contraseña\n");
            cliente.enviar();
        }
    }

    return accesoConcedido;
}
/****************************************************************************************************/
/****************************************************************************************************/
void menu(Cliente cliente){ //Menu principal, de aca el cliente puede ir al submenu tipo de medicamento, al submenu medicamento, ver toda su actividad o cerrar sesión

    int opcion = 0;

    while(opcion != 4){

        printf("1) Tipo de medicamento \n");
        printf("2) Medicamento \n");
        printf("3) Ver registro de actividades \n");
        printf("4) Cerrar sesión \n");

        printf("Elija una opción: ");

        scanf("%d",&opcion);
        fflush(stdin);

        switch(opcion){
            case 1:
                submenu_TipoDeMedicamento(cliente);
                break;

            case 2:
                submenu_Medicamento(cliente);
                break;

            case 3:
                verActividad(cliente);
                break;

            case 4:
                strcpy(cliente.buffer, "4"); //Informo al servidor de que el cliente cerro sesión
                cliente.enviarInforme();
                cliente.cerrarSocket();
                printf("Vuelva pronto \n");
                system("pause");
                break;

            default:
                system("cls");
                printf("Ingrese una opción valida \n\n");
                break;
        }

    }

}
/****************************************************************************************************/
/****************************************************************************************************/
void submenu_TipoDeMedicamento(Cliente cliente){
    strcpy(cliente.buffer, "1"); //Informo al servidor de que el cliente entro en el submenu tipo de medicamento
    cliente.enviarInforme();
    system("cls");

    int opcion = 0;

    while(opcion != 4){

        printf("1) Crear \n");
        printf("2) Administrar \n");
        printf("3) Ver todo \n");
        printf("4) Volver \n");

        printf("Elija una opción: ");

        scanf("%d",&opcion);
        fflush(stdin);

        switch(opcion){
            case 1:
                strcpy(cliente.buffer, "1"); //Informo al servidor de que el cliente ingresó a la opción Crear
                cliente.enviarInforme();
                system("cls");

                printf("Ingrese el tipo de medicamento que quiere crear \n");
                cliente.enviar();
                cliente.recibir();

                system("pause");
                system("cls");
                break;

            case 2:
                {
                strcpy(cliente.buffer, "2"); //Informo al servidor de que el cliente ingresó a la opcion Administrar
                cliente.enviarInforme();
                system("cls");

                printf("Ingrese el tipo de medicamento que quiere administrar (@ para no aplicar el filtro) \n");
                cliente.enviar();
                printf("Ingrese el estado que quiere administrar (s para activo, n para no activo y @ para no aplicar el filtro) \n");
                cliente.enviar();

                bool mensajeCompleto = false;
                bool hayResultado = true;
                while(!mensajeCompleto){ //While que sirve para imprimir todos la info que mande el servidor
                    string aux = cliente.recibir();
                    if(aux == "No se encontró ningún resultado para los criterios seleccionados \n") hayResultado = false;
                    if(aux == "") mensajeCompleto = true;
                }


                bool IDexistente = false;
                if(hayResultado && !IDexistente){ //El TP pide que si no se encontro ningún resultado que cumpla con los filtros que inmediatamente se vuelva al submenu de tipo de medicamento, por eso existe el boleano hayResultado

                    while(!IDexistente){ //Mientras no se ingrese un ID que exista en el archivo
                        cout << "Ingrese el ID del tipo de medicamento que quiere modificar" << endl;
                        int id;
                        cin >> id;
                        while(cin.fail() ){ //while que sirve para que se ingrese si o si un número
                            cout << "Ingrese un número" <<endl;
                            cin.clear();
                            cin.ignore(256, '\n');
                            cin >> id;
                        }
                        string auxID = to_string(id); //Convierto el int para poder cargarlo en el buffer que es un char array
                        strcpy(cliente.buffer, auxID.c_str() );
                        cliente.enviarInforme();
                        if(cliente.recibir() != "No existe el ID ingresado \n\n") IDexistente = true; //Si no existe el ID ingresado, se repite el while
                    }
                    fflush(stdin);

                    cliente.enviar();
                    printf("Ingrese el nuevo estado del tipo de medicamento(s para activo, n para no activo y @ para no modificarlo) \n");
                    cliente.enviar();

                    printf("\n¿Desea guardar los cambios? s/n \n");
                    char guardar;
                    bool opcionValida = false;

                    while(!opcionValida){ //while para que el cliente ingrese si o si una s o una n
                        cin >> guardar;
                        fflush(stdin);
                        if(guardar != 's' && guardar != 'n') cout << "Ingrese una opción valida" <<endl;
                        else opcionValida = true;
                    }

                    cliente.buffer[0] = guardar; //Informo al servidor si el cliente quiere guardar o no
                    cliente.enviarInforme();

                    if(guardar == 's') cliente.recibir();
                }

                }

                system("pause");
                system("cls");
                break;

            case 3:
                {
                strcpy(cliente.buffer, "3"); //Informo al servidor de que el cliente ingreso a la opcion Ver todo
                cliente.enviarInforme();
                system("cls");

                bool mensajeCompleto = false;
                while(!mensajeCompleto){ //While que sirve para imprimir todos la info que mande el servidor
                    if(cliente.recibir() == "") mensajeCompleto = true;
                }

                }
                system("pause");
                system("cls");
                break;

            case 4:
                strcpy(cliente.buffer, "4"); //Informo al servidor de que el cliente vuelve al menu principal
                cliente.enviarInforme();
                system("cls");
                break;

            default: //En caso de que no se ingrese una de las opciones del submenu
                system("cls");
                printf("Ingrese una opción valida \n\n");
                break;
        }

    }

}
/****************************************************************************************************/
/****************************************************************************************************/
void submenu_Medicamento(Cliente cliente){

    strcpy(cliente.buffer, "2"); //Informo al servidor de que el cliente entro en el submenu medicamento
    cliente.enviarInforme();
    system("cls");

    int opcion = 0;

    while(opcion != 4){

        printf("1) Crear \n");
        printf("2) Administrar \n");
        printf("3) Ver todo \n");
        printf("4) Volver \n");

        printf("Elija una opción: ");

        scanf("%d",&opcion);
        fflush(stdin);

        switch(opcion){
            case 1:
                {
                strcpy(cliente.buffer, "1"); //Informo al servidor de que el cliente ingresó a la opción Crear
                cliente.enviarInforme();
                system("cls");

                bool procede = false;
                printf("Ingrese el CODIGO del medicamento que quiere crear \n");
                while(!procede) {
                    cliente.enviar();
                    if(cliente.recibir() == "\n") procede = true; //Mientras el servidor no me diga que el codigo es valido y no se repite no salgo del while
                }

                procede = false;
                while(!procede){
                    printf("Ingrese el NOMBRE del medicamento \n");
                    cliente.enviar();

                    bool procede2 = false;
                    while(!procede2){
                        printf("Ingrese el TIPO del medicamento a crear \n");
                        cliente.enviar();
                        if(cliente.recibir() == "") procede2 = true;
                    }

                    if(cliente.recibir() == "\n") procede = true; //Mientras el servidor no me diga que la dupla nombre-tipo no se repite no salgo del while
                }

                printf("Ingrese la DROGA del medicamento \n"); //Droga no pide ningun chequeo
                cliente.enviar();

                cliente.recibir();
                }

                system("pause");
                system("cls");
                break;


            case 2:
                {
                strcpy(cliente.buffer, "2"); //Informo al servidor de que el cliente ingresó a la opción Administrar
                cliente.enviarInforme();
                system("cls");

                printf("Ingrese el nombre del medicamento que quiere administrar (@ para no aplicar el filtro) \n");
                cliente.enviar();
                printf("Ingrese el tipo de medicamento que quiere administrar (@ para no aplicar el filtro) \n");
                cliente.enviar();

                bool mensajeCompleto = false;
                bool hayResultado = true;
                while(!mensajeCompleto){ //While que sirve para imprimir todos la info que mande el servidor
                    string aux = cliente.recibir();
                    if(aux == "No se encontró ningún resultado para los criterios seleccionados \n") hayResultado = false;
                    if(aux == "") mensajeCompleto = true;
                }

                bool IDexistente = false;
                if(hayResultado && !IDexistente){ //El TP pide que si no se encontro ningún resultado que cumpla con los filtros que inmediatamente se vuelva al submenu de tipo de medicamento, por eso existe el boleano hayResultado

                    while(!IDexistente){ //Mientras no se ingrese un ID que exista en el archivo
                        cout << "Ingrese el ID del medicamento que quiere modificar/eliminar" << endl;
                        int id;
                        cin >> id;
                        while(cin.fail() ){  //while que sirve para que se ingrese si o si un número
                            cout << "Ingrese un número" <<endl;
                            cin.clear();
                            cin.ignore(256, '\n');
                            cin >> id;
                        }

                        string auxID = to_string(id); //Convierto el int para poder cargarlo en el buffer que es un char array
                        strcpy(cliente.buffer, auxID.c_str() );
                        cliente.enviarInforme();
                        if(cliente.recibir() != "El medicamento especificado no existe \n\n") IDexistente = true; //Si no existe el ID ingresado, se repite el while
                    }
                    fflush(stdin);

                    printf("¿Quiere modificar o eliminar? \n1)Modificar \n2)Eliminar \n");
                    int numero;
                    cin >> numero;
                    fflush(stdin);
                    while(cin.fail() || numero != 1 && numero != 2){ // while para que el cliente ingrese si o si el 1 o el 2
                        cout << "Ingrese un número/opción valida" <<endl;
                        if(cin.fail() ){
                            cin.clear();
                            //cin.ignore(256, '\n');
                        }
                        cin >> numero;
                        fflush(stdin);
                        cout << "Ingresado: " << numero << endl;
                    }


                    if(numero == 1){ //Si el cliente ingreso el 1, informo al servidor de que el cliente quiere modificar el medicamento
                    cliente.buffer[0] = '1';
                    cliente.enviarInforme();

                    printf("\nIngresa el nuevo codigo del medicamento (@ para no modificar este campo) \n");
                    bool codigoValido = false;
                    while(!codigoValido){
                        cliente.enviar();
                        if(cliente.recibir() == "\n") codigoValido = true; //Mientras el cliente no ingrese un codigo valido y que no este repetido no se sale del while
                    }


                    bool duplaValida = false;
                    while(!duplaValida){
                        printf("Ingresa el nuevo nombre del medicamento (@ para no modificar este campo) \n");
                        cliente.enviar();

                        bool procede = false;
                        while(!procede){
                            printf("\nIngresa el nuevo tipo del medicamento (@ para no modificar este campo) \n");
                            cliente.enviar();
                            if(cliente.recibir() == "\n") procede = true;
                        }

                        if(cliente.recibir() == "") duplaValida = true; //Mientras el cliente no ingrese una dupla que no este repetida no se sale del while
                    }

                    printf("Ingresa la nueva droga del medicamento (@ para no modificar este campo) \n");
                    cliente.enviar(); //Droga no pide ningún chequeo así que se envía directamente

                    printf("\n¿Desea guardar los cambios/eliminar el registro? s/n \n");
                    char guardar;
                    bool opcionValida = false;

                    while(!opcionValida){ //while para que el cliente ingrese si o si la s o la n
                        cin >> guardar;
                        fflush(stdin);
                        if(guardar != 's' && guardar != 'n') cout << "Ingrese una opción valida" <<endl;
                        else opcionValida = true;
                    }

                    cliente.buffer[0] = guardar; //Informo al servido si el cliente quiere guardar los datos modificados o no
                    cliente.enviarInforme();

                    if(guardar == 's') cliente.recibir();
                }
                if(numero == 2){ //Si el cliente ingreso el 2, informo al servidor de que el cliente quiere eliminar el medicamento
                    cliente.buffer[0] = '2';
                    cliente.enviarInforme();

                    printf("\n¿Desea guardar los cambios/eliminar el registro? s/n \n");
                    char guardar;
                    bool opcionValida = false;

                    while(!opcionValida){ //while para que el cliente ingrese si o si la s o la n
                        cin >> guardar;
                        fflush(stdin);
                        if(guardar != 's' && guardar != 'n') cout << "Ingrese una opción valida" <<endl;
                        else opcionValida = true;
                    }

                    cliente.buffer[0] = guardar; //Informo al servido si el cliente quiere proceder con la eliminación
                    cliente.enviarInforme();
                }


                }

                }

                system("pause");
                system("cls");
                break;

            case 3:
                {
                strcpy(cliente.buffer, "3"); //Informo al servidor de que el cliente ingreso a la opción Ver todo
                cliente.enviarInforme();
                system("cls");

                bool mensajeCompleto = false;
                while(!mensajeCompleto){ //While que sirve para imprimir todos la info que mande el servidor
                    if(cliente.recibir() == "") mensajeCompleto = true;
                }

                }
                system("pause");
                system("cls");
                break;

            case 4:
                strcpy(cliente.buffer, "4"); //Informo al servidor que el cliente volvio al menu principal
                cliente.enviarInforme();
                system("cls");
                break;

            default:
                system("cls");
                printf("Ingrese una opción valida \n\n");
                break;
        }

    }

}
/****************************************************************************************************/
/****************************************************************************************************/
void verActividad(Cliente cliente){

    strcpy(cliente.buffer, "3"); //Informo al servidor de que el cliente quiere ver toda su actividad
    cliente.enviarInforme();
    system("cls");

    FILE *aux;

    bool archivoExiste = false;
    if(aux = fopen("Mi_actividad.log","r") ) { //Intento abrir el archivo Mi_actividad, el archivo que contiene la actividad del cliente
        archivoExiste = true;                  //Si se pudo abrir el archivo es porque existe
        fclose(aux);
    }

    if(archivoExiste) remove("Mi_actividad.log"); //Si existe el archivo Mi_actividad, lo elimino

    ofstream archivo;
    archivo.open("Mi_actividad.log", ios::app); //Abro el archivo en modo escritura

    string linea = "";
    while(linea != " "){
        linea = cliente.recibir(); //El servidor me envía la info de la actividad del cliente y la escribo en el archivo
        archivo << linea;
    }

    archivo.close(); //Cierro el archivo

    system("pause");
    system("cls");
}
/****************************************************************************************************/
/****************************************************************************************************/
