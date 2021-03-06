#include "Servidor.h"
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <cstring>    //Para convertir string a char array
#include <cstdio>     //Para renombrar archivos
#include <filesystem> //Para eliminar archivos

typedef struct{

    int id;
    char tipo[20];      //tambi?n llamado denominaci?n
    bool activo;

} tipoMedicamento;

typedef struct{

    int id;
    char codigo[15];
    char nombre[20];    //Nombre comercial
    char droga[20];
    char tipo[20];      //tambi?n llamado denominaci?n

} medicamento;

using namespace std;

string nombreArchivoCliente; //Variable global para usar en todos los metodos y actualizar el registro del cliente
/*******************************************************************************************************************/
string fechaHoraActual(){

    char tiempoYFecha[20];
    time_t tiempo = time(NULL);
    struct tm tiempoMaquina = *localtime(&tiempo);
    sprintf(tiempoYFecha, "%d-%d-%d_%d:%d: ", tiempoMaquina.tm_year+1900, tiempoMaquina.tm_mon, tiempoMaquina.tm_mday, tiempoMaquina.tm_hour, tiempoMaquina.tm_min); //Guardo la fecha y hora actual. Este metodo solo acepta char array

    string aux = tiempoYFecha; //Paso los datos a un string porque los string son m?s faciles de manejar
    return aux;
}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
void registrarInicioServidor(int puerto){
        string tiempoYFecha = fechaHoraActual();

        ofstream archivoServidor;
        archivoServidor.open("server.log", ios::app); //Con ios::app escribe al final del archivo y si no existe lo crea
        archivoServidor << tiempoYFecha << "==============================" << endl;
        archivoServidor << tiempoYFecha << "========Inicia Servidor=======" << endl;
        archivoServidor << tiempoYFecha << "==============================" << endl;
        archivoServidor << tiempoYFecha << "Socket creado. Puerto de escucha: " << puerto << endl;
        archivoServidor << tiempoYFecha << "==============================" << endl;
        archivoServidor.close();
}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
boolean chequearUsuarioYContrasenia(string usuarioIngresado, string contraseniaIngresada){

    FILE *archivo_txt;
    archivo_txt = fopen("Usuarios.txt","r");
    if(archivo_txt == NULL){
        printf("No se pudo abrir el registro de usuarios \n");
        return 0;
    }

    char usuario[12];
    char contrasenia[12];
    boolean usuarioCompleto = false;
    char caracter = NULL;

    int i = 0;  //Auxiliar para llenar las variables

    bool datosValidos = false;
    while(!feof(archivo_txt) && !datosValidos){
        caracter = fgetc(archivo_txt); //Leo caracter por caracter del archivo y avanza al siguiente caracter en la proxima iteraci?n

        //Me fijo el usuario en el txt
        if(caracter != ';' && !usuarioCompleto){
            usuario[i] = caracter;
            i++;
            memset(contrasenia,'\0',sizeof(contrasenia) );
        }

        //Si ya termine de cargar los caracteres del usuario, paro y empiezo a llenar los caracteres de la contrase?a
        if(caracter == ';'){
            usuarioCompleto = true;
            i = 0;
        }

        //Me fijo la contrase?a en el txt
        if(usuarioCompleto && caracter != ';' && caracter != '\n'){
            contrasenia[i] = caracter;
            i++;
        }

        //Si ya termine de leer la linea, comparo si los datos ingresados por el cliente son correctos, y si no es as? paso a la siguiente linea
        if(caracter == '\n'){ //Falta comparaci?n
            usuarioCompleto = false;
            i = 0;

            //Si usuario y contrase?a coinciden se acaba el while
            if(usuario == usuarioIngresado && contrasenia == contraseniaIngresada) datosValidos = true;

            //Vac?o los string
            memset(usuario,'\0', sizeof(usuario) );
            memset(contrasenia,'\0',sizeof(contrasenia) );
        }

    }

    fclose(archivo_txt);

    return datosValidos;
}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
boolean login(Servidor servidor){
    int intentos = 1;

    bool loginValido = false;
    while(intentos <= 3){

        printf("Intento numero: %d \n", intentos); //Numero de veces que el usuario intenta loguearse

        string usuarioIngresado = servidor.recibir();       //Datos que ingresa el cliente
        string contraseniaIngresada = servidor.recibir();

        bool usuarioValido = chequearUsuarioYContrasenia(usuarioIngresado, contraseniaIngresada);

        if(usuarioValido){
            strcpy(servidor.buffer, "Acceso concedido\n");
            servidor.enviar();
            intentos = 5;   //Si el usuario y contrase?a son correctos, salgo del loop
            loginValido = true;

            string tiempoYFecha = fechaHoraActual(); //Registro la actividad en el .log del servidor y del cliente

            ofstream archivoServidor;
            archivoServidor.open("server.log", ios::app);
            archivoServidor << tiempoYFecha << "==============Inicia sesion================" << endl;
            archivoServidor << tiempoYFecha << "========Usuario: " << usuarioIngresado << "========" << endl;
            archivoServidor.close();

            ofstream archivoCliente;
            nombreArchivoCliente = usuarioIngresado + ".log";
            archivoCliente.open(nombreArchivoCliente, ios::app);
            archivoCliente << tiempoYFecha << "==============Inicia sesion================" << endl;
            archivoCliente << tiempoYFecha << "========Usuario: " << usuarioIngresado << "========" << endl;
            archivoCliente.close();
        }

        if(!usuarioValido && intentos <=2){
            strcpy(servidor.buffer, "Usuario o contrase?a incorrecto\n\n");
            servidor.enviar();
        }

        if(!usuarioValido && intentos ==3){//Al tercer intento fallido se lo expulsa al cliente
            strcpy(servidor.buffer, "Se super? la cantidad m?xima de intentos de ingreso\n");
            servidor.enviar();
            servidor.cerrarSocket();
        }

        intentos++;
    }

    return loginValido;
}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
void atenderTipoMedicamento(Servidor servidor){

    char opcion = ' ';
    while(opcion != '4'){ //Si el cliente elige la opci?n 4 se regresa al menu principal

        opcion = servidor.recibirInforme();

        switch(opcion){
            case '1':   //El cliente eligi? crear
                    crearTipoMedicamento(servidor);
                    break;

            case '2':   //El cliente eligi? administrar
                    administarTipoMedicamento(servidor);
                    break;

            case '3':   //El cliente eligi? ver toda la info
                    verTodoTipoMedicamento(servidor);
                    break;

            default: break;
        }

    }

}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
void crearTipoMedicamento(Servidor servidor){

    string tipo = servidor.recibir(); //Tipo de medicamento que quiere crear el cliente

    tipoMedicamento t;

    FILE *archivo;
    archivo = fopen("Datos_Tipos_De_Medicamentos.dat", "rb"); //Abro el archivo en modo lectura
    fread(&t, sizeof(tipoMedicamento), 1, archivo);

    bool existe = false;
    string mensaje; //Mensaje para informar al cliente

    srand (time(NULL));
    int IDrandom = rand() % 100 + 1; //Este numero sera el ID del tipo de medicamento en caso de no existir (un n?mero del 1 al 100)
    vector<int> IDexistentes;        //Vector para almacenar los ID que existen en el archivo y as? ,cuando se cree el tipo de medicamento, chequear que no tenga ID repetido

    int i = 1;
    while(!feof(archivo) && !existe){

        string auxTipo = t.tipo;

        //Chequeo si ya existe el tipo de medicamento
        if(convertirAMayusculas(tipo) == convertirAMayusculas(auxTipo) ){ //Si existe el tipo de medicamento, le envi? un mensaje al cliente informandole que ya existe

            mensaje = "El tipo de medicamento " + tipo + " ya existe, por favor ingrese otra denominaci?n. \n";
            existe = true; //Lo coloco en true para no recorrer el resto del archivo
        }

        i++;                            //Almaceno los ID de los tipos de medicamentos
        IDexistentes.push_back(t.id);
        IDexistentes.resize(i);

        fread(&t, sizeof(tipoMedicamento), 1, archivo);
    }

    fclose(archivo); //Cierro el archivo que estaba en modo lectura


    //Si ya existe el tipo de medicamento que se quiere crear, se vuelve al submenu de tipo de medicamento
    //Si no existe el tipo de medicmento, se procede con el codigo de a continuaci?n
    if(!existe) {

        //Con el vector en donde almacene los ID chequeo si ya existe el ID generado al azar
        for(int x=0; x<IDexistentes.size(); x++){

            if(IDrandom == IDexistentes[x]){ //Si ya existe el codigo generado al azar
                IDrandom = rand() % 100 + 1; //Genero otro ID al azar entre 1 y 100
                x = 0;                       //Y vuelvo al principio del vector para poder comparar con todos los ID existentes
            }

        }


        //Guardo todos los datos en una estructura del tipo de medicamento
        t.id = IDrandom;
        strcpy(t.tipo, tipo.c_str() );
        t.activo = true; //Cuando se crea un tipo de medicamento, el activo siempre esta en true

        archivo = fopen("Datos_Tipos_De_Medicamentos.dat", "ab"); //Abro el archivo en modo escritura y guardo el tipo de medicamento creado
        fwrite(&t, sizeof(tipoMedicamento), 1, archivo);
        fclose(archivo);

        mensaje = "El ID del tipo de medicamento creado es " + to_string(IDrandom) + "\n";


        string tiempoYFecha = fechaHoraActual(); //Registro la actividad en el .log del servidor y del cliente

        ofstream archivoServidor;
        archivoServidor.open("server.log", ios::app);
        archivoServidor << tiempoYFecha << "========ID: " << IDrandom << " - Tipo Medicamento - Alta=======" << endl;
        archivoServidor.close();

        ofstream archivoCliente;
        archivoCliente.open(nombreArchivoCliente, ios::app);
        archivoCliente << tiempoYFecha << "========ID: " << IDrandom << " - Tipo Medicamento - Alta=======" << endl;
        archivoCliente.close();
    }


    strcpy(servidor.buffer,  mensaje.c_str() ); //Informo al cliente
    servidor.enviar();
}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
string convertirAMayusculas(string s){

    for(int i=0; i<s.length(); i++){
        s[i] = toupper(s[i]);
    }

    return s;
}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
void administarTipoMedicamento(Servidor servidor){

    //1? Parte: Busqueda con filtros
    string tipo = servidor.recibir();   //Tipo que busca el cliente
    string activo = servidor.recibir(); //Activo que busca el cliente

    string mensaje;
    bool existe = false;
    vector<int> IDexistentes;   //Vector para almacenar los ID que existen en el archivo
    int i = 1;

    tipoMedicamento t;

    FILE *archivo;
    archivo = fopen("Datos_Tipos_De_Medicamentos.dat", "rb"); //Abro archivo en modo lectura
    fread(&t, sizeof(tipoMedicamento), 1, archivo);

    while(!feof(archivo) ){

        //Si solo se busca por tipo
        if(tipo != "@" && activo == "@"){

            bool filtro_tipo_coinciden = true;
            for(int i = 0; i<tipo.length(); i++){
                string aux1(1, tipo[i]);
                string aux2(1, t.tipo[i]);
                if(convertirAMayusculas(aux1) != convertirAMayusculas(aux2) ) filtro_tipo_coinciden = false; //Si el tipo ingresado por el cliente no coincide con el tipo de esta lectura (o por los menos con las primeras letras) se pone en false
            }

            //Solo los tipos que coinciden (o por lo menos coinciden en las primeras letras) se muestran en pantalla
            if(filtro_tipo_coinciden){
                string auxActivo;   //Paso el boleano a un string para poder mostrarlo en el mensaje
                if(t.activo)auxActivo = "true";
                else auxActivo = "false";

                mensaje = "ID: " + to_string(t.id) + "\n" + "Tipo de medicamento: " + t.tipo + "\n" + "Activo: " + auxActivo + "\n\n";

                strcpy(servidor.buffer,  mensaje.c_str() ); //Informo al cliente del tipo de medicamento que cumple con el filtro
                servidor.enviar();
                existe = true;
            }

        }


        //Si solo se busca por activo
        if(tipo == "@" && activo != "@"){

            //Si se busca solo los que tengan el activo en true
            if(activo == "s" && t.activo == 1){
                string auxActivo;   //Paso el boleano a un string para poder mostrarlo en el mensaje
                if(t.activo)auxActivo = "true";
                else auxActivo = "false";

                mensaje = "ID: " + to_string(t.id) + "\n" + "Tipo de medicamento: " + t.tipo + "\n" + "Activo: " + auxActivo + "\n\n";

                strcpy(servidor.buffer,  mensaje.c_str() );  //Informo al cliente del tipo de medicamento que cumple con el filtro
                servidor.enviar();
                existe = true;
            }

            //Si se busca solo los que tengan el activo en false
            if(activo == "n" && t.activo == 0){
                string auxActivo;   //Paso el boleano a un string para poder mostrarlo en el mensaje
                if(t.activo)auxActivo = "true";
                else auxActivo = "false";

                mensaje = "ID: " + to_string(t.id) + "\n" + "Tipo de medicamento: " + t.tipo + "\n" + "Activo: " + auxActivo + "\n\n";

                strcpy(servidor.buffer,  mensaje.c_str() );  //Informo al cliente del tipo de medicamento que cumple con el filtro
                servidor.enviar();
                existe = true;
            }

        }


        //Si se busca por tipo y por activo
        if(tipo != "@" && activo != "@"){

            //Primero me fijo que se cumpla con el filtro de tipo. Si de entrada no se cumple con el filtro de tipo, salteo el filtro de activo
            bool filtro_tipo_coinciden = true;
            for(int i = 0; i<tipo.length(); i++){
                string aux1(1, tipo[i]);
                string aux2(1, t.tipo[i]);
                if(convertirAMayusculas(aux1) != convertirAMayusculas(aux2) ) filtro_tipo_coinciden = false;
            }

            bool filtro_activo_coinciden = false;

            if(filtro_tipo_coinciden && activo == "s" && t.activo == 1){ //Si cumple con el filtro de tipo y el activo que se busca es true
                filtro_activo_coinciden = true;
            }

            if(filtro_tipo_coinciden && activo == "n" && t.activo == 0){ //Si cumple con el filtro de tipo y el activo que se busca es false
                filtro_activo_coinciden = true;
            }

            if(filtro_tipo_coinciden && filtro_activo_coinciden){ //Si ambos filtros se cumplen, informo al cliente del tipo de medicamento que cumple con el filtro
                string auxActivo;   //Paso el boleano a un string para poder mostrarlo en el mensaje
                if(t.activo)auxActivo = "true";
                else auxActivo = "false";

                mensaje = "ID: " + to_string(t.id) + "\n" + "Tipo de medicamento: " + t.tipo + "\n" + "Activo: " + auxActivo + "\n\n";

                strcpy(servidor.buffer,  mensaje.c_str() );
                servidor.enviar();
                existe = true;
            }

        }

        //Si no se busca por tipo o por activo significa que el cliente quiere ver todas los tipos de medicina
        if(tipo == "@" && activo == "@"){
            string auxActivo;   //Paso el boleano a un string para poder mostrarlo en el mensaje
            if(t.activo)auxActivo = "true";
            else auxActivo = "false";

            mensaje = "ID: " + to_string(t.id) + "\n" + "Tipo de medicamento: " + t.tipo + "\n" + "Activo: " + auxActivo + "\n\n";

            strcpy(servidor.buffer,  mensaje.c_str() );
            servidor.enviar();
            existe = true;
        }


        i++;                            //Sin importar si cumplen o no con el filtro, almaceno los ID. El cliente debe poder modificar el tipo de medicamento que desee, incluso si es uno que no muestre en pantalla
        IDexistentes.push_back(t.id);
        IDexistentes.resize(i);

        fread(&t, sizeof(tipoMedicamento), 1, archivo); //Siguiente lectura
    }


    if(!existe){ //Si ning?n tipo de medicamento cumple los filtros
        strcpy(servidor.buffer,  "No se encontr? ning?n resultado para los criterios seleccionados \n");
        servidor.enviar();
    }

    strcpy(servidor.buffer,  "");   //Para avisar al cliente que ya no espere por m?s info
    servidor.enviar();
    fclose(archivo);


    //2? Parte: El cliente ingresa el ID y los nuevos valores para denominaci?n y activo
    int id;
    if(existe){ //Si no se encontro ninguna coincidencia con los filtros, se vuelve al submenu de los tipos de medicamentos

        bool IDvalida = false;

        while(!IDvalida){ //Mientras el cliente no ingrese un ID que exista

            string idIngresada = servidor.recibir();
            int auxID = stoi(idIngresada); //El cliente ingresa la ID del tipo de medicamento a modificar
            string mensaje;
            for(int x=0; x<IDexistentes.size(); x++){ //Chequeo si existe el ID ingresado por el cliente
                if(auxID == IDexistentes[x]){ //Si existe el ID, le pide al cliente que ingrese la nueva denominaci?n del tipo de medicamento
                    mensaje = "Ingrese la nueva denominaci?n del tipo de medicamento (@ para no modificar)\n";
                    IDvalida = true;
                    id = auxID;
                }
            }

            if(!IDvalida) { //Si no existe, le informo al cliente que el ID ingresado no existe
                mensaje = "No existe el ID ingresado \n\n";
            }

            strcpy(servidor.buffer, mensaje.c_str() );
            servidor.enviar();
        }

        string cambioTipo = servidor.recibir();   //El cliente ingresa el nuevo tipo/denominaci?n del tipo de medicamento
        string cambioActivo = servidor.recibir(); //El cliente ingresa el nuevo activo del tipo de medicamento

        FILE *auxArchivo;
        auxArchivo = fopen("Archivo_Auxiliar.dat", "ab");   //Creo otro archivo binario donde se guardan los datos originales modificados
        archivo = fopen("Datos_Tipos_De_Medicamentos.dat", "rb");

        tipoMedicamento auxT;

        //Si se modifica solo la denominacion del tipo de medicamento
        if(cambioTipo != "@" && cambioActivo == "@"){

            fread(&t, sizeof(tipoMedicamento), 1, archivo); //Primera lectura

            while(!feof(archivo) ){

                if(id == t.id) { //Si el ID que ingreso el cliente y el ID de esta lectura coinciden

                    //Guardo los datos modificados en una estructura auxiliar
                    auxT.id = t.id;
                    strcpy(auxT.tipo, cambioTipo.c_str() );
                    auxT.activo = t.activo;

                    fwrite(&auxT, sizeof(tipoMedicamento), 1, auxArchivo); //y lo escribo en el archivo binario auxiliar
                }
                else fwrite(&t, sizeof(tipoMedicamento), 1, auxArchivo); //Si no coinciden los ID, escribo los datos en el archivo binario auxiliar sin que sean modificados

                fread(&t, sizeof(tipoMedicamento), 1, archivo); //Siguiente lectura
            }

        }

        //Si se modifica solo el activo del tipo de medicamento
        if(cambioTipo == "@" && cambioActivo != "@"){

            fread(&t, sizeof(tipoMedicamento), 1, archivo); //Primera lectura

            while(!feof(archivo) ){

                if(id == t.id) { //Si el ID que ingreso el cliente y el ID de esta lectura coinciden

                    //Guardo los datos modificados en una estructura auxiliar
                    auxT.id = t.id;
                    strcpy(auxT.tipo, t.tipo);
                    bool auxActivo;
                    if(cambioActivo == "s") auxActivo = true;
                    if(cambioActivo == "n") auxActivo = false;
                    auxT.activo = auxActivo;

                    fwrite(&auxT, sizeof(tipoMedicamento), 1, auxArchivo); //y lo escribo en el archivo binario auxiliar
                }
                else fwrite(&t, sizeof(tipoMedicamento), 1, auxArchivo);  //Si no coinciden los ID, escribo los datos en el archivo binario auxiliar sin que sean modificados

                fread(&t, sizeof(tipoMedicamento), 1, archivo); //Siguiente lectura
            }

        }

        //Si se modifica la denominacion y el activo del tipo de medicamento
        if(cambioTipo != "@" && cambioActivo != "@"){

            fread(&t, sizeof(tipoMedicamento), 1, archivo); //Primera lectura

            while(!feof(archivo) ){

                if(id == t.id) { //Si el ID que ingreso el cliente y el ID de esta lectura coinciden

                    //Guardo los datos modificados en una estructura auxiliar
                    auxT.id = t.id;
                    strcpy(auxT.tipo, cambioTipo.c_str() );
                    bool auxActivo;
                    if(cambioActivo == "s") auxActivo = true;
                    if(cambioActivo == "n") auxActivo = false;
                    auxT.activo = auxActivo;

                    fwrite(&auxT, sizeof(tipoMedicamento), 1, auxArchivo); //y lo escribo en el archivo binario auxiliar
                }
                else fwrite(&t, sizeof(tipoMedicamento), 1, auxArchivo); //Si no coinciden los ID, escribo los datos en el archivo binario auxiliar sin que sean modificados

                fread(&t, sizeof(tipoMedicamento), 1, archivo); //Siguiente lectura
            }


        }


        //Si no se modifica nada
        if(cambioTipo == "@" && cambioActivo == "@"){

            fread(&t, sizeof(tipoMedicamento), 1, archivo); //Primera lectura

            while(!feof(archivo) ){

                if(id == t.id){
                    //Guardo los datos en una estructura auxiliar
                    auxT.id = t.id;
                    strcpy(auxT.tipo, t.tipo);
                    auxT.activo = t.activo;
                }

                fwrite(&t, sizeof(tipoMedicamento), 1, auxArchivo); //Si no coinciden los ID, escribo los datos en el archivo binario auxiliar sin que sean modificados
                fread(&t, sizeof(tipoMedicamento), 1, archivo); //Siguiente lectura
            }

        }


        fclose(archivo);
        fclose(auxArchivo);

        char guardar = servidor.recibirInforme(); //El cliente me indica si quiere que se guarden los cambios o no

        if(guardar == 's'){ //Si se quiere guardar los cambios
            remove("Datos_Tipos_De_Medicamentos.dat");  //Elimino el archivo con los datos viejos
            rename("Archivo_Auxiliar.dat", "Datos_Tipos_De_Medicamentos.dat"); //y el archivo con los datos nuevos lo renombro con el nombre del original

            //Muestro al cliente como quedo los datos modificados
            string auxActivo;   //Paso el boleano a un string para poder mostrarlo en el mensaje
            if(auxT.activo)auxActivo = "true";
            else auxActivo = "false";

            mensaje = "ID: " + to_string(auxT.id) + "\n" + "Tipo de medicamento: " + auxT.tipo + "\n" + "Activo: " + auxActivo + "\n\n";

            strcpy(servidor.buffer,  mensaje.c_str() );
            servidor.enviar();

            string tiempoYFecha = fechaHoraActual();    //Registro la actividad en el .log del servidor y del cliente

            ofstream archivoServidor;
            archivoServidor.open("server.log", ios::app);
            archivoServidor << tiempoYFecha << "========ID: " << id << " - Tipo Medicamento - Modificado=======" << endl;
            archivoServidor.close();

            ofstream archivoCliente;
            archivoCliente.open(nombreArchivoCliente, ios::app);
            archivoCliente << tiempoYFecha << "========ID: " << id << " - Tipo Medicamento - Modificado=======" << endl;
            archivoCliente.close();
        }
        if(guardar == 'n') remove("Archivo_Auxiliar.dat"); //Si no se quiere guardar, solo elimino el archivo con los datos modificados y me quedo con el archivo con los datos originales

   }

}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
void verTodoTipoMedicamento(Servidor servidor){

    tipoMedicamento t;

    FILE *archivo;
    archivo = fopen("Datos_Tipos_De_Medicamentos.dat", "rb");   //Abro archivo en modo lectura
    fread(&t, sizeof(tipoMedicamento), 1, archivo);             //Primera lectura

    string mensaje;

    while(!feof(archivo) ){

        string auxActivo;   //Es m?s facil para el usuario ver el activo como true o false que como uno o cero
        if(t.activo)auxActivo = "true";
        else auxActivo = "false";

        mensaje = "ID: " + to_string(t.id) + "\n" + "Tipo de medicamento: " + t.tipo + "\n" + "Activo: " + auxActivo + "\n\n";

        strcpy(servidor.buffer,  mensaje.c_str() ); //Envio al cliente la info de la lectura
        servidor.enviar();

        fread(&t, sizeof(tipoMedicamento), 1, archivo); //Siguiente lectura
    }

    strcpy(servidor.buffer,  "" ); //Informo al cliente de que ya no espere m?s info
    servidor.enviar();

    fclose(archivo);


    string tiempoYFecha = fechaHoraActual();    //Registro la actividad en el .log del servidor y del cliente

    ofstream archivoServidor;
    archivoServidor.open("server.log", ios::app);
    archivoServidor << tiempoYFecha << "========Ver todo - Tipo Medicamento=======" << endl;
    archivoServidor.close();

    ofstream archivoCliente;
    archivoCliente.open(nombreArchivoCliente, ios::app);
    archivoCliente << tiempoYFecha << "========Ver todo - Tipo Medicamento=======" << endl;
    archivoCliente.close();

}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
void atenderMedicamento(Servidor servidor){

    char opcion = ' ';
    while(opcion != '4'){  //Si el cliente elige la opci?n 4 se regresa al menu principal

        opcion = servidor.recibirInforme();
        switch(opcion){
            case '1':   //El cliente eligi? crear
                    crearMedicamento(servidor);
                    break;

            case '2':   //El cliente eligi? administrar
                    administarMedicamento(servidor);
                    break;

            case '3':   //El cliente eligi? ver toda la info
                    verTodoMedicamento(servidor);
                    break;

            default: break;
        }

    }

}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
void crearMedicamento(Servidor servidor){

    medicamento m;
    FILE *archivo;
    string mensaje;

    srand (time(NULL));
    int IDrandom = rand() % 100 + 1; //Este numero sera el ID del medicamento en caso de no existir (un n?mero del 1 al 100)
    vector<int> IDexistentes;        //Vector para almacenar los ID que existen en el archivo y as? ,cuando se cree el medicamento, chequear que no tenga ID repetido


    //Creaci?n/modificaci?n: el servidor debe validar que el c?digo de producto sea correcto contrastando su digito verificador

    //Chequeo que el codigo no este repetido y que sea valido
    bool codigoValido = false;
    string codigo = servidor.recibir(); //El cliente ingresa el codigo del nuevo medicamento

    while(!codigoValido){

        archivo = fopen("Datos_Medicamentos.dat", "rb"); //Abro el archivo de medicamentos en modo lectura
        fread(&m, sizeof(medicamento), 1, archivo);      //Primera lectura

        int i = 1;
        bool existe = false;
        bool digitoVerificador = false;

        while(!feof(archivo) && !existe){

            string auxCodigo = m.codigo; //Paso el char array a string

            //Chequeo si ya existe el codigo
            if(convertirAMayusculas(codigo) == convertirAMayusculas(auxCodigo) ){

                existe = true; //Para que no recorra el resto del archivo
                mensaje = "El medicamento con el codigo " + codigo + " ya existe, por favor ingrese otro codigo \n";
                fclose(archivo);
            }

            i++;                            //Almaceno los ID
            IDexistentes.push_back(m.id);
            IDexistentes.resize(i);

            fread(&m, sizeof(medicamento), 1, archivo);
        }

        //Chequeo el digito verificador
        char auxCodigo[15];
        strcpy(auxCodigo, codigo.c_str() ); //Convierto el string a char array

        if(validador_de_codigo(auxCodigo) ) digitoVerificador = true;
        else mensaje = "Codigo no valido, por favor ingrese otro codigo \n";


        if(!existe && digitoVerificador){ //Si el codigo no se repite y el digito verificador es valido, le informo al cliente que ya puede proseguir con los otros atributos
            mensaje = "\n";
            codigoValido = true;
        }

        strcpy(servidor.buffer,  mensaje.c_str() );
        servidor.enviar();

        if(existe || !digitoVerificador){ //Si el codigo se repite o si el digito verificador no es valido, el cliente tiene que introducir un nuevo codigo
            memset(auxCodigo, '\n', sizeof(auxCodigo) );
            codigo = servidor.recibir();
        }

        fclose(archivo);
    }




    //Creaci?n/modificaci?n: el servidor debe validar que la combinaci?n Nombre comercial / Tipo de medicamento no est? presente
    //en otro medicamento ya existente. En caso positivo, responder? con el mensaje:
    //?El medicamento [NOMBRE COMERCIAL ? TIPO MEDICAMENTO] ya existe, por favor ingrese un Nombre Comercial y/o Tipo de medicamento diferente?.

    //EJ: Existe el ibuprofeno en comprimidos y el ibuprofeno para inhalar.
    //El nombre o el tipo se pueden repetir, pero la dupla no

    string nombre = servidor.recibir(); //El cliente ingresa el nombre
    string tipo = servidor.recibir();   //El cliente ingresa el tipo

    tipoMedicamento t;

    //Chequeo que el tipo de medicamento ingresado por el usuario exista
    bool proceder = false;
    while(!proceder){

        archivo = fopen("Datos_Tipos_De_Medicamentos.dat", "rb"); //Abro el archivo de medicamentos en modo lectura
        fread(&t, sizeof(tipoMedicamento), 1, archivo);      //Primera lectura

        bool existeTipo = false;
        while(!feof(archivo) && !existeTipo){

            string auxTipo = t.tipo;
            if(tipo == auxTipo) existeTipo = true;

            fread(&t, sizeof(tipoMedicamento), 1, archivo);      //Primera lectura
        }

        fclose(archivo);

        if(existeTipo) {
            mensaje = "";
            proceder = true;
        }
        if(!existeTipo)mensaje = "No existe el tipo de medicamento \n";

        strcpy(servidor.buffer,  mensaje.c_str() );
        servidor.enviar();

        if(!existeTipo) tipo = servidor.recibir();
    }

    bool duplaValida = false;
    while(!duplaValida){ //Mientras la dupla no sea valida

        string mensaje = "\n";

        archivo = fopen("Datos_Medicamentos.dat", "rb"); //Abro el archivo en modo lectura binaria
        fread(&m, sizeof(medicamento), 1, archivo);

        duplaValida = true;

        while(duplaValida && !feof(archivo) ){
            //Chequeo medicamento por medicamento para saber si ya existe la dupla

            //Chequeo el nombre ingresado por el usuario con el de esta lectura
            string auxNombre = m.nombre; //Paso el char array a string
            bool nombresCoinciden = false;
            if(nombre == auxNombre) nombresCoinciden = true;

            //Chequeo el tipo ingresado por el usuario con el de esta lectura
            string auxTipo = m.tipo;    //Paso el char array a string
            bool tiposCoinciden = false;
            if(tipo == auxTipo) tiposCoinciden = true;

            //Como m (el medicamento) se actualiza a medida que vamos leyendo el archivo, si en la misma lectura salta que ambos booleanos son verdaderos significa que la dupla ya existe
            if(nombresCoinciden && tiposCoinciden) {
                duplaValida = false;
                mensaje = "El medicamento " + nombre + "?" + tipo + " ya existe, por favor ingrese un Nombre Comercial y/o Tipo de medicamento diferente \n";
            }

            fread(&m, sizeof(medicamento), 1, archivo); //Siguiente lectura
        }

        strcpy(servidor.buffer,  mensaje.c_str() );
        servidor.enviar();

        fclose(archivo); //Cierro el archivo. Sirve tanto para si la dupla es valida o no. Si es valida simplemente se cierra y si no es valida sirve para volver a chequear desde el inicio cuando se vuelva a abrir el archivo al principio del while

        if(!duplaValida){ //Si ya existe la dupla, el usuario debe ingresar un nuevo nombre y un nuevo tipo
            nombre = servidor.recibir();
            tipo = servidor.recibir();

            //Chequeo que el tipo de medicamento ingresado por el usuario exista
            bool proceder = false;
            while(!proceder){

                archivo = fopen("Datos_Tipos_De_Medicamentos.dat", "rb"); //Abro el archivo de medicamentos en modo lectura
                fread(&t, sizeof(tipoMedicamento), 1, archivo);      //Primera lectura

                bool existeTipo = false;
                while(!feof(archivo) && !existeTipo){

                    string auxTipo = t.tipo;
                    if(tipo == auxTipo) existeTipo = true;

                    fread(&t, sizeof(tipoMedicamento), 1, archivo);      //Primera lectura
                }

                fclose(archivo);

                if(existeTipo) {
                    mensaje = "";
                    proceder = true;
                }
                if(!existeTipo)mensaje = "No existe el tipo de medicamento \n";

                strcpy(servidor.buffer,  mensaje.c_str() );
                servidor.enviar();

                if(!existeTipo) tipo = servidor.recibir();
            }

        }

    }


    string droga = servidor.recibir(); //La droga no hace falta que chequee si se repite, ya que varios medicamentos pueden estar hechos con la misma droga

    //Con el vector en donde almacene los ID chequeo si ya existe el ID generado al azar
    for(int x=0; x<IDexistentes.size(); x++){

        if(IDrandom == IDexistentes[x]){ //Si ya existe el codigo generado al azar
            IDrandom = rand() % 100 + 1; //Genero otro ID al azar entre 1 y 100
            x = 0;                       //Y vuelvo al principio del vector para poder comparar con todos los ID existentes
        }

    }

    //Guardo los datos en una estructura de medicamento
    m.id = IDrandom;
    strcpy(m.codigo, codigo.c_str() );
    strcpy(m.nombre, nombre.c_str() );
    strcpy(m.droga, droga.c_str() );
    strcpy(m.tipo, tipo.c_str() );

    archivo = fopen("Datos_Medicamentos.dat", "ab"); //Abro el archivo en modo escritura binaria
    fwrite(&m, sizeof(medicamento), 1, archivo);     //escribo el nuevo medicamento
    fclose(archivo);                                 //y lo cierro


    mensaje = "\nEl medicamento se creo con el ID: " + to_string(IDrandom) + "\n";
    strcpy(servidor.buffer,  mensaje.c_str() );
    servidor.enviar();


    string tiempoYFecha = fechaHoraActual();        //Registro la actividad en el .log del servidor y del cliente

    ofstream archivoServidor;
    archivoServidor.open("server.log", ios::app);
    archivoServidor << tiempoYFecha << "========ID: " << IDrandom << " - Medicamento - Alta=======" << endl;
    archivoServidor.close();

    ofstream archivoCliente;
    archivoCliente.open(nombreArchivoCliente, ios::app);
    archivoCliente << tiempoYFecha << "========ID: " << IDrandom << " - Medicamento - Alta=======" << endl;
    archivoCliente.close();
}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
bool validador_de_codigo(char codigo[15]){

/*
El codigo es: XXX-NNNNN-Y
XXX son letras (A-Z)    -> c?digo alfab?tico del producto
NNNNN son n?meros (0-9) -> c?digo num?rico de la regi?n
Y es el d?gito verificador (0-9)
*/

/*
El d?gito verificador se calcula de la siguiente manera:
1) Se suma cada cifra de la regi?n.
2) Si obtengo un n?mero de m?s de 2 d?gitos, repito (vuelvo a sumar cada d?gito del resultado).
3) Se finaliza al llegar a 1 sola cifra, la cual ser? mi d?gito verificador.
*/
    bool coinciden;

    //Calculo el valor de los caracteres
    int digito_1 = codigo[4] - '0';
    int digito_2 = codigo[5] - '0';
    int digito_3 = codigo[6] - '0';
    int digito_4 = codigo[7] - '0';
    int digito_5 = codigo[8] - '0';

    int suma = digito_1 + digito_2 + digito_3 + digito_4 + digito_5;

    if(suma >= 10){ //Si la suma da dos digitos
        int digitoIzquierda = suma/10;
        int digitoDerecha = suma%10;
        suma = digitoIzquierda + digitoDerecha;

        if(suma >= 10){
            int digitoIzquierda = suma/10;
            int digitoDerecha = suma%10;
            suma = digitoIzquierda + digitoDerecha;
        }

    }

    int digito_verificador = codigo[10] - '0';
    if(digito_verificador == suma) coinciden = true;

    //Me fijo si los caracteres son n?meros
    if(!isdigit(codigo[4]) || !isdigit(codigo[5]) || !isdigit(codigo[6]) || !isdigit(codigo[7]) || !isdigit(codigo[8]) || !isdigit(codigo[10]) ){
        coinciden = false;
    }

    return coinciden;
}

