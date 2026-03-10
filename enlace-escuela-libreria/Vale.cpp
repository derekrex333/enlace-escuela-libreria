#include "Vale.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

Vale::Vale() : idVale(0), idEscuela(0), total(0.0f), cantidadExistente(0), pagado(false) {}

Vale::Vale(int idV, int idE, string lista, float t, string fecha, string c, bool p): idVale(idV), idEscuela(idE), listaLibros(lista), total(t),
      fechaEmision(fecha), correo(c), pagado(p)
      {
    // Contar cantidad de libros en la lista
    cantidadExistente = 0;
    stringstream ss(lista);
    string item;
    while (getline(ss, item, ';'))
    {
        if (!item.empty()) cantidadExistente++;
    }
}

//get
int Vale::getIdVale() const
{
    return idVale;
}
int Vale::getIdEscuela() const
{
    return idEscuela;
}
string Vale::getListaLibros() const
{
    return listaLibros;
}
float Vale::getTotal() const
{
    return total;
}
string Vale::getFechaEmision() const
{
    return fechaEmision;
}
string Vale::getCorreo() const
{
    return correo;
}
int Vale::getCantidadExistente() const
{
    return cantidadExistente;
}
bool Vale::getPagado() const
{
    return pagado;
}

//set
void Vale::setIdVale(int id)
{
    idVale = id;
}
void Vale::setIdEscuela(int id)
{
    idEscuela = id;
}
void Vale::setListaLibros(string lista)
{
    listaLibros = lista;
}
void Vale::setTotal(float t)
{
    total = t;
}
void Vale::setFechaEmision(string fecha)
{
    fechaEmision = fecha;
}
void Vale::setCorreo(string c)
{
    correo = c;
}
void Vale::setCantidadExistente(int cant)
{
    cantidadExistente = cant;
}
void Vale::setPagado(bool p)
{
    pagado = p;
}

//mostrar vale
void Vale::mostrarVale() const {
    cout << "VALE #" << idVale << "\nEscuela ID: " << idEscuela << "\nFecha: " << fechaEmision << "\nCorreo: " << correo << "\nTotal: $" << fixed << setprecision(2) << total << "\nEstado: " << (pagado ? "PAGADO" : "PENDIENTE") << "\nLibros (" << cantidadExistente << "):\n";
    vector<pair<int, int>> libros = parsearListaLibros();
    for (const auto& par : libros)
    {
        cout << "  - ISBN: " << par.first << " | Cantidad: " << par.second << "\n";
    }
}

// Parsear lista de libros (string → vector<pair<ISBN, cantidad>>) ayuda de un gran modelo del lenguaje no me salia
vector<pair<int, int>> Vale::parsearListaLibros() const {
    vector<pair<int, int>> resultado;
    if (listaLibros.empty()) return resultado;

    stringstream ss(listaLibros);
    string item;
    while (getline(ss, item, ';')) {
        if (item.empty()) continue;
        size_t pos = item.find(':');
        if (pos == string::npos) continue;

        try {
            int isbn = stoi(item.substr(0, pos));
            int cant = stoi(item.substr(pos + 1));
            resultado.emplace_back(isbn, cant);
        } catch (...) {
            // Ignorar errores de parseo
        }
    }
    return resultado;
}
