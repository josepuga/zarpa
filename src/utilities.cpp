#include <string>
#include <vector>
#include <regex>
#include <codecvt>
#include <unordered_map>


std::string WStringToString(const std::wstring& wstr);
std::wstring StringToWString(const std::string& str);

/*
C++ no tiene una forma estándar de convertir caracteres no ascii de mayús. a minús. y viceversa. 
Me he tenido que crear esta función que no está muy optimizada y tampoco funciona con todos los
caracteres (hay que especificarlos manualmente). Sólo está adaptado para los españoles.
Hay métodos más rapidos usando locale, pero me han dado muchos problemas bajo Windows.
*/

void ToLower(std::string& s)
{
    // Necesaria la conversión string->wstring->string para poder pasar a minúsculas caraceres utf8 no ascii
    auto ws = StringToWString(s);
 
    std::unordered_map<wchar_t, wchar_t> substMap {
        // Caracteres castellanos: ÁÉÍÓÚÜÑ
        {L'Á', L'á'}, {L'É', L'é'}, {L'Í', L'í'},{L'Ó', L'ó'}, {L'Ú', L'ú'}, {L'Ü', L'ü'}, {L'Ñ', L'ñ'}
    };
    for (auto it = ws.begin(); it != ws.end(); ++it) {
        auto oldChar = *it;
        *it = towlower(*it);
        // Si no ha cambiado, se prueba si es un válido del idioma.
        if (oldChar == *it) {
            if (substMap.find(*it) != substMap.end()) {
                    *it = substMap[*it];
            }
        }
    }
    s = WStringToString(ws);    
}

// Snipper extraido de https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
// Gracias al usuario Martin Ba
std::wstring StringToWString(const std::string& str)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

std::string WStringToString(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}


void Trim(std::string& s)
{
    static std::regex e("^\\s+|\\s+$");
    //LTrim only std::regex_replace(s, std::regex("^\\s+"), "");
    //RTrim only std::regex_replace(s, std::regex("\\s+$"), "");
    s = std::regex_replace(s, e, "");
}    


// buen hack!. gracias a hellow
// https://stackoverflow.com/questions/2844817/how-do-i-check-if-a-c-string-is-an-int/37864920
bool IsNumber(const std::string& s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   strtol(s.c_str(), &p, 10);
   return (*p == 0);
}

// Un método bueno que encontré para dividir frases, mejor que el que yo usaba con stringstreams
// https://www.techiedelight.com/split-string-cpp-using-delimiter/
std::vector<std::string> Split(const std::string& str, const char delim) 
{
    std::vector<std::string> strings;
    size_t start;
    size_t end = 0;
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
        end = str.find(delim, start);
        strings.push_back(str.substr(start, end - start));
    }
    return strings;
}


std::vector<std::string> Split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> strings;
    size_t start;
    size_t end = 0;
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
        end = str.find(delim, start);
        strings.push_back(str.substr(start, end - start));
    }
    return strings;    
}


// Snipper de StackOverflow. Me gusta porque gestiona muy bien la memoria evitando copias
// https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
void ReplaceAll(std::string& source, const std::string& from, const std::string& to)
{
    std::string newString;
    newString.reserve(source.length());  // avoids a few memory allocations

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while(std::string::npos != (findPos = source.find(from, lastPos)))
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    // Care for the rest after last occurrence
    //newString += source.substr(lastPos);
    newString.append(source, lastPos, source.length() - lastPos);

    source.swap(newString);
}