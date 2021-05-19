#ifndef D8235850_28C4_4758_B453_2BE6322B8B2D
#define D8235850_28C4_4758_B453_2BE6322B8B2D
#include <string>

struct AppInfo
{
    bool verbose;
    bool checkOnly;
    bool enableDebug;
    bool showReport;
    bool devMode;
};

struct AdventureInfo 
{
    std::string lang;
    std::string title;
    std::string version;
    std::string author;
    std::string brief;    
    std::string folder;
};

#endif /* D8235850_28C4_4758_B453_2BE6322B8B2D */
