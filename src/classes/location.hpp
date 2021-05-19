#ifndef AC2AF4D9_83E0_4573_8B42_873467A3CAB9
#define AC2AF4D9_83E0_4573_8B42_873467A3CAB9
#include <unordered_map>
#include <string>
#include "base.hpp"
#include "zarpa_types.hpp"

struct Location
{
    std::string id; // ¿Necesario?
    std::string description;
    std::string zone;
    std::string imageFile;
    std::string musicFile;
    std::string containerOf; // Si fuera contenedor, id del objeto o entidad
    std::unordered_map<std::string, std::string> exits;
    Location();
};


#endif /* AC2AF4D9_83E0_4573_8B42_873467A3CAB9 */
