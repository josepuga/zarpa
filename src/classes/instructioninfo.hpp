#ifndef BC94430E_D705_4AA8_8AE5_FFEAED8FF67A
#define BC94430E_D705_4AA8_8AE5_FFEAED8FF67A

#include <array>
#include <vector>
#include "zarpa_types.hpp"

struct InstructionInfo
{
    InstructionInfo();
    bool conditional {false};

    // Parámetros permitidos usando aritmética binaria. El índice es el número de parámetro
    // y el valor es el entero resultante de usar el AND lógico con los tipos permitidos
    std::array<zarpa::paramType, zarpa::max_params> paramsType;

    // Lo mismo que el anterior sólo que en vez de está codificado en un entero mediante aritm. bin.
    // se usa un vector. El vector es más eficiente porque hay que usar un bucle con todos los tipos
    // de parám. posibles a la hora de "decodificar" una linea. Esto es así porque un mismo parámetro
    // puede ser de diferente tipo y hay que recorrer los mapas para encontrar su ID y ubicarlo
    std::array<std::vector<zarpa::paramType>, zarpa::max_params> vecParamsType;

    // Hace ambas operaciones, añade por bitwise y por vector
    void AddParamType(zarpa::paramType type, const size_t index);

    bool CouldBe(zarpa::paramType type, const size_t index)
        {return (paramsType[index] & type) == type;}


};


#endif /* BC94430E_D705_4AA8_8AE5_FFEAED8FF67A */
