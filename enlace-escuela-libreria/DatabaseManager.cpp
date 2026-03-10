/*  DatabaseManager.cpp
    Implementacion del conector PostgreSQL con libpq.
*/

#include "DatabaseManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

//  Constructor / Destructor
DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager()
{
    desconectar();
}

//  Conexion
bool DatabaseManager::conectar(const std::string &host,
                               const std::string &puerto,
                               const std::string &baseDatos,
                               const std::string &usuario,
                               const std::string &contrasena)
{
    std::string cadena =
        "host="     + host      +
        " port="    + puerto    +
        " dbname="  + baseDatos +
        " user="    + usuario   +
        " password="+ contrasena+
        " connect_timeout=10"
        " application_name=EnlaceLibreria";

    conn = PQconnectdb(cadena.c_str());

    if (PQstatus(conn) != CONNECTION_OK)
    {
        lastError = PQerrorMessage(conn);
        PQfinish(conn);
        conn = nullptr;
        return false;
    }

    // Forzar UTF-8
    PQsetClientEncoding(conn, "UTF8");
    return true;
}

void DatabaseManager::desconectar()
{
    if (conn)
    {
        PQfinish(conn);
        conn = nullptr;
    }
}

bool DatabaseManager::estaConectado() const
{
    return conn && PQstatus(conn) == CONNECTION_OK;
}

std::string DatabaseManager::ultimoError() const
{
    return lastError;
}

//  Helpers internos
PGresult* DatabaseManager::ejecutar(const std::string &sql)
{
    return PQexec(conn, sql.c_str());
}

PGresult* DatabaseManager::ejecutarParams(const std::string &sql,
                                          int nParams,
                                          const char * const *valores)
{
    return PQexecParams(conn,
                        sql.c_str(),
                        nParams,
                        nullptr,   // tipos inferidos
                        valores,
                        nullptr,   // longitudes (texto)
                        nullptr,   // formatos (texto)
                        0);        // resultado en texto
}

void DatabaseManager::liberarResultado(PGresult *res)
{
    if (res) PQclear(res);
}

bool DatabaseManager::resultadoOk(PGresult *res, const std::string &contexto)
{
    if (!res ||
        (PQresultStatus(res) != PGRES_COMMAND_OK &&
         PQresultStatus(res) != PGRES_TUPLES_OK))
    {
        lastError = contexto + ": " + (res ? PQresultErrorMessage(res) : "Sin resultado");
        std::cerr << "[DB ERROR] " << lastError << "\n";
        liberarResultado(res);
        return false;
    }
    return true;
}

//  Mapear fila -> struct
LibroDB DatabaseManager::filaALibro(PGresult *res, int fila)
{
    LibroDB l;
    l.isbn      = std::stoll(PQgetvalue(res, fila, 0));
    l.titulo    = PQgetvalue(res, fila, 1);
    l.autor     = PQgetvalue(res, fila, 2);
    l.editorial = PQgetvalue(res, fila, 3);
    l.categoria = PQgetvalue(res, fila, 4);
    l.materia   = PQgetvalue(res, fila, 5);
    l.precio    = std::stod(PQgetvalue(res, fila, 6));
    l.cantidad  = std::stoi(PQgetvalue(res, fila, 7));
    l.imagenUrl = PQgetvalue(res, fila, 8);
    return l;
}

EscuelaDB DatabaseManager::filaAEscuela(PGresult *res, int fila)
{
    EscuelaDB e;
    e.idEscuela = std::stoi(PQgetvalue(res, fila, 0));
    e.nombre    = PQgetvalue(res, fila, 1);
    e.ubicacion = PQgetvalue(res, fila, 2);
    const char *tel = PQgetvalue(res, fila, 3);
    e.telefono  = (tel && tel[0] != '\0') ? std::stoll(tel) : 0;
    e.correo    = PQgetvalue(res, fila, 4);
    return e;
}

