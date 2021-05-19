#ifndef DC779846_E1BE_4F63_AC02_812F3FDC3127
#define DC779846_E1BE_4F63_AC02_812F3FDC3127

#include <string>
#include <vector>
#include <cstdio>

struct Message
{
    std::string text;
    std::vector<std::string> replaceTags;
    std::vector<std::pair<std::string, std::string>> keyValueTags;
};

class MessageGroup
{
    public:
    void Add(const std::string& text, const char sep = '|');
    Message Get() {return m_messages.at(rand() % m_messages.size());}
    Message Get(size_t idx) {return m_messages.at(idx);}
    size_t Count() {return m_messages.size();}

    private:
    std::vector<Message> m_messages;

};
#endif /* DC779846_E1BE_4F63_AC02_812F3FDC3127 */
