#ifndef F560CE8B_24B3_4EAA_8CB5_78875BF210E2
#define F560CE8B_24B3_4EAA_8CB5_78875BF210E2

#include <type_traits>
#include <array>
#include <unordered_map>
#include <string>

template <typename T>
class ZarpaList
{
    public:
    void Add(T item, const std::string& id) { m_map.insert({id, item});}
    T& Get(const std::string& id) {return m_map[id];}
    std::unordered_map<std::string, T>& GetItems() {return m_map;}
    bool IdExists(const std::string& id) const {return m_map.find(id) != m_map.end();}
    size_t Count() const {return m_map.size();}
    void Reserve(const size_t r) {m_map.reserve(r);}
    unsigned BucketCount() const {return m_map.bucket_count();}

    private:
    std::unordered_map<std::string, T> m_map;
};

#endif /* F560CE8B_24B3_4EAA_8CB5_78875BF210E2 */