//  AUTENTICACION
bool DatabaseManager::autenticar(const std::string &correo,
                                 const std::string &contrasena,
                                 UsuarioDB &out)
{
    /*  Llama a la funcion SQL:
        autenticar_usuario(correo, contrasena)
        que internamente usa crypt() de pgcrypto.
    */
    const char *vals[2] = { correo.c_str(), contrasena.c_str() };

    PGresult *res = ejecutarParams(
        "SELECT id_usuario, nombre, rol "
        "FROM autenticar_usuario($1, $2)",
        2, vals);

    if (!resultadoOk(res, "autenticar"))
        return false;

    bool ok = (PQntuples(res) > 0);
    if (ok)
    {
        out.idUsuario = std::stoi(PQgetvalue(res, 0, 0));
        out.nombre    = PQgetvalue(res, 0, 1);
        out.rol       = PQgetvalue(res, 0, 2);
    }
    else
    {
        lastError = "Credenciales incorrectas.";
    }

    liberarResultado(res);
    return ok;
}

//  LIBROS
bool DatabaseManager::registrarLibro(const LibroDB &libro)
{
    std::string isbn     = std::to_string(libro.isbn);
    std::string precio   = std::to_string(libro.precio);
    std::string cantidad = std::to_string(libro.cantidad);

    const char *vals[9] = {
        isbn.c_str(),
        libro.titulo.c_str(),
        libro.autor.c_str(),
        libro.editorial.c_str(),
        libro.categoria.c_str(),
        libro.materia.c_str(),
        precio.c_str(),
        cantidad.c_str(),
        libro.imagenUrl.c_str()
    };

    PGresult *res = ejecutarParams(
        "INSERT INTO libros "
        "(isbn, titulo, autor, editorial, categoria, materia, precio, cantidad, imagen_url) "
        "VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9) "
        "ON CONFLICT (isbn) DO UPDATE SET "
        "  titulo=EXCLUDED.titulo, autor=EXCLUDED.autor, "
        "  editorial=EXCLUDED.editorial, categoria=EXCLUDED.categoria, "
        "  materia=EXCLUDED.materia, precio=EXCLUDED.precio, "
        "  cantidad=libros.cantidad + EXCLUDED.cantidad, "
        "  imagen_url=EXCLUDED.imagen_url",
        9, vals);

    bool ok = resultadoOk(res, "registrarLibro");
    liberarResultado(res);
    return ok;
}

std::vector<LibroDB> DatabaseManager::librosDisponibles()
{
    std::vector<LibroDB> lista;

    PGresult *res = ejecutar(
        "SELECT isbn, titulo, autor, editorial, categoria, materia, "
        "       precio, cantidad, COALESCE(imagen_url,'') "
        "FROM v_libros_disponibles");

    if (!resultadoOk(res, "librosDisponibles"))
        return lista;

    int n = PQntuples(res);
    lista.reserve(n);
    for (int i = 0; i < n; i++)
        lista.push_back(filaALibro(res, i));

    liberarResultado(res);
    return lista;
}

LibroDB* DatabaseManager::buscarLibroPorISBN(long long isbn)
{
    std::string isbnStr = std::to_string(isbn);
    const char *vals[1] = { isbnStr.c_str() };

    PGresult *res = ejecutarParams(
        "SELECT isbn, titulo, autor, editorial, categoria, materia, "
        "       precio, cantidad, COALESCE(imagen_url,'') "
        "FROM libros WHERE isbn = $1",
        1, vals);

    if (!resultadoOk(res, "buscarLibroPorISBN") || PQntuples(res) == 0)
    {
        liberarResultado(res);
        return nullptr;
    }

    LibroDB *l = new LibroDB(filaALibro(res, 0));
    liberarResultado(res);
    return l;
}

//  ESCUELAS
bool DatabaseManager::registrarEscuela(const EscuelaDB &esc)
{
    std::string id  = std::to_string(esc.idEscuela);
    std::string tel = std::to_string(esc.telefono);

    const char *vals[5] = {
        id.c_str(),
        esc.nombre.c_str(),
        esc.ubicacion.c_str(),
        tel.c_str(),
        esc.correo.c_str()
    };

    PGresult *res = ejecutarParams(
        "INSERT INTO escuelas (id_escuela, nombre, ubicacion, telefono, correo) "
        "VALUES ($1,$2,$3,$4,$5) "
        "ON CONFLICT (id_escuela) DO UPDATE SET "
        "  nombre=EXCLUDED.nombre, ubicacion=EXCLUDED.ubicacion, "
        "  telefono=EXCLUDED.telefono, correo=EXCLUDED.correo",
        5, vals);

    bool ok = resultadoOk(res, "registrarEscuela");
    liberarResultado(res);
    return ok;
}

