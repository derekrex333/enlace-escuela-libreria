#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <libpq-fe.h>
#include <string>
#include <vector>
#include <tuple>

//  Estructuras de datos (mapean filas de PostgreSQL)
struct LibroDB {
    long long   isbn;
    std::string titulo;
    std::string autor;
    std::string editorial;
    std::string categoria;
    std::string materia;
    double      precio;
    int         cantidad;
    std::string imagenUrl;
};

struct EscuelaDB {
    int         idEscuela;
    std::string nombre;
    std::string ubicacion;
    long long   telefono;
    std::string correo;
};

struct DetalleValeDB {
    int       isbn;
    int       cantidad;
    double    precioUnit;
};

struct ValeDB {
    int                        idVale;
    int                        idEscuela;
    std::string                correo;
    std::string                fecha;
    std::string                estado;
    std::vector<DetalleValeDB> detalles;
};

struct UsuarioDB {
    int         idUsuario;
    std::string nombre;
    std::string rol;   // 'p', 'e', 'es'
};

//  DatabaseManager
class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();

    // Conexion
    bool conectar(const std::string &host,
                  const std::string &puerto,
                  const std::string &baseDatos,
                  const std::string &usuario,
                  const std::string &contrasena);
    void desconectar();
    bool estaConectado() const;
    std::string ultimoError() const;

    // Autenticacion
    bool autenticar(const std::string &correo,
                    const std::string &contrasena,
                    UsuarioDB &out);

    // Libros
    bool registrarLibro(const LibroDB &libro);
    std::vector<LibroDB> librosDisponibles();
    LibroDB* buscarLibroPorISBN(long long isbn);   // caller debe liberar

    // Escuelas
    bool registrarEscuela(const EscuelaDB &esc);
    std::vector<EscuelaDB> todasEscuelas();
    EscuelaDB* buscarEscuelaPorID(int id);         // caller debe liberar

    // Vales
    bool registrarVale(const ValeDB &vale);
    std::vector<ValeDB> valesPendientes();
    int  siguienteIdVale();

    // Reportes
    std::string reporteSemanal();
    std::string resumenGeneral();

    // Inventario (exportar a CSV — compatibilidad con tu sistema actual)
    bool guardarInventarioCSV(const std::string &rutaArchivo);

private:
    PGconn  *conn = nullptr;
    std::string lastError;

    // Helpers internos
    PGresult* ejecutar(const std::string &sql);
    PGresult* ejecutarParams(const std::string &sql,
                             int nParams,
                             const char * const *valores);
    void       liberarResultado(PGresult *res);
    bool       resultadoOk(PGresult *res, const std::string &contexto);

    LibroDB   filaALibro(PGresult *res, int fila);
    EscuelaDB filaAEscuela(PGresult *res, int fila);
};

#endif // DATABASEMANAGER_H
