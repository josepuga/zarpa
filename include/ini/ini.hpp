/*
MIT License

Copyright (c) 2018-2021 José Puga

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef INI_HPP
#define INI_HPP
#define INI_HPP_VERSION "1.0.2"

#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <map>
#include <fstream>
#include <regex>
#include <iostream>



//GetValue<type>(...)
    /**
     * @brief Return the value's section as typel
     * @param section "" for no section.
     * @param key
     * @param default (optional) default value if is not set
     * for bool values return true only if value = (1, true)
     * @return value.
     * */

//GetSplitValues<type>(...)
    /**
     * @brief Return a vector of int splitting the value with a separator
     * @param section "" for no section.
     * @param key
     * @param separator (optional) separator to split the values. Defaul is comma ','.
     * @param default (optional) default value if an element has an error converting
     * @return vector with values or empty if the key is empty or doesnt exists.
     * */

// GetValueHex() and GetSplitValuesHex() :
// same as above, but only for types convertible to int. The value must be a hex
// number 


namespace ini
{
    typedef std::pair<std::string, std::string> keyvalue_t;
    typedef std::multimap<std::string, keyvalue_t> sections_t;
}


class Ini
{
public:

    Ini() {}

// Type bool
    template <typename T>
    std::enable_if_t< std::is_same< T, bool >::value, 
    T > GetValue(const std::string& section, const std::string& key, const T def = false) const
    {
        auto val = _GetValue(section, key);
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        bool result = false;
        if (val == "true" || val == "1") {
            result = true;
        } 
        else if (val == "false" || val == "0") {
            result = false;
        } 
        else {
            result = def;
        }
        return result;
    }

    template <typename T>
    std::enable_if_t< std::is_same< T, bool >::value, 
    std::vector<T> > GetSplitValues(const std::string& section, const std::string& key, 
                const char sep = ',',  const T def = false) const
    {
        std::vector<bool> result;
        auto val = _GetValue(section, key);
        if (val == "") {
            return result;
        }
        std::stringstream ss(val);
        std::string buff;
        while (getline(ss, buff, sep)) {
            if (buff == "true" || buff == "1") {
                result.push_back(true);
            } 
            else if (buff == "false" || buff == "0") {
                result.push_back(false);
            } 
            else {
                result.push_back(def);
            }
        }
        return result;
    }

// Type char
    template <typename T>
    std::enable_if_t< std::is_same< T, char >::value, 
    T > GetValue(const std::string& section, const std::string& key, const T def = '\0') const
    {
        auto result = _GetValue(section, key);
        if (result == "") {
            result = def;
        }
        return result.at(0);   
    }

    template <typename T>
    std::enable_if_t< std::is_same< T, char >::value, 
    std::vector<T> > GetSplitValues(const std::string& section, const std::string& key, 
                const char sep = ',',  const T def = '\0') const
    {
        std::vector<T> result;
        auto val = _GetValue(section, key);
        if (val == "") {
            return result;
        }
        std::stringstream ss(val);
        std::string buff;
        while (getline(ss, buff, sep)) {
            if (buff == "") {
                result.push_back(def);
            } else {
                result.push_back(buff.at(0));
            }
        }
        return result;
    }




// Type string
    template <typename T>
    std::enable_if_t< std::is_same< T, std::string >::value, 
    T > GetValue(const std::string& section, const std::string& key, const T def = "") const
    {
        auto result = _GetValue(section, key);
        if (result == "") {
            result = def;
        }
        return result;   
    }

    template <typename T>
    std::enable_if_t< std::is_same< T, std::string >::value, 
    std::vector<T> > GetSplitValues(const std::string& section, const std::string& key, 
                const char sep = ',',  const T def = "") const
    {
        std::vector<T> result;
        auto val = _GetValue(section, key);
        if (val == "") {
            return result;
        }
        std::stringstream ss(val);
        std::string buff;
        while (getline(ss, buff, sep)) {
            if (buff == "") {
                result.push_back(def);
            } else {
                result.push_back(buff);
            }
        }
        return result;
    }


// Type float
    template <typename T>
    std::enable_if_t< std::is_same< T, float >::value, 
    T > GetValue(const std::string& section, const std::string& key, const T def = 0.0f) const
    {
        T result;
        try {
            result = std::stof(_GetValue(section, key));
        } catch (std::invalid_argument& e) {
            result = def;
        }
        return result;        
    }