std::vector<EscuelaDB> DatabaseManager::todasEscuelas()
{
    std::vector<EscuelaDB> lista;

    PGresult *res = ejecutar(
        "SELECT id_escuela, nombre, ubicacion, "
        "       COALESCE(telefono::text,''), COALESCE(correo,'') "
        "FROM escuelas WHERE activa = TRUE ORDER BY nombre");

    if (!resultadoOk(res, "todasEscuelas"))
        return lista;

    int n = PQntuples(res);
    lista.reserve(n);
    for (int i = 0; i < n; i++)
        lista.push_back(filaAEscuela(res, i));

    liberarResultado(res);
    return lista;
}

EscuelaDB* DatabaseManager::buscarEscuelaPorID(int id)
{
    std::string idStr = std::to_string(id);
    const char *vals[1] = { idStr.c_str() };

    PGresult *res = ejecutarParams(
        "SELECT id_escuela, nombre, ubicacion, "
        "       COALESCE(telefono::text,''), COALESCE(correo,'') "
        "FROM escuelas WHERE id_escuela = $1 AND activa = TRUE",
        1, vals);

    if (!resultadoOk(res, "buscarEscuelaPorID") || PQntuples(res) == 0)
    {
        liberarResultado(res);
        return nullptr;
    }

    EscuelaDB *e = new EscuelaDB(filaAEscuela(res, 0));
    liberarResultado(res);
    return e;
}

//  VALES
int DatabaseManager::siguienteIdVale()
{
    PGresult *res = ejecutar("SELECT siguiente_id_vale()");
    if (!resultadoOk(res, "siguienteIdVale")) return -1;
    int id = std::stoi(PQgetvalue(res, 0, 0));
    liberarResultado(res);
    return id;
}

bool DatabaseManager::registrarVale(const ValeDB &vale)
{
    // Iniciar transaccion
    PGresult *tx = ejecutar("BEGIN");
    liberarResultado(tx);

    // Insertar cabecera del vale
    std::string idVale    = std::to_string(vale.idVale);
    std::string idEscuela = std::to_string(vale.idEscuela);

    const char *valsVale[4] = {
        idVale.c_str(),
        idEscuela.c_str(),
        vale.correo.c_str(),
        vale.fecha.c_str()
    };

    PGresult *res = ejecutarParams(
        "INSERT INTO vales (id_vale, id_escuela, correo, fecha) "
        "VALUES ($1, $2, $3, $4)",
        4, valsVale);

    if (!resultadoOk(res, "registrarVale-cabecera"))
    {
        liberarResultado(res);
        ejecutar("ROLLBACK");
        return false;
    }
    liberarResultado(res);

    // Insertar detalles
    for (const auto &det : vale.detalles)
    {
        std::string isbn  = std::to_string(det.isbn);
        std::string cant  = std::to_string(det.cantidad);
        std::string precio= std::to_string(det.precioUnit);

        const char *valsDet[4] = {
            idVale.c_str(),
            isbn.c_str(),
            cant.c_str(),
            precio.c_str()
        };

        PGresult *rd = ejecutarParams(
            "INSERT INTO detalle_vale (id_vale, isbn, cantidad, precio_unit) "
            "VALUES ($1, $2, $3, $4)",
            4, valsDet);

        if (!resultadoOk(rd, "registrarVale-detalle"))
        {
            liberarResultado(rd);
            ejecutar("ROLLBACK");
            return false;
        }
        liberarResultado(rd);
    }

    PGresult *commit = ejecutar("COMMIT");
    liberarResultado(commit);
    return true;
}

