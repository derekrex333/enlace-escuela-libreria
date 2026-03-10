#ifndef LIBRO_H
#define LIBRO_H

#include <string>
using namespace std;

class Libro {
public:
    Libro();
    Libro(string titulo, string autor, string editorial, string categoria, string materia, int isbn, float precio, int existencias); //materia

    // get
    string getTitulo() const;
    string getAutor() const;
    string getEditorial() const;
    string getCategoria() const;
    string getMateria() const; //se me olvido agregarlo la otra vez jajaj
    int getIsbn() const;
    float getPrecio() const;
    int getExistencias() const;
    string getImagenUrl() const { return imagenUrl; }

    // set
    void setTitulo(string titulo);
    void setAutor(string autor);
    void setEditorial(string editorial);
    void setCategoria(string categoria);
    void setMateria(string materia); //se me olvido agregarlo la otra vez jajaj
    void setIsbn(int isbn);
    void setPrecio(float p);
    void setExistencias(int e);

    // funcional
    void mostrarDatos() const;
    bool reducirExistencias(int cantidad);
    bool aumentarExistencias(int cantidad);

    string toCsv ()const;
    static Libro fromCsv(const string& csvLine);

private:
    string titulo;
    string autor;
    string editorial;
    string categoria;
    string materia; //se me olvido agregarlo la otra vez jajaj
    string imagenUrl = "";
    int isbn;
    float precio;
    int existencias;

};

#endif // LIBRO_H