    template <typename T>
    std::enable_if_t< std::is_same< T, float >::value, 
    std::vector<T> > GetSplitValues(const std::string& section, const std::string& key, 
                const char sep = ',',  const T def = 0.0f) const
    {
        std::vector<T> result;
        auto val = _GetValue(section, key);
        if (val == "") {
            return result;
        }

        std::stringstream ss(val);
        std::string buff;
        while (getline(ss, buff, sep)) {
            T newVal;
            try {
                newVal = stof(buff);
            } catch (std::invalid_argument& e) {
                newVal = def;
            }
            result.push_back(newVal);
        }
        return result;
    }     


// Convertible to int
    template <typename T>
    std::enable_if_t< std::is_convertible< T, int >::value &&
                     !std::is_same< T, bool >::value &&
                     !std::is_same< T, char >::value &&
                     !std::is_same< T, float >::value,
    T > GetValue(const std::string& section, const std::string& key, const T def = 0, int base = 10) const
    {
        T result;
        try {
            result = std::stoll(_GetValue(section, key), 0, base);
        } catch (std::invalid_argument& e) {
            result = def;
        }
        return result;        
    }

    template <typename T>
    std::enable_if_t< std::is_convertible< T, int >::value &&
                     !std::is_same< T, bool >::value &&
                     !std::is_same< T, char >::value &&
                     !std::is_same< T, float >::value,
     T > GetValueHex(const std::string& section, const std::string& key, const T def = 0) const
    {
        return GetValue(section, key, def, 16);
    }    

    template <typename T>
    std::enable_if_t< std::is_convertible< T, int >::value &&
                     !std::is_same< T, bool >::value &&
                     !std::is_same< T, char >::value &&
                     !std::is_same< T, float >::value,
    std::vector<T> > GetSplitValues(const std::string& section, const std::string& key, 
                const char sep = ',',  const T def = 0) const
    {
        std::vector<T> result;
        auto val = _GetValue(section, key);
        if (val == "") {
            return result;
        }

        std::stringstream ss(val);
        std::string buff;
        while (getline(ss, buff, sep)) {
            int newVal;
            try {
                newVal = stoll(buff);
            } catch (std::invalid_argument& e) {
                newVal = def;
            }
            result.push_back(newVal);
        }
        return result;
    } 

    template <typename T>
    std::enable_if_t< std::is_convertible< T, int >::value &&
                     !std::is_same< T, bool >::value &&
                     !std::is_same< T, char >::value &&
                     !std::is_same< T, float >::value,
    std::vector<T> > GetSplitValuesHex(const std::string& section, const std::string& key, 
                const char sep = ',',  const T def = 0) const
    {
        //TODO: avoid duplicate code with GetSplitValue<int>
        std::vector<T> result;
        auto val = _GetValue(section, key);
        if (val == "") {
            return result;
        }

        std::stringstream ss(val);
        std::string buff;
        while (getline(ss, buff, sep)) {
            int newVal;
            try {
                newVal = stoll(buff, 0, 16);
            } catch (std::invalid_argument& e) {
                newVal = def;
            }
            result.push_back(newVal);
        }
        return result;
    } 

    //Default type
    std::string GetValue(const std::string& section, const std::string& key, const std::string def = "") const
    {
        return GetValue<std::string>(section, key, def);
    }

    std::vector<std::string> GetSplitValues(const std::string& section, const std::string& key, 
                const char sep = ',',  const std::string def = "") const
    {
        return GetSplitValues<std::string>(section, key, sep, def);
    }



    /**
     * @brief Load all data from the file.
     * @param fileName.
     * @return true if success.
     * */
    bool LoadFromFile(const std::string& fileName)
    {
        m_fileName = fileName;
        std::ifstream file;
        file.open(m_fileName);
        if (file.bad() || file.fail()) {
            return false;
        }
        m_sections.clear();
        m_linesBuffer.clear();
        for (std::string ln; getline(file, ln);) {
            Trim(ln);
            m_linesBuffer.push_back(ln);
        }
        file.close();

        ParseLines();
        return true;
    }

    /**
     * @brief Returns the file name of the last LoadFromFile() call.
     * @return file name with full path.
     * */
    inline const std::string GetFileName() const {return m_fileName;}

