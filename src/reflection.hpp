#ifndef B02FC107_23D3_44EA_A778_6BE40BF181E6
#define B02FC107_23D3_44EA_A778_6BE40BF181E6
#include <regex>
#include <string>


// Para convertir el par objeto->dato  ("objeto.dato) en string y poder guardarlo como una
// key del mapa de variables. Devolverá la cadena "dato"
template<class T>
std::string GetMemberName(T var, const std::string& memberName)
{
    (void)var; // Deshabilita el error de unused variable
    auto s = std::regex_replace(memberName, std::regex("->"), ".");
    return s.substr(s.find(".")+1);
}
#define GETMEMBERNAME(var) GetMemberName(var, #var)

#endif /* B02FC107_23D3_44EA_A778_6BE40BF181E6 */
