#ifndef SISTEMAGESTION_H
#define SISTEMAGESTION_H

#include <vector>
#include <string>
#include <utility>
#include "Inventario.h"
#include "Escuela.h"
#include "Vale.h"
#include "DatabaseManager.h"

using namespace std;

class SistemaGestion
{
public:
    SistemaGestion();

    // Conexion a la base de datos
    // Llama esto una vez al iniciar la app (desde MainWindow o main.cpp)
    bool conectarDB(const string &host     = "localhost",
                    const string &puerto   = "5432",
                    const string &bd       = "enlace_libreria",
                    const string &usuario  = "postgres",
                    const string &password = "");

    bool dbConectada() const;

    // Autenticacion
    bool autenticar(const string &correo,
                    const string &contrasena,
                    UsuarioDB &out);

    // Registro
    bool registrarLibro(string titulo, string autor, int isbn,
                        string editorial, string categoria,
                        string materia, float precio, int cantidad,
                        string imagenUrl = "");

    bool registrarEscuela(string nombre, string ubicacion,
                          int telefono, string correo,
                          int idEscuela, string fecha = "");

    bool registrarVale(int idVale, int idEscuela,
                       const vector<pair<int,int>> &librosSolicitados,
                       string fechaEmision, string correo,
                       bool pagado = false);

    // Consultas — devuelven datos desde PostgreSQL
    vector<LibroDB>   librosDisponibles()  const;
    vector<EscuelaDB> todasEscuelas()      const;
    vector<ValeDB>    valesPendientes()    const;

    // Busqueda
    Escuela*   buscarEscuela(int idEscuela);   // compatibilidad MainWindow
    EscuelaDB* buscarEscuelaDB(int idEscuela); // version DB (caller libera)

    // Reportes (devuelven texto listo para mostrar en QTextEdit)
    string reporteSemanal()  const;
    string resumenGeneral()  const;

    // Para mostrar en consola (compatibilidad original)
    void mostrarEscuelas()       const;
    void mostrarValesPendientes()const;
    void generarReporteSemanal() const;
    void mostrarResumen()        const;

    // ID de vale
    int  generarIdVale();

    // Guardar/cargar (compatibilidad CSV)
    void guardarInventario(const string &archivo = "data/inventario.csv");
    void cargarInventario (const string &archivo = "data/inventario.csv");

private:
    DatabaseManager db;

    // Cache local para compatibilidad con codigo que usa Escuela*
    vector<Escuela> cacheEscuelas;
    int contadorVales = 1;

    void sincronizarCacheEscuelas();
};

#endif // SISTEMAGESTION_H