    /**
     * @brief Returns the file path of the last LoadFromFile() call.
     * @return directory of the current ini file.
     * */
    const std::string GetFilePath() const 
    {
        auto found = m_fileName.find_last_of("/\\");
        return m_fileName.substr(0, found); // For file name only: substr(found+1);
    }

    /**
     * @brief Check if the key exists and has a value.
     * @param section
     * @param key
     * @return true if success.
     * */
    bool KeyExists(const std::string& section, const std::string& key) const
    {
        if (!SectionExists(section)) {
            return false;
        }
        auto range = m_sections.equal_range(section);
        for (auto it = range.first; it != range.second; it++) {
            if (std::get<0>(it->second) == key && std::get<1>(it->second) != "") {
                return true;
            }
        }
        return false;        
    }

    /**
     * @brief Check if the section exists
     * @param section
     * @return true if success.
     * */
    bool SectionExists(const std::string& section) const
    {
        auto it = m_sections.find(section);
        return (it != m_sections.end());
    }    

    /**
     * @brief Gets all the sections names.
     * NOTE: if one or more keys are outside section a new section named "" will be created.
     * @return vector of strings with all the sections names.
     * */
    std::vector<std::string> GetSections()
    {
        //https://stackoverflow.com/questions/11554932/how-can-i-get-all-the-unique-keys-in-a-multimap
        std::vector<std::string> result;
        for (auto it = m_sections.begin(); it != m_sections.end(); 
                it = m_sections.upper_bound(it->first)) {
            result.push_back(it->first);
        }
        return result;
    }    

    /**
     * @brief Gets all the keys of a section.
     * @return vector of strings with all keys.
     * */
    std::vector<std::string> GetSectionKeys(const std::string& section)
    {
        std::vector<std::string> result {};
        if (!SectionExists(section)) {
            return result;
        }
        auto range = m_sections.equal_range(section);
        for (auto it = range.first; it != range.second; it++) {
            result.push_back(std::get<0>(it->second));
        }
        return result;
    }    

private:
    std::string m_fileName;
    std::vector<std::string> m_linesBuffer;
    //std::vector<std::string> m_comments;
    ini::sections_t m_sections;

    std::string _GetValue(const std::string& section, const std::string& key) const
    {
        if (!SectionExists(section)) {
            return "";
        }
        auto range = m_sections.equal_range(section);
        for (auto it = range.first; it != range.second; it++) {
            if (std::get<0>(it->second) == key) {
                return std::get<1>(it->second);
            }
        }
        return "";
    }

    void ParseLines()
    {
        //TODO: replace "continue" by "else if"
        std::string sec, key, val, com;
        std::string sectionActive;

        for ( auto lin : m_linesBuffer) {
            if (lin.empty()) {
                continue;
            }
            if (LineIsComment(lin)) {
                continue;
            }
            if (IfLineIsSectionSet(lin, sec)) {
                sectionActive = sec;
                continue;
            }
            if (IfLineIsKeySet(lin, key, val)) {
                m_sections.insert(std::pair<std::string, ini::keyvalue_t>(sectionActive, std::make_pair(key, val)));
                continue;
            }
        }
    }

    void Trim(std::string& s)
    {
        static std::regex e("^\\s+|\\s+$");
        //LTrim only std::regex_replace(s, std::regex("^\\s+"), "");
        //RTrim only std::regex_replace(s, std::regex("\\s+$"), "");
        s = std::regex_replace(s, e, "");
    }    

    bool IfLineIsSectionSet(const std::string& lin, std::string& sec)
    {
        bool result = (*lin.begin() == '[' && *(lin.end()-1) == ']');
        if (result) {
            sec = lin.substr(1, lin.length()-2);
            Trim(sec);
        }
        return result;
    }

    bool IfLineIsKeySet(const std::string& lin, std::string& key, std::string& val)
    {
        auto pos = lin.find("=");
        bool result = (pos != std::string::npos);
        if (! result) {
            return result;
        }
        key = lin.substr(0, pos);
        val = lin.substr(pos+1, lin.length());
        Trim(key);
        Trim(val);
        return result;
    }

    bool LineIsComment(const std::string& lin) //Not jet implemented.
    {
        return (lin.at(0) == '#' || lin.at(0) == ';');
    }

};
#endif // INI_HPP
