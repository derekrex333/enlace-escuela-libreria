#ifndef ESCUELA_H
#define ESCUELA_H

#include <string>
#include <vector>
#include "Vale.h"
using namespace std;

class Escuela
{
    public:
        Escuela();
        Escuela(string nombre, string ubicacion, int telefono, string correo, int idEscuela, string fecha = "");
        //get
        string getNombre() const;
        int getIdEscuela() const;
        const vector<Vale>& getListaVales () const;
        string getUbicacion() const;
        int getTelefono() const;
        string getCorreo() const;
        string getFecha() const;
        //set
        void setNombre (string nombre);
        void setIdEscuela(int id);
        void setUbicacion(string ubicacion);
        void setTelefono(int telefono);
        void setCorreo(string correo);
        void setFecha(string fecha);
        //funcion
        void agregarVale(const Vale& vale);
        float calcularTotalPagado() const;
        float calcularTotalPendiente() const;
        void consultarHistoriaVales() const;
        void mostrarDatos() const;

    private:
        string nombre;
        int idEscuela;
        vector<Vale> listaVales;
        string ubicacion;
        int telefono;
        string correo;
        string fecha;
};

#endif // ESCUELA_H
