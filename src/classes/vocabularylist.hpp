#ifndef D10D01B9_ADDF_44D1_88C0_537706F343F3
#define D10D01B9_ADDF_44D1_88C0_537706F343F3

// Lista que contendrá diversas palabras de vocabulario, permite duplicidad
// de keys para los sinónimos

#include <string>
#include <unordered_map>
#include <array>
#include <memory>
#include "zarpa_types.hpp"
#include "vocabulary.hpp"


template <size_t DIM>
class VocabularyList
{
    /*
    https://thispointer.com/finding-all-values-for-a-key-in-multimap-using-equals_range-example/
    https://stackoverflow.com/questions/54276467/check-if-value-already-exists-in-multimap-c
    */
    public:
    VocabularyList();
    // Inserta un objeto vocabulario
    void Add(std::shared_ptr<Vocabulary> voc);

    std::shared_ptr<Vocabulary> GetVocById(const std::string& id,  zarpa::wordType type);
    std::string GetId(const std::string& word,  zarpa::wordType type)
            {return m_wordToVocMap[type][word]->id;}
    std::string GetWord(const std::string& id,  zarpa::wordType type)
            {return m_idToWordMap[type].find(id)->second;}
    bool IdExists(const std::string& id, zarpa::wordType type) 
            {return m_idToWordMap[type].find(id) != m_idToWordMap[type].end();}
    bool WordExits(const std::string& word, zarpa::wordType type) 
            {return m_wordToVocMap[type].find(word) != m_wordToVocMap[type].end();}
    size_t IdsCount(size_t type); //FIXME: No funciona del todo bien
    size_t WordsCount(size_t type) {return m_wordToVocMap[type].size();}


    private:
    /*
    La forma sencilla, elegante y rápida, es tener 2 mapas cruzados de ID->Palabra/s y Palabra/s->ID.
    - multimap<id, word>: Multimap porque el ID se repetirá por cada sinónimo.
    - map<word, id>: Aquí cada palabra del voc. es única y con este mapa encontramos el ID a través
    de una palabra.

    El problema es que aquí tenemos otro factor y es el TIPO. Necesitamos saber si esa palabra es un
    verbo, nombre, etc. Por eso el mapa apunta de word a Vocabulary, donde ya podremos saber el ID y
    tambien el tipo consultando la clase.

    Las palabras están como key en m_wordToVocMap y como valor en m_idToWordMap. Estas últimas las
    he puesto como referencia y así puedo asignar las palabras al objecto voc evitando "triplicar"
    datos.
    */
    //m_idToWordMap value es por referencia!. Se usa el valor del objeto Vocabulary para no duplicar
    std::array<std::unordered_multimap<std::string, std::string&>, DIM> m_idToWordMap;
    std::array<std::unordered_map<std::string, std::shared_ptr<Vocabulary>>, DIM>  m_wordToVocMap;
};



template <size_t DIM>
VocabularyList<DIM>::VocabularyList()
{
    m_idToWordMap[zarpa::wordType::verb].reserve(zarpa::reserves::id_verbs);
    m_wordToVocMap[zarpa::wordType::verb].reserve(zarpa::reserves::word_verbs);

    m_idToWordMap[zarpa::wordType::noun].reserve(zarpa::reserves::id_nouns);
    m_wordToVocMap[zarpa::wordType::noun].reserve(zarpa::reserves::word_nouns);
}

template <size_t DIM>
void VocabularyList<DIM>::Add(std::shared_ptr<Vocabulary> voc)
{

    for (auto& word : voc->words) {
        m_idToWordMap[voc->type].insert({voc->id, word});  //Aquí word se inserta por referencia
        m_wordToVocMap[voc->type].insert({word, voc});     //Aquí se inserta por valor
    }
}


template <size_t DIM>
std::shared_ptr<Vocabulary> VocabularyList<DIM>::GetVocById(const std::string& id,  zarpa::wordType type)
{
    auto word = GetWord(id, type);
    return m_wordToVocMap[type][word];
}


// Una puta locura contar las keys únicas de un unordered multimap porque no puedes usar upper_bound()
// Excelente snipper de sashimee en 
// https://leetcode.com/problems/group-anagrams/discuss/19518/c-simple-code-using-unordered_multimap
// Y otro de gsf
// https://stackoverflow.com/questions/19716202/boost-unordered-multimap-loop-over-the-unique-keys
template <size_t DIM>
size_t VocabularyList<DIM>::IdsCount(size_t type)
{
    size_t result {0};

   // Solución de gsf
    for (auto iter=m_idToWordMap[type].begin();
            iter!=m_idToWordMap[type].end();
            ){ // Notice the lack of ++iter!!!
        auto end=m_idToWordMap[type].equal_range(iter->first).second;
        for (;iter!=end;++iter)
            result++;
    }

    //Solución de sashimee
    /**
    // loop over unique keys
    for (auto iter=m_idToWordMap[type].begin(); iter!=m_idToWordMap[type].end(); 
            iter=m_idToWordMap[type].equal_range(iter->first).second){
        auto unique_key = iter->first;
        if (m_idToWordMap[type].count(unique_key) > 1){ // if count is greater than 1, push the values to the result.
            auto it_bounds = m_idToWordMap[type].equal_range(unique_key);
            for (auto it=it_bounds.first; it!=it_bounds.second; it++){
               // result.push_back(it->second);
               result++;
            }
        }
    }
    **/

    return result;
}   


#endif /* D10D01B9_ADDF_44D1_88C0_537706F343F3 */
