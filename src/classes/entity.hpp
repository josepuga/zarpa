#ifndef AFBB420B_514E_4073_A5C6_ECA40819C3F7
#define AFBB420B_514E_4073_A5C6_ECA40819C3F7
#include <string>
#include "zarpa_types.hpp"
/**
 * He creado una clase común tanto para el jugador como para el resto de entidades, veo más
 * complicado crear una clase base, y derivar un jugador y una entidad. A efectos de juego y de
 * buscar las variables serializadas sería mucho peor.
 * */

struct Entity
{
    std::string id; 
    std::string startLocation;
    std::string location;
    std::string zone;    
    std::string prevLocation;  // Localidad del turno anterior
    std::string container;    
    std::string varResetOnNewLoc;
    std::string varFollowPlayer;
};


#endif /* AFBB420B_514E_4073_A5C6_ECA40819C3F7 */
