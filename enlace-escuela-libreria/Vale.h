#ifndef VALE_H
#define VALE_H

#include <string>
#include <vector>
using namespace std;

class Vale {
private:
    int idVale;
    int idEscuela;
    string listaLibros;     // Formato: "ISBN1:cant1; ISBN2:cant2; ..."
    float total;
    string fechaEmision;
    string correo;
    int cantidadExistente;  // Número de libros en el vale
    bool pagado;

public:
    Vale();
    Vale(int idVale, int idEscuela, string listaLibros, float total,
         string fechaEmision, string correo, bool pagado = false);

    // Getters
    int getIdVale() const;
    int getIdEscuela() const;
    string getListaLibros() const;
    float getTotal() const;
    string getFechaEmision() const;
    string getCorreo() const;
    int getCantidadExistente() const;
    bool getPagado() const;

    // Setters
    void setIdVale(int id);
    void setIdEscuela(int id);
    void setListaLibros(string lista);
    void setTotal(float t);
    void setFechaEmision(string fecha);
    void setCorreo(string c);
    void setCantidadExistente(int cant);
    void setPagado(bool p);

    // Métodos funcionales
    void mostrarVale() const;
    vector<pair<int, int>> parsearListaLibros() const;  // Convierte string a vector<ISBN, cant>
};

#endif // VALE_H
