#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <cassert>
#include "script.hpp"
#include "resource_manager.hpp"
/*
MoveEntity(ent, loc)
Mueve la entidad a la localidad, actualiza los contadores y si es la entidad
jugador, mueve atomaticamente entidades follow a la suya.
*/


extern ResourceManager rm;

extern hash_t hash_(char const* str);
extern void Parse(const std::string&);
extern void TEST_Parse(const std::string&);
extern void ReplaceAll(std::string& source, const std::string& from, const std::string& to);
extern std::string GetFullName(const std::string& objId);



std::string Script::GetObjIdFromNounId(const std::string& nounId)
{
    auto id = nounId;
    if (nounId == "_OBJ") {
        id = rm.phrases.front()->obj_noun;
    }
    for (const auto& pair : rm.objectsList.GetItems()) {
        if (pair.second->noun == id) {
            return pair.second->id; // ó pair.first
        }
    }
    assert(false && "GetObjIdFromNounId() NounId not found.");
    return "";
}

std::string Script::GetVerb()
{
    if (rm.phrases.empty()) return ""; // Prevenir crash
    return rm.phrases.front()->verb;
}

std::string Script::GetMovVerb()
{
    if (rm.phrases.empty()) return "";
    return rm.phrases.front()->mov_verb;
}

std::string Script::GetObjNoun()
{
    if (rm.phrases.empty()) return "";
    return rm.phrases.front()->obj_noun;
}

std::string Script::GetNoun()
{
    if (rm.phrases.empty()) return "";
    return rm.phrases.front()->noun;
}

bool Script::EntryIsValid(std::shared_ptr<Entry> entry)
{
    // Si no hay frases, sólo se validarán las entradas "* *"
    bool verbValid, nounValid;
    if (rm.phrases.empty()) { //TODO: || tabla no afterprompt
        verbValid = (entry->verb == "*");
        nounValid = (entry->noun == "*");
    }
    else {
        verbValid = (entry->verb == "*") || (rm.phrases.front()->verb == entry->verb);
        nounValid = (entry->noun == "*") || (rm.phrases.front()->noun == entry->noun);
    }
    return (verbValid && nounValid);
}


int_t Script::GetIntValue(std::shared_ptr<Instruction> inst, unsigned paramN)
{
    if (inst->paramsType[paramN] == zarpa::paramType::num) {
        return std::stol(inst->paramsId[paramN]);
    }
    if (inst->paramsType[paramN] == zarpa::paramType::var) {
        return rm.variablesList.Get(inst->paramsId[paramN])->value;
    }
    assert(false && "GetIntValue() not num or var");
    return 0;
}


void Script::Move(std::shared_ptr<Entity> ent, std::shared_ptr<Location> loc)
{
    ent->prevLocation = ent->location;
    ent->location = loc->id;
    // ¿Es la entidad jugador?. Se apunta la entidad especial _locPlayer
    if (ent->id == "_entPlayer") {
        SetLocPlayer();
    }
    //TODO: CONTADORES
}

void Script::Move(std::shared_ptr<Object> obj, std::shared_ptr<Location> loc)
{
    if (obj->location == "_locWorn") {
        //TODO: Actualizar contadores
    }
    else if (obj->location == "_locCarried") {
        //TODO: Actualizar contadores
    }
    
    obj->location = loc->id;

    if (loc->id == "_locWorn") {
        //TODO: Actualizar contadores
    }
    else if (loc->id == "_locCarried") {
        //TODO: Actualizar contadores
    }
}

void Script::Move(std::shared_ptr<Object> obj, std::shared_ptr<Object> objDest)
{
    //TODO: Actualizar contadores y comprobar capacidades
    Move(obj, rm.locationsList.Get(objDest->location));
}

void Script::SetLocPlayer()
{
    std::string locPlayerId = rm.entitiesList.Get("_entPlayer")->location;
    rm.locationsList.Get("_locPlayer") = rm.locationsList.Get(locPlayerId);
}

void Script::ShowMessage(std::string id, bool newLine)
{
    ShowMessage(rm.messagesGroupList.Get(id), newLine);
}

