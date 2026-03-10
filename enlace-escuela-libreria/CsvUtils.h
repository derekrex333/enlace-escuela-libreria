#ifndef CSVUTILS_H // Esto evita que el archivo se incluya dos veces por error (include guard).
#define CSVUTILS_H
#include <string>
#include <vector>
#include <sstream>     // Permite separar y combinar texto
#include <filesystem>  // Permite crear carpetas y verificar si existen

using namespace std;

class CsvUtils
{
public:
    static vector<string> split(const string& s, char delimiter = ','); // Divide una cadena usando un delimitador (por defecto una coma).
    static string join(const vector<string>& v, char delimiter = ','); // Une los elementos de un vector en un solo string separado por un delimitador.
    static string escapeCsv(const string& s); // Escapa comas, comillas y saltos de línea según el estándar CSV.
    static string unescapeCsv(const string& s); // Revierte el escape de un campo CSV (quita comillas, etc.)
    static void asegurarCarpetaData(); // Crea la carpeta “data” si no existe.
    static string trim(const string& str);  // Elimina espacios al inicio y al final de un string.
};

#endif // CSVUTILS_H
