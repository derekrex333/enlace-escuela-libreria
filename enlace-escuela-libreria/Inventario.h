#ifndef INVENTARIO_H
#define INVENTARIO_H
#include <map> //diccionarios
#include <list>
#include <string>
#include "Libro.h"
using namespace std;

class Inventario
{
    public:
        Inventario();
        void registrarLibro(string titulo, string autor, int isbn, string editorial, string categoria, string materia, float precio, int cantidad);
        Libro consultarLibro(int id) const;
        int getIdPorIsbn(int isbn) const;
        list<Libro> consultarPorTitulo(string titulo) const;
        list<Libro> consultarPorAutor(string autor) const;
        list<Libro> consultarPorEditorial(string editorial) const;
        list<Libro> consultarPorCategoria(string categoria) const;
        list<Libro> consultarPorMateria(string materia) const;
        bool modificarLibro(int id, float precio);
        bool eliminarLibro(int id);
        list<Libro> listarTodos() const;
        bool actualizarStock(int id, int cantidad);
        list<Libro> librosDisponibles() const;
        bool validarDatos(string titulo, string autor) const;
        bool existeId(int id) const;
        void guardarCsv(const string& archivo = "data/inventario.csv");
        void cargarCsv(const string& archivo = "data/inventario.csv");

    private:
        map<int, Libro> libros; //https://www.youtube.com/watch?v=w7OwilQ_3XM
        int proximoId;          //este es el video que me ayudo a entender los diccionarios en c++
};

#endif // INVENTARIO_H
