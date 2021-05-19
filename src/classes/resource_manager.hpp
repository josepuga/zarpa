#ifndef DAC46F0C_FDA1_47DE_A5AE_75EB78723218
#define DAC46F0C_FDA1_47DE_A5AE_75EB78723218
#include <vector>
#include <memory>
#include <queue>
#include <map>
#include <string>
#include <unordered_map>
#include <ini/ini.hpp>
#include "zarpa.hpp"

struct ResourceManager {
    ZarpaList<std::shared_ptr<Location>> locationsList;
    ZarpaList<std::shared_ptr<Description>> descriptionsList;
    ZarpaList<std::shared_ptr<MessageGroup>> messagesGroupList;
    ZarpaList<std::shared_ptr<Object>> objectsList;
    ZarpaList<std::shared_ptr<Entity>> entitiesList;
    ZarpaList<std::shared_ptr<Variable>> variablesList;
    ZarpaList<std::shared_ptr<Sound>> soundsList;
    ZarpaList<std::shared_ptr<Zone>> zonesList;
    ZarpaList<std::shared_ptr<InstructionInfo>> instructionsInfoList;

    std::queue<std::shared_ptr<Phrase>> phrases;
    std::queue<std::shared_ptr<Phrase>> phrasesEntities;

    Error error;
    AppInfo appInfo;
    AdventureInfo adventureInfo;
    std::unordered_map<std::string, std::string> filePaths;
    FakeServer server;

    Ini ini;

    VocabularyList<zarpa::wordType::wordType_count>  vocabularyList;

    std::shared_ptr<Script> script;


    ResourceManager();
    bool Init(const std::string& fileConfig = "");
    bool LoadTexts();
    bool LoadData();
    bool LoadCode();
    std::vector<std::string> GetTableLines(const std::string& fileName);
};


#endif /* DAC46F0C_FDA1_47DE_A5AE_75EB78723218 */
