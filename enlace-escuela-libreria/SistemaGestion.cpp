#include "SistemaGestion.h"
#include "CsvUtils.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

//  Constructor
SistemaGestion::SistemaGestion() : contadorVales(1)
{
    CsvUtils::asegurarCarpetaData();
    // La conexion a DB se hace llamando conectarDB() desde MainWindow
    // despues de construir el objeto.
}

//  Conexion
bool SistemaGestion::conectarDB(const string &host,
                                const string &puerto,
                                const string &bd,
                                const string &usuario,
                                const string &password)
{
    bool ok = db.conectar(host, puerto, bd, usuario, password);
    if (!ok)
        cerr << "[SistemaGestion] Error al conectar a PostgreSQL: "
             << db.ultimoError() << "\n";
    else
    {
        cout << "[SistemaGestion] Conectado a PostgreSQL correctamente.\n";
        sincronizarCacheEscuelas();
    }
    return ok;
}

bool SistemaGestion::dbConectada() const
{
    return db.estaConectado();
}

//  Autenticacion
bool SistemaGestion::autenticar(const string &correo,
                                const string &contrasena,
                                UsuarioDB &out)
{
    if (!db.estaConectado()) return false;
    return db.autenticar(correo, contrasena, out);
}

//  Cache de escuelas (para compatibilidad con Escuela*)
void SistemaGestion::sincronizarCacheEscuelas()
{
    cacheEscuelas.clear();
    auto lista = db.todasEscuelas();
    for (const auto &e : lista)
    {
        Escuela esc(e.nombre, e.ubicacion,
                    (int)e.telefono, e.correo,
                    e.idEscuela, "");
        cacheEscuelas.push_back(esc);
    }

    // Actualizar contador de vales
    int idDB = db.siguienteIdVale();
    if (idDB > contadorVales) contadorVales = idDB;
}

//  REGISTRAR LIBRO
bool SistemaGestion::registrarLibro(string titulo, string autor, int isbn,
                                    string editorial, string categoria,
                                    string materia, float precio, int cantidad,
                                    string imagenUrl)
{
    if (!db.estaConectado())
    {
        cerr << "[registrarLibro] Sin conexion a la base de datos.\n";
        return false;
    }

    LibroDB libro;
    libro.isbn      = isbn;
    libro.titulo    = titulo;
    libro.autor     = autor;
    libro.editorial = editorial;
    libro.categoria = categoria;
    libro.materia   = materia;
    libro.precio    = precio;
    libro.cantidad  = cantidad;
    libro.imagenUrl = imagenUrl;

    bool ok = db.registrarLibro(libro);
    if (ok)
        cout << "[registrarLibro] Libro ISBN " << isbn << " registrado en PostgreSQL.\n";
    return ok;
}

//  REGISTRAR ESCUELA
bool SistemaGestion::registrarEscuela(string nombre, string ubicacion,
                                      int telefono, string correo,
                                      int idEscuela, string /*fecha*/)
{
    if (!db.estaConectado())
    {
        cerr << "[registrarEscuela] Sin conexion a la base de datos.\n";
        return false;
    }

    EscuelaDB esc;
    esc.idEscuela = idEscuela;
    esc.nombre    = nombre;
    esc.ubicacion = ubicacion;
    esc.telefono  = telefono;
    esc.correo    = correo;

    bool ok = db.registrarEscuela(esc);
    if (ok)
    {
        cout << "[registrarEscuela] Escuela ID " << idEscuela << " registrada en PostgreSQL.\n";
        sincronizarCacheEscuelas();
    }
    return ok;
}

//  REGISTRAR VALE
bool SistemaGestion::registrarVale(int idVale, int idEscuela,
                                   const vector<pair<int,int>> &librosSolicitados,
                                   string fechaEmision, string correo,
                                   bool /*pagado*/)
{
    if (!db.estaConectado())
    {
        cerr << "[registrarVale] Sin conexion a la base de datos.\n";
        return false;
    }

    ValeDB vale;
    vale.idVale    = idVale;
    vale.idEscuela = idEscuela;
    vale.correo    = correo;
    vale.fecha     = fechaEmision;
    vale.estado    = "pendiente";

    for (const auto &par : librosSolicitados)
    {
        // Obtener precio actual desde DB
        LibroDB *lib = db.buscarLibroPorISBN(par.first);
        double precioUnit = 0.0;
        if (lib)
        {
            precioUnit = lib->precio;
            delete lib;
        }

        DetalleValeDB det;
        det.isbn      = par.first;
        det.cantidad  = par.second;
        det.precioUnit= precioUnit;
        vale.detalles.push_back(det);
    }

    bool ok = db.registrarVale(vale);
    if (ok)
        cout << "[registrarVale] Vale #" << idVale << " guardado en PostgreSQL.\n";
    return ok;
}

