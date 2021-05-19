#include <regex>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "zarpa.hpp"
#include "zarpa_types.hpp"

#include "resource_manager.hpp"

extern ResourceManager rm;

extern std::vector<std::string> Split(const std::string& str, char delim);
extern std::vector<std::string> Split(const std::string& str, const std::string& delim);
extern void ToLower(std::string& s);

void SetNoun(std::string& noun, const std::string& word);
void Parse_(const std::string& input);
std::string GetFullName(const std::string& objId);

std::mutex mtx;

using namespace std;

void Parse(const std::string& input) {

    // SOLUCIÓN TEMPORAL
    if (input == "/quit") {
        auto phr = std::make_shared<Phrase>();
        phr->verb = input;
        rm.phrases.push(phr);
        return;
    }

    //std::cout << "[PARSE]" << input << "." << std::endl;

    std::lock_guard<std::mutex> lck {mtx};
    Parse_(input);
}

//
// Descompone la entrada del usuario en estructuras Phrase
//
void Parse_(const std::string& input)
{



    // Quitar doble espacio.
    auto s = std::regex_replace(input, std::regex("  "), " ");

    // Convertir todo a minúsculas
    ToLower(s);

    // Las comas son separadores de frases: "n, examina cuenco, s, e" (3 frases)
    // Las conjunciones también: "pulsar boton y tirar palanca luego ir norte" (3 frases)
    // Hay que dividirlo todo teniendo en cuenta que se pueden combinar:
    //  "n, pulsa boton y tira palanca luego ir norte, se, baja"

    // Extraer las frases que hubiera.
    auto tokens = Split(s, ' ');
    string rawPhrase {}; 
    vector<string> rawPhrases; // Contendrá las frases "en crudo"

        for (auto t: tokens) {
        if (t.at(t.size()-1) == ',') {
            t = t.substr(0, t.size()-1);
            (rawPhrase == "") ? rawPhrase = t : rawPhrase += " " + t;
            //Es una coma crear nuevo vector
            rawPhrases.push_back(std::move(rawPhrase));
        } else if (rm.vocabularyList.WordExits(t, zarpa::wordType::conjunction)) {
            rawPhrases.push_back(std::move(rawPhrase));
        } else {
            (rawPhrase == "") ? rawPhrase = t : rawPhrase += " " + t;
        }
    }
    // FIX!
    if (rawPhrase != "") { //Hay frase en blanco al hacer "coge manzana, luego lanzala"  ("," + "luego")
        rawPhrases.push_back(std::move(rawPhrase));
    }

    // Ahora se tiene/n la/s frase/s en formato "crudo", se descompondrán en los diferentes
    // elementos, verbos, nombres, adjetivos, etc.

    shared_ptr<Phrase> phr;
    for (size_t i = 0; i < rawPhrases.size(); i++) {
        auto raw = rawPhrases[i];

        // El verbo siempre debe de estar en la primera palabra. Sólo se buscará en ella
        bool firstWord {true};
        phr = make_shared<Phrase>();
        for (auto word : Split(raw, ' ')) {
            if (firstWord) {
                firstWord = false;
                if (rm.vocabularyList.WordExits(word, zarpa::wordType::verb)) {
                    phr->verb = rm.vocabularyList.GetId(word, zarpa::wordType::verb);
                    continue;
                }

                // Si no hay verbo, se busca uno de movimiento
                if (rm.vocabularyList.WordExits(word, zarpa::wordType::mov_verb)) {
                    phr->verb = rm.vocabularyList.GetId(word, zarpa::wordType::mov_verb);
                    phr->mov_verb = phr->verb;
                    continue;
                }

                // NOTA: ESTE CÓDIGO SÓLO FUNCIONA EN ESPAÑOL
                // TODO: CREAR RUTINAS SEPARADAS POR IDIOMAS
                // Si no hemos encontrado verbo, queda la posibilidad de que esté como pronombre
                // junto a él. "cogelo, dejalo,...". Se usa el nombre de la frase anterior
                if (i > 0) {  // Lógicamente esto no funciona en la primera frase
                    auto article = rm.phrases.front()->article;
                    // Convertimos el pronombre (si lo hubiera) en el nombre anterior
                    if (article != "") {
                        //Cada ID de pronombre tiene un mismo ID que su artículo correspondiente
                        auto pos = word.find_last_of(rm.vocabularyList.GetWord(article, zarpa::wordType::pronoun));
                        if (pos != string::npos) {
                            auto possibleVerb = word.substr(0,pos-1);
                            // Si se encuentra el verbo, se actualiza también artículo, nombre y adjetivo.
                            if (rm.vocabularyList.WordExits(possibleVerb, zarpa::wordType::verb)) {
                                phr->verb = rm.vocabularyList.GetId(possibleVerb, zarpa::wordType::verb);
                                phr->article = article;
                                phr->noun = rm.phrases.front()->noun;
                                phr->adjective = rm.phrases.front()->adjective;
                                continue;
                            }
                        }
                    }
                }

            }   // if firstWord

            // Si aún no tenemos nombre, buscamos el artículo de no tenerlo. El artículo no irá después del nombre
            if (phr->noun == "" && phr->article == "") {
                if (rm.vocabularyList.WordExits(word, zarpa::wordType::article)) {
                    phr->article = rm.vocabularyList.GetId(word, zarpa::wordType::article);
                    continue;
                }
            }

            // 1er Nombre
            if (phr->noun == "") {
                    SetNoun(phr->noun, word);
                    if (phr->noun != "") {
                        // Antes, si el nombre es un nombre de objeto, lo asignamos, esto nos servirá luego
                        // para usar _OBJ
                        if (rm.vocabularyList.IdExists(phr->noun, zarpa::wordType::obj_noun)) {
                            phr->obj_noun = phr->noun;
                        }
                        continue;
                    }
            }

            // 1er Adjetivo
            // Aquí no tengo en cuenta si está ya el nombre porque según el idioma irá antes o después
            // "manzana roja", "red apple"
            if (phr->adjective == "" && phr->noun != "") { //FIXME: phr->noun?
                if (rm.vocabularyList.WordExits(word, zarpa::wordType::adjective)) {
                    phr->adjective = rm.vocabularyList.GetId(word, zarpa::wordType::adjective);
                    continue;
                }
            }

            // Preposición, después del nombre
            if (phr->preposition == "" && phr->noun != "") {
                if (rm.vocabularyList.WordExits(word, zarpa::wordType::preposition)) {
                    phr->preposition = rm.vocabularyList.GetId(word, zarpa::wordType::preposition);
                    continue;
                }
            }

            // Un segundo nombre y adjetivo, deben ir después de una preposición: "deja la manzana en la mesa"
            if (phr->preposition != "") {
                if (phr->noun2 == "") {
                    // Se comprueba antes el artículo (siempre irá antes que el nombre)
                    if (phr->article2 == "") {
                        if (rm.vocabularyList.WordExits(word, zarpa::wordType::article)) {
                            phr->article2 = rm.vocabularyList.GetId(word, zarpa::wordType::article);
                            continue;
                        }
                    }
                    SetNoun(phr->noun2, word);
                    if (phr->noun2 != "") {
                        continue;
                    }
                } else if (phr->adjective2 == "") {
                    if (rm.vocabularyList.WordExits(word, zarpa::wordType::adjective)) {
                        phr->adjective2 = rm.vocabularyList.GetId(word, zarpa::wordType::preposition);
                        continue;
                    }

                }
            }
        } //for auto word

        rm.phrases.push(phr);
    }
}


