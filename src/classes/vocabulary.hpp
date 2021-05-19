#ifndef C154E67F_35A4_432E_8863_A4C69F99BFF6
#define C154E67F_35A4_432E_8863_A4C69F99BFF6
#include <string>
#include <vector>
#include "zarpa_types.hpp"

struct Vocabulary
{
    std::string id;
    zarpa::wordType type;
    std::vector<std::string> words;
};


#endif /* C154E67F_35A4_432E_8863_A4C69F99BFF6 */
