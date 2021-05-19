#include "message.hpp"

std::vector<std::string> Split(const std::string& str, char delim); 

void MessageGroup::Add(const std::string& text, const char sep)
{
    for (auto& t : Split(text, sep)) {
        Message msg;
        msg.text = t;
        // Busca los posibles tags sustitutorios del texto del mensaje. Se guarda un vector con
        // dichos tags para luego sustituirlos facilmente y tambien se guarda otro vector con el
        // pair key:value de dicho tag, para procesarlo de forma fácil.
        auto beginPos = t.find('[');
        auto endPos = t.find(']');
        while (beginPos != std::string::npos && endPos != std::string::npos) {
            // Aquí se guarda el tag íntegro
            msg.replaceTags.push_back(t.substr(beginPos, endPos - beginPos +1));

            // Aquí se guarda como un par key:value
            std::pair<std::string, std::string> pair;
            std::vector<std::string> vpair = Split(msg.replaceTags.front(), ':');
            // Si formato correcto sólo con 2 puntos ":" ....
            if (vpair.size() == 2) {
                // Hay que quitar "[" y "]"
                pair.first = vpair[0].substr(1);
                pair.second = vpair[1].substr(0, vpair[1].size() -1);
            }
            msg.keyValueTags.push_back(pair);
            
            beginPos = t.find('[', beginPos+1);
            endPos = t.find(']', endPos+1);
        }
        m_messages.push_back(std::move(msg)); //TODO: Checkear eficacia de move
    }
}