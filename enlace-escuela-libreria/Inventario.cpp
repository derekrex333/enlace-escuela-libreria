#include "Inventario.h"
#include <iostream>
#include <fstream>  //Librería añadida para manejo de archivos.
#include <sstream>
#include <stdexcept>
#include <algorithm> //https://www.youtube.com/watch?v=bB9B1J0ZObc
                     //el video explica algoritmo como ordena, encuentra, etc

Inventario::Inventario()
{ //constructor
    proximoId = 1;
}

void Inventario::registrarLibro(string titulo, string autor, int isbn, string editorial, string categoria, string materia, float precio, int cantidad)
{
    if (!validarDatos(titulo, autor))
    {
        cout << "Error, los datos son invalidos" << endl;
        return;
    }
    Libro nuevoLibro(titulo, autor, editorial, categoria, materia, isbn, precio, cantidad); //materia
    libros[isbn] = nuevoLibro;  // clave = ISBN
    cout << "Libro registrado con ISBN: " << isbn << endl;
}

Libro Inventario::consultarLibro(int id) const
{ //consultar libro por id
    if (existeId(id))
    {
        return libros.at(id); // usamos .at() para const
    }
    cout << "Error, libro no encontrado" << endl;
    return Libro(); //espacio vacio ya que es un objeto y no un puntero
}

list<Libro> Inventario::consultarPorTitulo(string titulo) const
{ //consultar por Titulo (ayuda de claude.ia solo en este solamente para que me explicara como hacer la consulta)
    list<Libro> resultado;
    for (auto& par : libros)
    {
        if (par.second.getTitulo().find(titulo) != string::npos)
        {
            resultado.push_back(par.second);
        }
    }
    return resultado;
}

list<Libro> Inventario::consultarPorAutor(string autor) const
{ //consulta por autor
    list<Libro> resultado;
    for (auto& par : libros)
    {
        if (par.second.getAutor().find(autor) != string::npos)
        {
            resultado.push_back(par.second);
        }
    }
    return resultado;
}

list<Libro> Inventario::consultarPorEditorial(string editorial) const
{ //consulta por editorial
    list<Libro> resultado;
    for (auto& par : libros)
    {
        if (par.second.getEditorial() == editorial)
        {
            resultado.push_back(par.second);
        }
    }
    return resultado;
}

list<Libro> Inventario::consultarPorCategoria(string categoria) const
{ //consulta por categoria
    list<Libro> resultado;
    for (auto& par : libros)
    {
        if (par.second.getCategoria() == categoria)
        {
            resultado.push_back(par.second);
        }
    }
    return resultado;
}

list<Libro> Inventario::consultarPorMateria(string materia) const
{ //consulta por materia
    list<Libro> resultado;
    for (auto& par : libros)
    {
        if (par.second.getMateria() == materia)
        {
            resultado.push_back(par.second);
        }
    }
    return resultado;
}

bool Inventario::modificarLibro(int id, float precio)
{ //modificar libro(precio)
    if (!existeId(id))
    {
        cout << "Error, libro no encontrado" << endl;
        return false;
    }
    if (precio <= 0)
    {
        cout << "Error, precio invalido" << endl;
        return false;
    }
    libros[id].setPrecio(precio);
    cout << "Precio actualizado exitosamente" << endl;
    return true;
}

bool Inventario::eliminarLibro(int id)
{ //eliminar libro
    if (!existeId(id))
    {
        cout << "Error, libro no encontrado" << endl;
        return false;
    }
    libros.erase(id); //.erase, metodo de contenedores
    cout << "Libro eliminado exitosamente" << endl;
    return true;
}

list<Libro> Inventario::listarTodos() const
{ //listar todos los libros
    list<Libro> resultado;
    for (auto& par : libros)
    {
        resultado.push_back(par.second);
    }
    return resultado;
}

bool Inventario::actualizarStock(int id, int cantidad)
{ //actualizar stock, aumentar o disminuir
    if (!existeId(id))
    {
        cout << "Error, libro no encontrado" << endl;
        return false;
    }
    int stockActual = libros[id].getExistencias();
    int nuevoStock = stockActual + cantidad;

    if (nuevoStock < 0)
    {
        cout << "Error, stock insuficiente" << endl;
        return false;
    }
    libros[id].setExistencias(nuevoStock);
    return true;
}

list<Libro> Inventario::librosDisponibles() const
{ //libros disponibles
    list<Libro> resultado;
    for (auto& par : libros)
    {
        if (par.second.getExistencias() > 0)
        {
            resultado.push_back(par.second);
        }
    }
    return resultado;
}

bool Inventario::validarDatos(string titulo, string autor) const
{ //validar datos
    if (titulo.empty() || autor.empty())
    {
        return false;
    }
    return true;
}

bool Inventario::existeId(int id) const
{ //verificar si existe la id
    return libros.find(id) != libros.end();
}

void Inventario::guardarCsv(const string& archivo)
{
    ofstream file(archivo);
    if (!file.is_open())
    {
        cerr << "Error al abrir " << archivo << endl;
        return;
    }
    file << "titulo,autor,editorial,categoria,materia,isbn,precio,existencias\n";
    for (const auto& par : libros)
    {
        file << par.second.toCsv() << "\n";
    }
    file.close();
    cout << "Inventario guardado en " << archivo << endl;
}

void Inventario::cargarCsv(const string& archivo)
{
    ifstream file(archivo);
    if (!file.is_open())
    {
        cout << "No existe archivo " << archivo << ", empezando inventario vacío.\n";
        return;
    }
    string line;
    getline(file, line); //salto de cabezera (no me digas que no)
    while (getline(file, line))
    {
        if (line.empty()) continue;
        try
        {
            Libro libro = Libro::fromCsv(line);
            libros[libro.getIsbn()] = libro;
            proximoId = max(proximoId, libro.getIsbn() + 1);
        }
        catch (const exception& e)
        {
            cerr << "Error cargando línea: " << line << " (" << e.what() << ")\n";
        }
    }
    file.close();
}

int Inventario::getIdPorIsbn(int isbn) const
{
    for (const auto& par : libros)
    {
        if (par.second.getIsbn() == isbn)
        {
            return par.first; // devuelve el ID
        }
    }
    return -1; // no encontrado
}
