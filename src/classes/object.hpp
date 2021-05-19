#ifndef E5D3C138_B172_4E81_A504_C3082CB472C8
#define E5D3C138_B172_4E81_A504_C3082CB472C8
#include <string>
#include "base.hpp"
#include "zarpa_types.hpp"


struct Object : public Base
{
    std::string id;
    uint_t state {0};
    std::string location;

    std::string noun;
    std::string adjective;
    zarpa::gender gender {zarpa::gender::neutral};
    bool isDefinite {false};
    bool isPlural {false};    
    std::string exam;

    uint_t weight {0};
    bool isCarriable {false};
    bool isWearable {false};
    std::string container;    



};


#endif /* E5D3C138_B172_4E81_A504_C3082CB472C8 */