// Uso interno para simplificar la búsqueda en el bucle del parser, ya que los nombre se buscan
// en diferentes tablas, además de que puede haber 2 nombres en una sentencia.
void SetNoun(std::string& noun, const std::string& word)
{
    for (auto type : {
            zarpa::wordType::noun, 
            zarpa::wordType::obj_noun, 
            zarpa::wordType::per_noun,
            zarpa::wordType::mov_verb}) {
        if (rm.vocabularyList.WordExits(word, type)) {
            noun = rm.vocabularyList.GetId(word, type);
            break;
        }
    }
    return;
}

// Devuelve el nombre completo del objeto. Para ello se tiene en cuenta el idioma para ordenar los
// adjetivos y algunos atributos lingüísticos del objeto. "the golden key", "la llave dorada".
std::string GetFullName(const std::string& objId)
{
    auto obj = rm.objectsList.Get(objId);
    string artId {"_"}, art, name, adj;
    if (obj->adjective != "") {
        adj = rm.vocabularyList.GetWord(obj->adjective, zarpa::wordType::adjective);
    }
    name = rm.vocabularyList.GetWord(obj->noun, zarpa::wordType::obj_noun);

    //La estructura del ID es _<GENERO (M,F,N)>_<NUMERO (S,P)>
    switch (obj->gender) {
        case zarpa::gender::male: artId += "M_"; break;
        case zarpa::gender::female: artId += "F_"; break;
        case zarpa::gender::neutral: artId += "N_"; break;
        default: break; //TODO: assert!
    }
    artId += obj->isPlural ? "P" : "S";
    
    // Se busca en la tabla correcta
    if (obj->isDefinite) {
        art = rm.vocabularyList.GetWord(artId, zarpa::wordType::article);
    } else {
        art = rm.vocabularyList.GetWord(artId, zarpa::wordType::indef_article);
    }

    // Ahora hacemos tratamientos específicos según el idioma. No todos los idiomas pueden llevar
    // artículo y puede cambiar el orden de presentación.
    if (rm.adventureInfo.lang == "en") {
        // Usamos un hack con el inglés, ya que si el artículo es indefinido y adjetivo
        // empieza con vocal el artículo sería "an" en vez de "a"
        if (art == "_N_S" && !obj->isDefinite && adj.at(0) != 'a')  {
            art = rm.vocabularyList.GetWord("_N_S", zarpa::wordType::indef_article);
        }
        return (art != "" ? art + " " : "" ) + (adj != "" ? adj + " " : "" ) + name;   
    } else { // Español y quizás otros idiomas latinos como portugués o francés.
        return (art != "" ? art + " " : "" ) + name + (adj != "" ? " " + adj : "" );
    }
} 