void Script::ShowMessage(std::shared_ptr<MessageGroup> mg, bool newLine)
{
    auto msg = mg->Get();
    std::vector<std::string> changedTags {};
    
    // Se procesan los posibles tags
    for (const auto& pair : msg.keyValueTags) {
        auto key = pair.first;
        auto value = pair.second;
        
        // Variable. Se comprueba que exista y se reemplaza el valor
        if (key == "var") {
            if (!rm.variablesList.IdExists(value)) {
                rm.error.Show("On message tag. Variable '" + value + "' doesn't exists.",
                        zarpa::errorType::error, m_currentTable->fileName, m_currentInst->line);
                changedTags.push_back("");
            } else {
                changedTags.push_back(std::to_string(rm.variablesList.Get(value)->value));
            }
        } 
        // Salidas
        else if (key == "exits") {
            if (!rm.locationsList.IdExists(value)) {
                rm.error.Show("On message tag. Exits of '" + value + "'. Location doesn't exists.",
                        zarpa::errorType::error, m_currentTable->fileName, m_currentInst->line);
                changedTags.push_back("");    
            } else { // Se recorren las posibles salidas y se crean sus nombres de salida
                // En un vector para gestionar las ","
                std::vector<std::string> exits;
                std::string textExits {""};
                auto loc = rm.locationsList.Get(value);
                for (const auto& pair : loc->exits) {
                    auto moveTo = pair.first;
                    auto locIdTo = pair.second;
                    if (locIdTo != "") {
                        exits.push_back(rm.vocabularyList.GetWord(moveTo, zarpa::wordType::mov_verb));
                    }
                }
                for (size_t i = 0; i < exits.size(); i++) {
                    textExits += exits[i];
                    if (i != exits.size() -1) {
                        textExits += ", ";
                    }
                }                
                changedTags.push_back(textExits);
            }                
        }
        // Localidad. Muestra todos los objetos de esa localidad.
        else if (key == "loc") {
            if (!rm.locationsList.IdExists(value)) {
                rm.error.Show("On message tag. Location '" + value + "' doesn't exists.",
                        zarpa::errorType::error, m_currentTable->fileName, m_currentInst->line);
                changedTags.push_back("");                    
            } else {   // Se recorren los objetos de esa localidad y se crean sus nombres largos
                // En un vector para gestionar las ","
                std::vector<std::shared_ptr<Object>> objs;
                auto loc = rm.locationsList.Get(value);
                std::string textObjs {""};
                for (const auto& pair : rm.objectsList.GetItems()) {
                    if (pair.second->location == loc->id) {
                        objs.push_back(pair.second);
                    }
                }

                for (size_t i = 0; i < objs.size(); i++) {
                    textObjs += GetFullName(objs[i]->id);
                    if (i != objs.size() -1) {
                        textObjs += ", ";
                    }
                }
                changedTags.push_back(textObjs);
            }
        }
        // Objeto actual
        else if (key == "obj") {
            std::string objId;
            if (value == "_OBJ") {
                objId = GetObjIdFromNounId(rm.phrases.front()->obj_noun);
            }
            if (objId == "" || !rm.objectsList.IdExists(objId)) {
                rm.error.Show("On message tag. Object '" + value + "' doesn't exists.",
                        zarpa::errorType::error, m_currentTable->fileName, m_currentInst->line);
                changedTags.push_back("");                        
            } else {
                changedTags.push_back(GetFullName(objId));
            }            
        }
        else {
            rm.error.Show("On message tag. Tag '" + key + "' unknown.",
                        zarpa::errorType::error, m_currentTable->fileName, m_currentInst->line);
                changedTags.push_back(key + ":" + value);
        } 
    }

    std::string text = msg.text;
    if (!changedTags.empty()) {
        for (size_t i = 0; i < changedTags.size(); i++) {
            if (changedTags[i] != "") {
                ReplaceAll(text, msg.replaceTags[i], changedTags[i]);
            }
        }
    }
    if (rm.appInfo.devMode) std::cout << text << std::endl;
    rm.server.Send("[msg]" + text + (newLine ? "\n" : ""));
}


void Script::ShowDescription()
{
    auto locId = rm.entitiesList.Get("_entPlayer")->location;
    auto desId = rm.locationsList.Get(locId)->description;
    if (!rm.descriptionsList.IdExists(desId)) {
        rm.error.Show("Location '" + locId + "' has not description ID.", zarpa::errorType::error);
        rm.server.Send("[des]");      
    }
    else if (desId == "") {
        rm.error.Show("Location '" + locId + "': Description '" + desId + "' not defined.", zarpa::errorType::error);
        rm.server.Send("[des]");
    } 
    else {
        if (rm.appInfo.devMode) std::cout << rm.descriptionsList.Get(desId)->text << "\n";
        rm.server.Send("[des]" + rm.descriptionsList.Get(desId)->text);
    }
}