std::vector<ValeDB> DatabaseManager::valesPendientes()
{
    std::vector<ValeDB> lista;

    PGresult *res = ejecutar(
        "SELECT v.id_vale, v.id_escuela, COALESCE(v.correo,''), "
        "       v.fecha::text, v.estado "
        "FROM vales v WHERE v.estado = 'pendiente' ORDER BY v.fecha DESC");

    if (!resultadoOk(res, "valesPendientes"))
        return lista;

    int n = PQntuples(res);
    lista.reserve(n);
    for (int i = 0; i < n; i++)
    {
        ValeDB v;
        v.idVale    = std::stoi(PQgetvalue(res, i, 0));
        v.idEscuela = std::stoi(PQgetvalue(res, i, 1));
        v.correo    = PQgetvalue(res, i, 2);
        v.fecha     = PQgetvalue(res, i, 3);
        v.estado    = PQgetvalue(res, i, 4);
        lista.push_back(v);
    }
    liberarResultado(res);

    // Cargar detalles de cada vale
    for (auto &v : lista)
    {
        std::string idVale = std::to_string(v.idVale);
        const char *vals[1] = { idVale.c_str() };

        PGresult *rd = ejecutarParams(
            "SELECT isbn, cantidad, precio_unit "
            "FROM detalle_vale WHERE id_vale = $1",
            1, vals);

        if (resultadoOk(rd, "valesPendientes-detalles"))
        {
            int nd = PQntuples(rd);
            for (int j = 0; j < nd; j++)
            {
                DetalleValeDB d;
                d.isbn      = std::stoi(PQgetvalue(rd, j, 0));
                d.cantidad  = std::stoi(PQgetvalue(rd, j, 1));
                d.precioUnit= std::stod(PQgetvalue(rd, j, 2));
                v.detalles.push_back(d);
            }
        }
        liberarResultado(rd);
    }

    return lista;
}

//  REPORTES (devuelven texto formateado)
std::string DatabaseManager::reporteSemanal()
{
    PGresult *res = ejecutar(
        "SELECT id_vale, escuela, fecha, estado, "
        "       titulo, autor, cantidad, precio_unit, subtotal "
        "FROM v_reporte_semanal");

    if (!resultadoOk(res, "reporteSemanal"))
        return "Error al generar reporte.";

    std::ostringstream oss;
    oss << "=== REPORTE SEMANAL ===\n\n";

    int n = PQntuples(res);
    for (int i = 0; i < n; i++)
    {
        oss << "Vale #" << PQgetvalue(res, i, 0)
            << " | Escuela: " << PQgetvalue(res, i, 1)
            << " | Fecha: "   << PQgetvalue(res, i, 2)
            << " | Estado: "  << PQgetvalue(res, i, 3) << "\n"
            << "  Libro: "    << PQgetvalue(res, i, 4)
            << " — Autor: "   << PQgetvalue(res, i, 5)
            << " — Cant: "    << PQgetvalue(res, i, 6)
            << " — Precio: $" << PQgetvalue(res, i, 7)
            << " — Subtotal: $" << PQgetvalue(res, i, 8) << "\n\n";
    }

    if (n == 0) oss << "Sin movimientos en los ultimos 7 dias.\n";

    liberarResultado(res);
    return oss.str();
}

std::string DatabaseManager::resumenGeneral()
{
    PGresult *res = ejecutar(
        "SELECT total_libros, total_ejemplares, total_escuelas, "
        "       vales_pendientes, vales_entregados_semana, monto_semanal "
        "FROM v_resumen_general");

    if (!resultadoOk(res, "resumenGeneral") || PQntuples(res) == 0)
        return "Error al obtener resumen.";

    std::ostringstream oss;
    oss << "=== RESUMEN GENERAL ===\n\n"
        << "Titulos en catalogo:       " << PQgetvalue(res, 0, 0) << "\n"
        << "Total ejemplares:          " << PQgetvalue(res, 0, 1) << "\n"
        << "Escuelas registradas:      " << PQgetvalue(res, 0, 2) << "\n"
        << "Vales pendientes:          " << PQgetvalue(res, 0, 3) << "\n"
        << "Vales entregados (semana): " << PQgetvalue(res, 0, 4) << "\n"
        << "Monto semanal: $           " << PQgetvalue(res, 0, 5) << "\n";

    liberarResultado(res);
    return oss.str();
}

//  EXPORTAR CSV (compatibilidad con guardarInventario)
bool DatabaseManager::guardarInventarioCSV(const std::string &rutaArchivo)
{
    auto libros = librosDisponibles();

    std::ofstream f(rutaArchivo);
    if (!f.is_open())
    {
        lastError = "No se pudo abrir el archivo: " + rutaArchivo;
        return false;
    }

    f << "isbn,titulo,autor,editorial,categoria,materia,precio,cantidad,imagen_url\n";
    for (const auto &l : libros)
    {
        f << l.isbn    << ","
          << l.titulo  << ","
          << l.autor   << ","
          << l.editorial << ","
          << l.categoria << ","
          << l.materia << ","
          << l.precio  << ","
          << l.cantidad << ","
          << l.imagenUrl << "\n";
    }

    return true;
}
