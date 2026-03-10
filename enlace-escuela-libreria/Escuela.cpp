#include "Escuela.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;

Escuela::Escuela():idEscuela(0), telefono(0){}

Escuela::Escuela(string nombre, string ubicacion, int telefono, string correo, int idEscuela, string fecha)
    : nombre(nombre), ubicacion(ubicacion), telefono(telefono), correo(correo), idEscuela(idEscuela), fecha(fecha) {}

//get
string Escuela::getNombre() const
{
    return nombre;
}
int Escuela::getIdEscuela() const
{
    return idEscuela;
}
const vector<Vale>& Escuela::getListaVales() const
{
    return listaVales;
}
string Escuela::getUbicacion() const
{
    return ubicacion;
}
int Escuela::getTelefono() const
{
    return telefono;
}
string Escuela::getCorreo() const
{
    return correo;
}
string Escuela::getFecha() const
{
    return fecha;
}

//set
void Escuela::setNombre(string n)
{
    nombre = n;
}
void Escuela::setIdEscuela(int id)
{
    idEscuela = id;
}
void Escuela::setUbicacion(string u)
{
    ubicacion = u;
}
void Escuela::setTelefono(int t)
{
    telefono = t;
}
void Escuela::setCorreo(string c)
{
    correo = c;
}
void Escuela::setFecha(string f)
{
    fecha = f;
}

//funciones
void Escuela::agregarVale(const Vale&vale)
{
    cout<<"\n----AGREGANDO NUEVO VALE----\n";
    listaVales.push_back(vale);
    cout<<"Vale agregado correctamente a la escuela: "<<nombre<<"\n";
    cout<<"Total de vales acumulados: "<<listaVales.size()<<"\n";
    cout<<"-----------------------------\n";
}

float Escuela::calcularTotalPagado()const
{
    cout<<"\n----CALCULANDO TOTAL PAGADO----\n";
    float total = 0.0f;
    for (const auto& v : listaVales)
    {
        if (v.getPagado())
        {
            cout<<"Pago total por vale: $"<<v.getTotal()<<"\n";
            total += v.getTotal();
        }
    }
    cout<<"\n----------------------\n";
    cout<<"CANTIDAD TODAL PAGADA: $"<<fixed<<setprecision(2)<<total<<"\n";
    cout<<"-------------------------\n";
    return total;
}

float Escuela::calcularTotalPendiente()const
{
    cout<<"----CANTIDAD PENDIENTE A PAGAR----\n";
    float total = 0.0f;
    for (const auto& v : listaVales)
    {
        if (!v.getPagado())
        {
            cout<<"Pendiente por pagar: $"<<v.getTotal()<<"\n";
            total += v.getTotal();
        }
    }
    cout<<"Total pendiente a pagar: $"<<fixed<<setprecision(2)<<total<<"\n";
    cout<<"FAVOR DE REALIZAR SU PAGO\n";
    cout<<"Gracias :) ";
    cout<<"--------------------------\n";
    return total;
}

void Escuela::consultarHistoriaVales()const
{
     cout << "\n----HISTORIAL DE VALES----\n";
    if (listaVales.empty())
    {
        cout << "No hay vales registrados para esta escuela\n";
        return;
    }

    cout<<"Escuela: "<<nombre<<"(ID: "<< idEscuela<<" )\n\n";
    cout<<"Total de vales registrados: "<<listaVales.size()<<"\n\n";
    for(size_t i=0; i<listaVales.size(); i++)
    {
       cout<<"Vale número "<<i+1<<":\n";
        cout<<"*************\n";
        listaVales[i].mostrarVale();
        cout<<"-----------------------\n";
    }
    cout<<"FIN DEL HISTORIAL.....\n\n"; //error <<< corregi por <<
}

void Escuela::mostrarDatos()const
{
    cout<<"\n----INFORMACIÓN DE LA ESCUELA----\n";
    cout << "Escuela: " << nombre << " (ID: " << idEscuela << ")\n";
    cout << "Ubicación: " << ubicacion << "\n";
    cout << "Teléfono: " << telefono << "\n";
    cout << "Correo: " << correo << "\n";
    cout << "Fecha de registro: " << (fecha.empty() ? "No especificada" : fecha) << "\n";
    cout << "Total pagado: $" << fixed << setprecision(2) << calcularTotalPagado() << "\n";
    cout << "Total pendiente: $" << fixed << setprecision(2) << calcularTotalPendiente() << "\n";
}
