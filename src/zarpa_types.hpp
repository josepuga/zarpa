#ifndef C064F25E_A182_4275_A3D1_0842B0A64146
#define C064F25E_A182_4275_A3D1_0842B0A64146
#include <cstdint>
#include <vector>
#include <string>
#include "enum_bitmask.hpp"

using int_t = int32_t;
using uint_t = uint32_t;

namespace zarpa
{
    const std::string INI_FILE = "zarpa.ini";


    //
    // Código Zarpascript
    //

    // Máximo número de parámetros para las instrucciones
    const size_t max_params = 2;

    // Tipo de parámetro para las instrucciones
    enum class paramType : uint32_t {
        no_param = 0,        //NO param at all
        reserved = 1 << 1,  
        num = 1 << 2,
        obj = 1 << 3,
        loc = 1 << 4, 
        var = 1 << 6,
        desc = 1 << 7,
        msg = 1 << 8,
        zone = 1 << 9,
        word_noun = 1 << 10,
        word_adj = 1 << 11,
        word_prep = 1 << 12,
        word_verb = 1 << 13,
        word_adv = 1 << 14,
        word_obj_noun = 1 << 16,
        word_mov_verb = 1 << 17,
        sound = 1 << 18,
        ent = 1 << 19,
        _lastone = 1 << 20
    };


    // Tipos de variables. Determina el comportamiento en cada turno.
    enum varType : uint_t {
        normal, increase, decrease
    };

    // Tipos de tablas. Para saber cuándo y en qué orden se ejecutará cada una.
    // Tipos de tablas
    enum tableType : uint_t {
        systemBeforeDesc, systemAfterDesc, 
        userBeforeDesc, userAfterDesc, 
        systemBeforePrompt, systemAfterPrompt,
        userBeforePrompt, userAfterPrompt,
        tableType_count
    };    

    // Estados al salir de una tabla
    enum scriptState : uint_t {
        running,    // Aún no se ha salido (se sigue procesando)
        paused,
        done, notdone, desc, 
        endTable         // Fin de tabla
    };


    //
    // Vocabulario
    //

    // Salidas posibles de una localidad (coincide también con los IDs de verbos de movimiento)
    const std::vector<std::string> exits {
        "N", "NE", "E", "SE", "S", "SW", "W", "NW", "N",
        "UP", "DOWN"};
    const size_t max_exits = sizeof(exits);

    // Género (para tratar objetos)
    enum gender : uint_t {
        neutral, male, female
    };

    // Tipos de palabra, el parser necesita clasificarlas
    enum wordType : size_t {
        adjective, 
        adverb, 
        article, 
        conjunction, 
        indef_article, 
        mov_verb, 
        noun, 
        obj_noun, 
        per_noun, 
        preposition, 
        pronoun, 
        var, 
        verb, 
        debug, 
        wordType_count
    };


    //
    // Otros
    //

    // Tipos de errores
    enum errorType : uint_t {
        warning, error, fatal_error, dev_error, errorType_count
    };

    // Tamaños para los reserve de los map/unordered_map, intento de optimizar un poquito...
    enum reserves : size_t {
        descs = 70,
        ents = 10,          
        locs = 60,          // Localidades
        msgs = 150,
        objs = 25,
        vars = 200,         // Variables
        // Para el vocabulario, los diferentes tipos de palabra se separan en un array de mapas 
        // por lo que nombres y verbos son los que tendrán un tamaño mayor.
        id_verbs = 40,      // IDs de verbos
        word_verbs = 100,   // Palabras de verbos
        id_nouns = 100,
        word_nouns = 300,
    };


} // namespace zarpa

    //https://gist.github.com/StrikerX3/46b9058d6c61387b3f361ef9d7e00cd4
    ENABLE_BITMASK_OPERATORS(zarpa::paramType)


// Hash utilizado por en las instrucciones de Zarpascript por 2 razones:
// 1. Más claridad de código al poder usar case en vez de if/else if
// 2. La comparación del hash es más eficiente que comparar strings, sobretodo en los bucles
// del script, que se llaman muchísimo durante la aventura.
// case "cadena"_hash
// Gracias a https://programmersought.com/article/17731356759/
typedef std::uint64_t hash_t;  
constexpr hash_t prime = 0x100000001B3ull;  
constexpr hash_t basis = 0xCBF29CE484222325ull;     
constexpr hash_t hash_compile_time(char const* str, hash_t last_value = basis)  
{  
    return *str ? hash_compile_time(str+1, (*str ^ last_value) * prime) : last_value;  
} 

constexpr unsigned long long operator "" _hash(char const* p, size_t)  
{  
    return hash_compile_time(p);  
}

inline hash_t hash_(char const* str)
{  
    hash_t ret{basis};    
    while(*str){  
        ret ^= *str;  
        ret *= prime;  
        str++;  
    }    
    return ret;  
}


#endif /* C064F25E_A182_4275_A3D1_0842B0A64146 */
