#include "Libro.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "CsvUtils.h"
using namespace std;

Libro::Libro() : isbn(0), precio(0.0f), existencias(0) {}

Libro::Libro(string titulo, string autor, string editorial, string categoria,
             string materia, int isbn, float precio, int existencias)
    : titulo(titulo), autor(autor), editorial(editorial), categoria(categoria),
      materia(materia), isbn(isbn), precio(precio), existencias(existencias) {}

// get
string Libro::getTitulo() const     { return titulo; }
string Libro::getAutor() const      { return autor; }
string Libro::getEditorial() const  { return editorial; }
string Libro::getCategoria() const  { return categoria; }
string Libro::getMateria() const    { return materia;} //existo muajaja
int    Libro::getIsbn() const       { return isbn; }
float  Libro::getPrecio() const     { return precio; }
int    Libro::getExistencias() const{ return existencias; }

// set
void Libro::setTitulo(string t)     { titulo = t; }
void Libro::setAutor(string a)      { autor = a; }
void Libro::setEditorial(string e)  { editorial = e; }
void Libro::setCategoria(string c)  { categoria = c; }
void Libro::setMateria(string m)    { materia = m; } //y yo tambien
void Libro::setIsbn(int i)          { isbn = i; }
void Libro::setPrecio(float p)      { precio = p; }
void Libro::setExistencias(int e)   { existencias = e; }

// funcional
void Libro::mostrarDatos() const
{
    cout << "ISBN: " << isbn << "\n";
    cout << "Título: " << titulo << "\n";
    cout << "Autor: " << autor << "\n";
    cout << "Editorial: " << editorial << "\n";
    cout << "Categoría: " << categoria << "\n";
    cout << "Materia: " << materia << "\n"; //listo
    cout << "Precio: $" << fixed << setprecision(2) << precio << "\n";
    cout << "Existencias: " << existencias << "\n";
}

bool Libro::reducirExistencias(int cantidad)
{
    if (cantidad > existencias) return false;
    existencias -= cantidad;
    return true;
}

bool Libro::aumentarExistencias(int cantidad)
{
    if (cantidad < 0) return false;
    existencias += cantidad;
    return true;
}

string Libro::toCsv() const
{
    return CsvUtils::escapeCsv(titulo) + "," +
           CsvUtils::escapeCsv(autor) + "," +
           CsvUtils::escapeCsv(editorial) + "," +
           CsvUtils::escapeCsv(categoria) + "," +
           CsvUtils::escapeCsv(materia) + "," +
           to_string(isbn) + "," +
           to_string(precio) + "," +
           to_string(existencias);
}

Libro Libro::fromCsv(const string& csvLine) {
    vector<string> campos = CsvUtils::split(csvLine, ',');
    if (campos.size() < 8) {
        throw runtime_error("lineea incompleta :( "); //:) oye mi amor
    }
    for (auto& campo : campos) {
        campo = CsvUtils::unescapeCsv(campo);
    }

    return Libro(
        campos[0], campos[1], campos[2], campos[3], campos[4],
        stoi(campos[5]), stof(campos[6]), stoi(campos[7])
    );
}