/*******************************************************************************************************************/
/*******************************************************************************************************************/
void administarMedicamento(Servidor servidor){

    //1? Parte: Busqueda con filtros
    string nombre = servidor.recibir(); //El cliente ingresa el nombre del medicamento
    string tipo = servidor.recibir();   //El cliente ingresa el tipo del medicamento

    string mensaje;
    bool existe = false;
    vector<int> IDexistentes;
    int i = 1;

    medicamento m;

    FILE *archivo;
    archivo = fopen("Datos_Medicamentos.dat", "rb"); //Abro el archivo en modo lectura
    fread(&m, sizeof(medicamento), 1, archivo);      //Primera lectura

    while(!feof(archivo) ){

        //Si solo se busca por nombre
        if(nombre != "@" && tipo == "@"){

            bool filtro_nombre_coinciden = true;
            for(int i = 0; i<nombre.length(); i++){
                string aux1(1, nombre[i]);
                string aux2(1, m.nombre[i]);
                if(convertirAMayusculas(aux1) != convertirAMayusculas(aux2) ) filtro_nombre_coinciden = false; //Si el nombre ingresado por el cliente no coincide con el nombre de esta lectura (o por los menos con las primeras letras) se pone en false
            }

            //Solo los nombres que coinciden (o por lo menos coinciden en las primeras letras) se muestran en pantalla
            if(filtro_nombre_coinciden){

                mensaje = "ID: " + to_string(m.id) + "\n" + "Codigo: " + m.codigo + "\n" +
                          "Nombre: " + m.nombre + "\n" + "Droga: " + m.droga + "\n" +
                          "Tipo: " + m.tipo + "\n\n";

                strcpy(servidor.buffer,  mensaje.c_str() ); //Informo al cliente del medicamento que coincide con los filtros
                servidor.enviar();
                existe = true;
            }

        }


        //Si solo se busca por tipo
        if(nombre == "@" && tipo != "@"){

            bool filtro_tipo_coinciden = true;
            for(int i = 0; i<tipo.length(); i++){
                string aux1(1, tipo[i]);
                string aux2(1, m.tipo[i]);
                if(convertirAMayusculas(aux1) != convertirAMayusculas(aux2) ) filtro_tipo_coinciden = false; //Si el tipo ingresado por el cliente no coincide con el tipo de esta lectura (o por los menos con las primeras letras) se pone en false
            }

            //Solo los tipos que coinciden (o por lo menos coinciden en las primeras letras) se muestran en pantalla
            if(filtro_tipo_coinciden){

                mensaje = "ID: " + to_string(m.id) + "\n" + "Codigo: " + m.codigo + "\n" +
                          "Nombre: " + m.nombre + "\n" + "Droga: " + m.droga + "\n" +
                          "Tipo: " + m.tipo + "\n\n";

                strcpy(servidor.buffer,  mensaje.c_str() ); //Informo al cliente del medicamento que coincide con los filtros
                servidor.enviar();
                existe = true;
            }

        }


        //Si se busca por nombre y por tipo
        if(nombre != "@" && tipo != "@"){

            //Me fijo si cumple con el filtro de nombre
            bool filtro_nombre_coinciden = true;
            for(int i = 0; i<nombre.length(); i++){
                string aux1(1, nombre[i]);
                string aux2(1, m.nombre[i]);
                if(convertirAMayusculas(aux1) != convertirAMayusculas(aux2)) filtro_nombre_coinciden = false;
            }

            //Me fijo si cumple con el filtro de tipo
            bool filtro_tipo_coinciden = true;
            for(int i = 0; i<tipo.length(); i++){
                string aux1(1, tipo[i]);
                string aux2(1, m.tipo[i]);
                if(convertirAMayusculas(aux1) != convertirAMayusculas(aux2) ) filtro_tipo_coinciden = false;
            }

            //Si se cumplen ambos filtros, informo al cliente del medicamento que cumple los filtros
            if(filtro_nombre_coinciden && filtro_tipo_coinciden){

                mensaje = "ID: " + to_string(m.id) + "\n" + "Codigo: " + m.codigo + "\n" +
                          "Nombre: " + m.nombre + "\n" + "Droga: " + m.droga + "\n" +
                          "Tipo: " + m.tipo + "\n\n";

                strcpy(servidor.buffer,  mensaje.c_str() );
                servidor.enviar();
                existe = true;
            }

        }

        //Si no se busca por nombre o por tipo significa que el cliente quiere ver todas los medicamentos
        if(nombre == "@" && tipo == "@"){

            mensaje = "ID: " + to_string(m.id) + "\n" + "Codigo: " + m.codigo + "\n" +
                          "Nombre: " + m.nombre + "\n" + "Droga: " + m.droga + "\n" +
                          "Tipo: " + m.tipo + "\n\n";

            strcpy(servidor.buffer,  mensaje.c_str() );
            servidor.enviar();
            existe = true;
        }


        i++;                            //Sin importar si cumplen o no con el filtro, almaceno los ID. El cliente debe poder modificar el tipo de medicamento que desee, incluso si es uno que no muestre en pantalla
        IDexistentes.push_back(m.id);
        IDexistentes.resize(i);

        fread(&m, sizeof(medicamento), 1, archivo);  //Siguiente lectura
    }


    if(!existe){ //Si ning?n medicamento cumple con los filtros
        strcpy(servidor.buffer,  "No se encontr? ning?n resultado para los criterios seleccionados \n");
        servidor.enviar();
    }

    strcpy(servidor.buffer,  "");   //Para avisar al cliente que ya no espere por m?s info
    servidor.enviar();
    fclose(archivo);


    //2? Parte: El cliente ingresa el ID y los nuevos valores para nombre, codigo, droga y tipo
   if(existe){ //Si no se encontro ninguna coincidencia con los filtros, se vuelve al submenu de medicamentos


    //Modificaci?n: se validar? que el Id del tipo de medicamento exista, en caso de no existir, el servidor debe
    //informar ?El tipo de medicamento especificado no existe.? y no se guardar?n los cambios.
        bool IDvalida = false;
        int id;

        while(!IDvalida){  //Mientras el cliente no ingrese un ID que exista

            string idIngresada = servidor.recibir();
            int auxID = stoi(idIngresada ); //El cliente ingresa la ID del tipo de medicamento a modificar

            for(int x=0; x<IDexistentes.size(); x++){ //Chequeo si existe el ID ingresado por el cliente
                if(auxID == IDexistentes[x]){ //Si existe, el ID es valido
                    IDvalida = true;
                    id = auxID;
                }
            }

            if(!IDvalida){ //Si no existe, le informo al cliente que el ID ingresado no existe
                strcpy(servidor.buffer, "El medicamento especificado no existe \n\n");
                servidor.enviar();
            }

        }

        strcpy(servidor.buffer, "\n"); //Le informo al cliente que no espere por m?s info
        servidor.enviar();

        char modificarOeliminar = servidor.recibirInforme();
        if(modificarOeliminar == '1'){ //Si el cliente ingresa un 1 es porque quiere modificar

            string codigo;
            bool codigoValido = false;
            bool existeCodigo = true;

            while(!codigoValido || existeCodigo){ //Chequeo si el nuevo codigo para el medicamento ya existe y si es valido

                codigoValido = false;

                string auxCodigo = servidor.recibir(); //Cliente ingresa el codigo
                char auxCodigo2[15];
                strcpy(auxCodigo2, auxCodigo.c_str() ); //Paso el string a char array

                if(validador_de_codigo(auxCodigo2) ) { //Chequeo si el codigo es valido
                    codigoValido = true;
                }

                memset(auxCodigo2, '\0', sizeof(auxCodigo2) ); //Valido el codigo

                if(!codigoValido){
                    mensaje = "Codigo no valido, por favor ingrese otro codigo \n";
                }

                if(codigoValido){ //Si el codigo es valido, chequeo si ya existia o no
                    archivo = fopen("Datos_Medicamentos.dat", "rb");
                    fread(&m, sizeof(medicamento), 1, archivo);

                    existeCodigo = false;
                    while(!feof(archivo) && !existeCodigo){

                        string aux = m.codigo;

                        //Chequeo si ya existe el codigo
                        if(convertirAMayusculas(auxCodigo) == convertirAMayusculas(aux) ){

                            existeCodigo = true; //Para que no recorra el resto del archivo
                            mensaje = "El medicamento con el codigo " + codigo + " ya existe, por favor ingrese otro codigo \n";
                        }

                        fread(&m, sizeof(medicamento), 1, archivo);
                    }

                    if(!existeCodigo) mensaje = "\n";

                    fclose(archivo);
                }

                if(codigoValido && !existeCodigo) codigo = auxCodigo; //Si el codigo es valido y no se repite
                if(auxCodigo == "@") { //Si el usuario no quiere modificar el codigo simplemente salgo del while
                    codigo = auxCodigo;
                    mensaje = "\n";
                    codigoValido = true;
                    existeCodigo = false;
                }

                strcpy(servidor.buffer, mensaje.c_str() );
                servidor.enviar();
            }


            bool duplaValida = false;
            while(!duplaValida){

                nombre = servidor.recibir(); //El cliente ingresa el nuevo nombre del medicamento
                tipo = servidor.recibir();   //El cliente ingresa el nuevo tipo del medicamento

                //En caso de que el usuario quiera modificar el tipo, chequeo que el tipo de medicamento ingresado por el usuario exista
                bool proceder = false;
                while(!proceder && tipo != "@"){

                    tipoMedicamento t;

                    archivo = fopen("Datos_Tipos_De_Medicamentos.dat", "rb"); //Abro el archivo de medicamentos en modo lectura
                    fread(&t, sizeof(tipoMedicamento), 1, archivo);      //Primera lectura

                    bool existeTipo = false;
                    while(!feof(archivo) && !existeTipo){

                        string auxTipo = t.tipo;
                        if(tipo == auxTipo) existeTipo = true;

                        fread(&t, sizeof(tipoMedicamento), 1, archivo);
                    }

                    fclose(archivo);

                    if(existeTipo) {
                        mensaje = "\n";
                        proceder = true;
                    }
                    if(!existeTipo)mensaje = "No existe el tipo de medicamento \n";

                    strcpy(servidor.buffer,  mensaje.c_str() );
                    servidor.enviar();

                    if(!existeTipo) tipo = servidor.recibir();
                }

                //Si el usuario no quiere modificar el tipo, hago que el cliente salga del loop para ingresar un tipo que exista
                if(tipo == "@"){
                    mensaje = "\n";
                    strcpy(servidor.buffer,  mensaje.c_str() );
                    servidor.enviar();
                }


                string auxNombre;
                if(nombre == "@"){ //Si el usuario a la hora de modificar quiere que el nombre se quede igual

                    archivo = fopen("Datos_Medicamentos.dat", "rb");
                    fread(&m, sizeof(medicamento), 1, archivo);

                    while(!feof(archivo) ){

                        if(id == m.id) auxNombre = m.nombre; //Me fijo cual ser?a el nombre que no tengo que cambiar

                        fread(&m, sizeof(medicamento), 1, archivo);
                    }

                    fclose(archivo);
                }

                string auxTipo;
                if(tipo == "@"){ //Si el usuario a la hora de modificar quiere que el tipo se quede igual

                    archivo = fopen("Datos_Medicamentos.dat", "rb");
                    fread(&m, sizeof(medicamento), 1, archivo);

                    while(!feof(archivo) ){

                        if(id == m.id) auxTipo = m.tipo; //Me fijo cual ser?a el tipo que no tengo que cambiar

                        fread(&m, sizeof(medicamento), 1, archivo);
                    }

                    fclose(archivo);
                }


                archivo = fopen("Datos_Medicamentos.dat", "rb"); //Abro archivo en modo lectura binaria
                fread(&m, sizeof(medicamento), 1, archivo);      //Primera lectura

                duplaValida = true;

                if(nombre != "@" || tipo != "@"){ //No tiene sentido revisar si la dupla es valida o no, cuando el usuario no modifica el nombre y el tipo que de por si ya son validos si estan cargados
                    while(duplaValida && !feof(archivo) ){
                        //Chequeo medicamento por medicamento para saber si ya existe la dupla

                        bool nombresCoinciden = false;

                        if(nombre != "@"){ //Si el usuario no ingreso la opcion de no modificar el nombre
                            auxNombre = m.nombre;
                            if(nombre == auxNombre) nombresCoinciden = true; //Me fijo si el nombre ingresado por el cliente coincide con el de esta lectura
                        }

                        if(nombre == "@"){ //Si el usuario si ingreso la opcion de no modificar el nombre
                            string auxNombre2 = m.nombre;
                            if(auxNombre == auxNombre2) nombresCoinciden = true;
                        }


                        bool tiposCoinciden = false;

                        if(tipo != "@"){ //Si el usuario no ingreso la opcion de no modificar el tipo
                            auxTipo = m.tipo;
                            if(tipo == auxTipo) tiposCoinciden = true; //Me fijo si el tipo ingresado por el cliente coincide con el de esta lectura
                        }

                        if(tipo == "@"){ //Si el usuario si ingreso la opcion de no modificar el tipo
                            string auxTipo2 = m.tipo;
                            if(auxTipo == auxTipo2) tiposCoinciden = true;
                        }

                        //Como m (el medicamento) se actualiza a medida que vamos leyendo el archivo, si en la misma lectura salta que ambos booleanos son verdaderos significa que la dupla ya existe
                        if(nombresCoinciden && tiposCoinciden) {
                            duplaValida = false;
                            if(nombre == "@") nombre = auxNombre;
                            if(tipo == "@") tipo = auxTipo;
                            mensaje = "El medicamento " + nombre + "?" + tipo + " ya existe, por favor ingrese un Nombre Comercial y/o Tipo de medicamento diferente \n\n";
                        }

                        fread(&m, sizeof(medicamento), 1, archivo); //Siguiente lectura
                    }
                }

                if(duplaValida) mensaje = "";

                strcpy(servidor.buffer, mensaje.c_str() );
                servidor.enviar();

                fclose(archivo); //Cierro el archivo que estaba en modo lectura
            }


            string droga = servidor.recibir(); //El cliente ingresa la nueva droga del medicamento

            FILE *auxArchivo;
            auxArchivo = fopen("Archivo_Auxiliar.dat", "ab");   //Creo otro archivo binario donde se guardan los datos originales modificados
            archivo = fopen("Datos_Medicamentos.dat", "rb");    //Abro el archivo original con los datos sin modificar

            medicamento auxM;

            fread(&m, sizeof(medicamento), 1, archivo); //Primera lectura

            while(!feof(archivo) ){

                if(id == m.id){ //Si el id ingresado por el cliente coincide con el id de esta lectura, se procede a guardar los cambios en el nuevo arhivo

                    //Cargo los datos originales en una estructura auxiliar de medicamento
                    auxM.id = id;
                    strcpy(auxM.codigo, m.codigo);
                    strcpy(auxM.nombre, m.nombre);
                    strcpy(auxM.tipo, m.tipo);
                    strcpy(auxM.droga, m.droga);

                    //Si el cliente ingreso un @, significa que no quiere que ese atributo sea modificado
                    //Por lo que si en ese campo hay un @, dejo los datos originales
                    if(codigo != "@")   strcpy(auxM.codigo, codigo.c_str() );
                    if(nombre != "@")   strcpy(auxM.nombre, nombre.c_str() );
                    if(tipo   != "@")   strcpy(auxM.tipo, tipo.c_str() );
                    if(droga  != "@")   strcpy(auxM.droga, droga.c_str() );

                    fwrite(&auxM, sizeof(medicamento), 1, auxArchivo);

                }else fwrite(&m, sizeof(medicamento), 1, auxArchivo); //Si el id ingresado por el cliente no coincide con el id de esta lectura, se guardan los datos sin modificar en el nuevo archivo

                fread(&m, sizeof(medicamento), 1, archivo); //Siguiente lectura
            }

            fclose(archivo);  //Cierro los archivos
            fclose(auxArchivo);

            char guardar = servidor.recibirInforme(); //El cliente decide si quiere guardar los datos modificados o no

            if(guardar == 's'){ //Si se quiere guardar los cambios
                remove("Datos_Medicamentos.dat"); //Elimino el archivo con los datos viejos
                rename("Archivo_Auxiliar.dat", "Datos_Medicamentos.dat"); //y el archivo con los datos nuevos lo renombro con el nombre del original

                //Muestro al cliente el cambio en los datos
                mensaje = "ID: " + to_string(auxM.id) + "\n" + "Codigo: " + auxM.codigo + "\n" +
                          "Nombre: " + auxM.nombre + "\n" + "Droga: " + auxM.droga + "\n" +
                          "Tipo: " + auxM.tipo + "\n\n";

                strcpy(servidor.buffer,  mensaje.c_str() );
                servidor.enviar();

                string tiempoYFecha = fechaHoraActual(); //Registro la actividad en el .log del servidor y del cliente

                ofstream archivoServidor;
                archivoServidor.open("server.log", ios::app);
                archivoServidor << tiempoYFecha << "========ID: " << id << " - Medicamento - Modificado=======" << endl;
                archivoServidor.close();

                ofstream archivoCliente;
                archivoCliente.open(nombreArchivoCliente, ios::app);
                archivoCliente << tiempoYFecha << "========ID: " << id << " - Medicamento - Modificado=======" << endl;
                archivoCliente.close();
            }
            if(guardar == 'n') remove("Archivo_Auxiliar.dat"); //Si no se quiere guardar los cambios, simplemente borro el archivo con los datos modificados y me quedo con el archivo original

        }

        if(modificarOeliminar == '2'){ //Si el cliente no ingreso un 1 significa que ingreso un 2, lo que significa que el cliente quiere eliminar

            FILE *auxArchivo;
            auxArchivo = fopen("Archivo_Auxiliar.dat", "ab");   //Creo otro archivo binario donde se guardan los datos originales modificados
            archivo = fopen("Datos_Medicamentos.dat", "rb");    //Abro el archivo original con los datos sin modificar

            fread(&m, sizeof(medicamento), 1, archivo); //Primera lectura

            while(!feof(archivo) ){

                //Si el id que ingreso el cliente y el de esta lectura no coinciden significa que no es el medicamento que se quiere eliminar, por lo que escribo los datos de esta lectura en el nuevo archivo
                //Si el id que ingreso el cliente y el de esta lectura coinciden significa que es el medicamento que el cliente quiere eliminar, por lo que simplemente no lo escribo y sigo leyendo
                if(id != m.id) fwrite(&m, sizeof(medicamento), 1, auxArchivo);

                fread(&m, sizeof(medicamento), 1, archivo); //Siguiente lectura
            }

            fclose(archivo);  //Cierro los archivos
            fclose(auxArchivo);

            char guardar = servidor.recibirInforme();

            if(guardar == 's'){  //Si se quiere guardar los cambios
                remove("Datos_Medicamentos.dat");  //Elimino el archivo con los datos viejos
                rename("Archivo_Auxiliar.dat", "Datos_Medicamentos.dat");  //y el archivo con los datos nuevos lo renombro con el nombre del original

                string tiempoYFecha = fechaHoraActual(); //Registro la actividad en el .log del servidor y del cliente

                ofstream archivoServidor;
                archivoServidor.open("server.log", ios::app);
                archivoServidor << tiempoYFecha << "========ID: " << id << " - Medicamento - Baja=======" << endl;
                archivoServidor.close();

                ofstream archivoCliente;
                archivoCliente.open(nombreArchivoCliente, ios::app);
                archivoCliente << tiempoYFecha << "========ID: " << id << " - Medicamento - Baja=======" << endl;
                archivoCliente.close();
            }
            if(guardar == 'n') remove("Archivo_Auxiliar.dat");  //Si no se quiere guardar los cambios, simplemente borro el archivo con los datos modificados y me quedo con el archivo original
        }

   }



}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
void verTodoMedicamento(Servidor servidor){

    medicamento m;

    FILE *archivo;
    archivo = fopen("Datos_Medicamentos.dat", "rb");  //Abro el archivo en modo lectura
    fread(&m, sizeof(medicamento), 1, archivo);       //Primera lectura

    while(!feof(archivo) ){ //Le muestro al cliente todo lo que hay en el archivo de medicamentos

        string mensaje = "ID: " + to_string(m.id) + "\n" + "Codigo: " + m.codigo + "\n" + "Nombre comercial: " + m.nombre + "\n"
                + "Droga: " + m.droga + "\n" + "Tipo de medicamento: " + m.tipo + "\n\n";

        strcpy(servidor.buffer,  mensaje.c_str() );
        servidor.enviar();

        fread(&m, sizeof(medicamento), 1, archivo);    //Siguiente lectura
    }

    strcpy(servidor.buffer,  ""); //Le aviso al cliente de que ya no espere por m?s info
    servidor.enviar();

    fclose(archivo);    //Cierro el archivo


    string tiempoYFecha = fechaHoraActual();  //Registro la actividad en el .log del servidor y del cliente

    ofstream archivoServidor;
    archivoServidor.open("server.log", ios::app);
    archivoServidor << tiempoYFecha << "========Ver todo - Medicamento=======" << endl;
    archivoServidor.close();

    ofstream archivoCliente;
    archivoCliente.open(nombreArchivoCliente, ios::app);
    archivoCliente << tiempoYFecha << "========Ver todo - Medicamento=======" << endl;
    archivoCliente.close();
}
/*******************************************************************************************************************/
/*******************************************************************************************************************/
void transmitirArchivoDeActividades(Servidor servidor){

    FILE *archivo;
    archivo = fopen(nombreArchivoCliente.c_str(), "r"); //Abro el archivo del cliente en modo lectura

    while(!feof(archivo) ){ //Le envi? el contenido del archivo al cliente
        fgets(servidor.buffer, 1024, archivo);
        servidor.enviar();
    }

    servidor.buffer[0] = ' ';    //Para avisar al cliente que ya no va a recibir m?s
    servidor.enviar();

    fclose(archivo); //Cierro el archivo

}


