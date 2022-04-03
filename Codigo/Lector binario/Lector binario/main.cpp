#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

typedef struct{

    int id;
    char tipo[20];      //también llamado denominación
    bool activo;

} tipomedicamento;

typedef struct{

    int id;
    char codigo[15];
    char nombre[20];    //Nombre comercial
    char droga[20];
    char tipo[20];      //también llamado denominación

} medicamento;

void crearArchivosVacios();

void crearMedicamento(medicamento m1);
void crearTipoMedicamento(tipomedicamento t);

void leer_todo_medicamentos();
void leer_todo_tipoMedicamentos();

void consulta_Medicamento(int id);

int main(){

    //crearArchivosVacios();

    /*
    medicamento m1 = {3, "DCR-88578-9", "Paracetamol", "Diazepan", "Analgesico"};
    medicamento m2 = {10, "ABC-11232-9", "Ibuprofeno", "Tramadol", "Antiinflamatorio"};
    crearMedicamento(m1);
    crearMedicamento(m2);
*/
    leer_todo_medicamentos();


    /*
    tipomedicamento t1 = {4, "Analgesico", false};
    tipomedicamento t2 = {23, "Antiinflamatorio", true};
    tipomedicamento t3 = {7, "Antiinfeccioso", false};
    crearTipoMedicamento(t1);
    crearTipoMedicamento(t2);
    crearTipoMedicamento(t3);
*/
    leer_todo_tipoMedicamentos();


    //int id = 20;
    //consulta_Medicamento(id);

    return 0;
}
/************************************************************************************************************************/
/************************************************************************************************************************/
void crearArchivosVacios(){

    FILE *archivo;
    archivo = fopen("Datos_Medicamentos.dat", "ab");
    fclose(archivo);

    FILE *archivo2;
    archivo2 = fopen("Datos_Tipos_De_Medicamentos.dat", "ab");
    fclose(archivo2);

}
/************************************************************************************************************************/
/************************************************************************************************************************/
void crearMedicamento(medicamento m){

    FILE *archivo;
    archivo = fopen("Datos_Medicamentos.dat", "ab");
    fwrite(&m, sizeof(medicamento), 1, archivo);
    fclose(archivo);

    /*
    Otra forma de escribit en archivos binarios

    ofstream archivo("Datos.bin", ios::out|ios::app|ios::binary);
    archivo.write((char *)&m1, sizeof(medicamento) );
    archivo.close();
    */
}
/************************************************************************************************************************/
/************************************************************************************************************************/
void crearTipoMedicamento(tipomedicamento t){

    FILE *archivo;
    archivo = fopen("Datos_Tipos_De_Medicamentos.dat", "ab");
    fwrite(&t, sizeof(tipomedicamento), 1, archivo);
    fclose(archivo);

}
/************************************************************************************************************************/
/************************************************************************************************************************/
void leer_todo_medicamentos(){

    medicamento m;

    FILE *archivo;
    archivo = fopen("Datos_Medicamentos.dat", "rb");
    fread(&m, sizeof(medicamento), 1, archivo);

    while(!feof(archivo) ){
        cout << "ID: "                  << m.id     << endl;
        cout << "Codigo: "              << m.codigo << endl;
        cout << "Nombre: "              << m.nombre << endl;
        cout << "Droga: "               << m.droga  << endl;
        cout << "Tipo de medicamento: " << m.tipo   << "\n" << endl;
        fread(&m, sizeof(medicamento), 1, archivo);
    }

    fclose(archivo);

    /*
    Otra forma de leer

    ifstream archivo("Datos.bin", ios::binary);
    archivo.read((char *)&id, sizeof(int) );
    archivo.close();
    */
}
/************************************************************************************************************************/
/************************************************************************************************************************/
void leer_todo_tipoMedicamentos(){

    tipomedicamento t;

    FILE *archivo;
    archivo = fopen("Datos_Tipos_De_Medicamentos.dat", "rb");
    fread(&t, sizeof(tipomedicamento), 1, archivo);

    while(!feof(archivo) ){
        cout << "ID: "                  << t.id     << endl;
        cout << "Tipo de medicamento: " << t.tipo   << endl;
        cout << "Activo: "              << t.activo << "\n" << endl;

        fread(&t, sizeof(tipomedicamento), 1, archivo);
    }

    fclose(archivo);
}
/************************************************************************************************************************/
/************************************************************************************************************************/
void consulta_Medicamento(int id){

    medicamento m;

    FILE *archivo;
    archivo = fopen("Datos_Medicamentos.dat", "rb");
    fread(&m, sizeof(medicamento), 1, archivo);

    bool existe = false;

    while(!feof(archivo) ){

        if(id == m.id){
            cout << "ID: "                  << m.id     << endl;
            cout << "Codigo: "              << m.codigo << endl;
            cout << "Nombre: "              << m.nombre << endl;
            cout << "Droga: "               << m.droga  << endl;
            cout << "Tipo de medicamento: " << m.tipo   << "\n" << endl;

            existe = true;
        }

        fread(&m, sizeof(medicamento), 1, archivo);
    }

    if(!existe) printf("No existe el medicamento con dicho ID \n");

    fclose(archivo);
}
