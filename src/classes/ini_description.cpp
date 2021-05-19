#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include "ini_description.hpp"

bool IniDescription::LoadFromFile(const std::string& fileName)
{
    m_fileName = fileName;
    std::ifstream file;
    file.open(m_fileName);
    if (file.bad() || file.fail()) {
        return false;
    }
    m_pairValues.clear();
    m_linesBuffer.clear();
    for (std::string ln; getline(file, ln);) {
        m_linesBuffer.push_back(ln);
    }
    file.close();

    ParseLines();
    return true;
}

void IniDescription::ParseLines()
{
    std::string sec;
    std::string sectionActive {""};
    std::pair<std::string, std::string> pairValue;

    for (auto lin: m_linesBuffer) {
        if (IfLineIsSectionSet(lin,sec)) {
            if (sectionActive != "") { // Not the first time.
                m_pairValues.push_back(std::move(pairValue));
            }
            pairValue.first = sec;
            pairValue.second = "";
            sectionActive = sec;
            continue;
        }
        pairValue.second += lin + "\n";
    }
    if (sec != "") {
        m_pairValues.push_back(std::move(pairValue));
    }
}

bool IniDescription::IfLineIsSectionSet(const std::string& lin, std::string& sec)
{
    bool result = (*lin.begin() == '[' && *(lin.end()-1) == ']');
    if (result) {
        sec = lin.substr(1, lin.length()-2);
    }
    return result;
}
