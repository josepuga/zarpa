#ifndef D4719DD4_7587_40ED_BBF1_6D8A3FD6FAF1
#define D4719DD4_7587_40ED_BBF1_6D8A3FD6FAF1

#include <vector>
#include <string>
#include <utility>

// Una sencilla clase inspirada en Ini, para leer los ficheros de descripciones, ya que estos son
// como ficheros ini pero sin key=value.
class IniDescription
{
    public:
    bool LoadFromFile(const std::string& fileName);
    inline std::vector<std::pair<std::string, std::string>> GetPairValues() {return m_pairValues;}

    private:
    std::string m_fileName;
    std::vector<std::string> m_linesBuffer;
    std::vector<std::pair<std::string, std::string>> m_pairValues;

    void ParseLines();
    bool IfLineIsSectionSet(const std::string& lin, std::string& sec);

};


#endif /* D4719DD4_7587_40ED_BBF1_6D8A3FD6FAF1 */