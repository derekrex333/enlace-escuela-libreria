#include "CsvUtils.h"
#include <iostream>

using namespace std;

vector<string> CsvUtils::split(const string& s, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

string CsvUtils::join(const vector<string>& v, char delimiter)
{
    string result;
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i > 0) result += delimiter;
        result += v[i];
    }
    return result;
}

string CsvUtils::escapeCsv(const string& s)
{
    bool necesitaComillas = (s.find(',') != string::npos ||
                             s.find('"') != string::npos ||
                             s.find('\n') != string::npos ||
                             s.find('\r') != string::npos);

    if (!necesitaComillas)
        return s;

    string escaped = "\"";
    for (char c : s)
    {
        if (c == '"')
            escaped += "\"\"";
        else
            escaped += c;
    }
    escaped += "\"";
    return escaped;
}

string CsvUtils::unescapeCsv(const string& s)
{
    if (s.size() < 2 || s.front() != '"' || s.back() != '"')
    {
        return s;
    }

    string result = s.substr(1, s.size() - 2);
    size_t pos = 0;
    while ((pos = result.find("\"\"", pos)) != string::npos)
    {
        result.replace(pos, 2, "\"");
        pos += 1;
    }
    return result;
}

void CsvUtils::asegurarCarpetaData()
{
    if (!filesystem::exists("data"))
    {
        filesystem::create_directories("data");
        cout << "Carpeta 'data' creada exitosamente!\n";
    } else
    {
        cout << "Carpeta 'data' ya existe.\n";
    }
}

string CsvUtils::trim(const string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}
