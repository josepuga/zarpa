#ifndef C8B52EEF_E0EE_4510_8B80_EE3A4DD063CA
#define C8B52EEF_E0EE_4510_8B80_EE3A4DD063CA
#include <string>

struct Phrase
{
    std::string article;
    std::string article2;
    std::string noun;
    // Sólo activo si el nombre que se introdujo es de un objeto
    std::string obj_noun;
    std::string noun2;
    std::string verb;
    // Sólo activo si el verbo que se introdujo es de movimiento
    std::string mov_verb;
    std::string verb2;
    std::string adjective;
    std::string adjective2;
    std::string adverb; // NOT IMPLEMENTED
    std::string preposition;
    std::string conjuntion;
};


#endif /* C8B52EEF_E0EE_4510_8B80_EE3A4DD063CA */
