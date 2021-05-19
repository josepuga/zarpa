#include <iostream>
#include <iomanip>
#include "zarpa_types.hpp"
#include "zarpa.hpp"
#include "resource_manager.hpp"

extern ResourceManager rm;

std::string DEV_GetParamType(zarpa::paramType type);

using namespace std;

// Herramienta desarrollador, para testear las tablas.
void DEV_ShowTables()
{
    for (auto& table : rm.script->tables)
    {
        if (table == nullptr) continue; // Not created because empty
        cout << "\n[DEV] " << table->fileName << "\n";
        for (auto& entry : table->entries) {
            cout << "[DEV] " << setw(4) << entry->line << ": " << entry->verb << " " << entry->noun << "\n";
            for (auto& inst : entry->instructions) {
                string token = (inst->negation ? "!" : "");
                token += inst->token;
                string param0 = "(" + DEV_GetParamType(inst->paramsType[0]) + ")" + inst->paramsId[0];
                string param1 = "(" + DEV_GetParamType(inst->paramsType[1]) + ")" + inst->paramsId[1];
                cout << "[DEV] " << setw(4) << inst->line << "    " <<
                    setw(11) << left << token << 
                    setw(23) << left << param0 <<
                    setw(23) << left << param1 << "\n";
            }            
        }
    }
}

std::string DEV_GetParamType(zarpa::paramType type)
{
    std::string result;
    using namespace zarpa;
    switch (type) {
        case paramType::ent : result = "ent"; break;
        case paramType::obj : result = "obj"; break;
        case paramType::loc : result = "loc"; break;
        case paramType::var : result = "var"; break;
        case paramType::num : result = "num"; break;
        case paramType::msg : result = "msg"; break;
        case paramType::desc : result = "des"; break;
        case paramType::word_verb : result = "ver"; break;
        case paramType::word_noun : result = "nou"; break;
        case paramType::word_mov_verb : result = "Vmo"; break;
        case paramType::word_obj_noun : result = "Vob"; break;
        case paramType::no_param : result = "---"; break;
        default : result = "???";
    }
    return result;
}