//  CONSULTAS
vector<LibroDB> SistemaGestion::librosDisponibles() const
{
    if (!db.estaConectado())
    {
        cerr << "[librosDisponibles] Sin conexion.\n";
        return {};
    }
    return const_cast<DatabaseManager&>(db).librosDisponibles();
}

vector<EscuelaDB> SistemaGestion::todasEscuelas() const
{
    if (!db.estaConectado()) return {};
    return const_cast<DatabaseManager&>(db).todasEscuelas();
}

vector<ValeDB> SistemaGestion::valesPendientes() const
{
    if (!db.estaConectado()) return {};
    return const_cast<DatabaseManager&>(db).valesPendientes();
}

//  BUSQUEDA
Escuela* SistemaGestion::buscarEscuela(int idEscuela)
{
    // Busca en cache local (para compatibilidad con MainWindow)
    for (auto &e : cacheEscuelas)
        if (e.getIdEscuela() == idEscuela) return &e;

    // Si no esta en cache, refresca y vuelve a buscar
    if (db.estaConectado())
    {
        sincronizarCacheEscuelas();
        for (auto &e : cacheEscuelas)
            if (e.getIdEscuela() == idEscuela) return &e;
    }
    return nullptr;
}

EscuelaDB* SistemaGestion::buscarEscuelaDB(int idEscuela)
{
    if (!db.estaConectado()) return nullptr;
    return db.buscarEscuelaPorID(idEscuela);
}

//  REPORTES — texto para QTextEdit
string SistemaGestion::reporteSemanal() const
{
    if (!db.estaConectado())
        return "Sin conexion a la base de datos.";
    return const_cast<DatabaseManager&>(db).reporteSemanal();
}

string SistemaGestion::resumenGeneral() const
{
    if (!db.estaConectado())
        return "Sin conexion a la base de datos.";
    return const_cast<DatabaseManager&>(db).resumenGeneral();
}

//  REPORTES — compatibilidad consola
void SistemaGestion::mostrarEscuelas() const
{
    auto lista = const_cast<DatabaseManager&>(db).todasEscuelas();
    if (lista.empty()) { cout << "No hay escuelas registradas.\n"; return; }
    cout << "\nLISTA DE ESCUELAS\n";
    for (const auto &e : lista)
        cout << "ID: " << e.idEscuela
             << " | " << e.nombre
             << " | " << e.ubicacion
             << " | " << e.correo << "\n";
}

void SistemaGestion::mostrarValesPendientes() const
{
    auto lista = const_cast<DatabaseManager&>(db).valesPendientes();
    if (lista.empty()) { cout << "No hay vales pendientes.\n"; return; }
    cout << "\nVALES PENDIENTES\n";
    for (const auto &v : lista)
        cout << "Vale #" << v.idVale
             << " | Escuela ID: " << v.idEscuela
             << " | Fecha: " << v.fecha
             << " | Libros: " << v.detalles.size() << "\n";
}

void SistemaGestion::generarReporteSemanal() const
{
    cout << reporteSemanal();
}

void SistemaGestion::mostrarResumen() const
{
    cout << resumenGeneral();
}

//  ID de vale
int SistemaGestion::generarIdVale()
{
    if (db.estaConectado())
    {
        int id = db.siguienteIdVale();
        if (id > 0) return id;
    }
    return contadorVales++;
}

//  Guardar/cargar CSV (compatibilidad)
void SistemaGestion::guardarInventario(const string &archivo)
{
    if (db.estaConectado())
        db.guardarInventarioCSV(archivo);
    else
        cerr << "[guardarInventario] Sin conexion a la base de datos.\n";
}

void SistemaGestion::cargarInventario(const string &/*archivo*/)
{
    // Con PostgreSQL la carga es automatica al conectar
    // Este metodo se conserva por compatibilidad
    if (db.estaConectado())
        sincronizarCacheEscuelas();
}
