#include <iostream>
#include <cassert>
#include "script.hpp"
#include "resource_manager.hpp"

extern ResourceManager rm;

void Script::nop()
{
    std::cout << "[DEV] " << m_currentInst->token << " not implemented.\n";
}

//
// Manejo de variables
//
//#[!]eq <var> <var>|<num>
void Script::eq()
{
    m_condSuccess = (GetIntValue(m_currentInst, 0) == GetIntValue(m_currentInst, 1));
    if (m_currentInst->negation) m_condSuccess = !m_condSuccess;
}

//#[!]le <var> <var>|<num>
void Script::le()
{
    m_condSuccess = (GetIntValue(m_currentInst, 0) <= GetIntValue(m_currentInst, 1));
    if (m_currentInst->negation) m_condSuccess = !m_condSuccess;
}

//#[!]ge <var> <var>|<num>
void Script::ge()
{
    m_condSuccess = (GetIntValue(m_currentInst, 0) >= GetIntValue(m_currentInst, 1));
    if (m_currentInst->negation) m_condSuccess = !m_condSuccess;
}

//#set <var> <var>|<num>
void Script::set()
{
    rm.variablesList.Get(m_currentInst->paramsId[0])->value = GetIntValue(m_currentInst, 1);
}

//#add <var> <var>|<num>
void Script::add()
{
    auto res = GetIntValue(m_currentInst, 0) + GetIntValue(m_currentInst, 1);
    rm.variablesList.Get(m_currentInst->paramsId[0])->value = res;
}

//#sub <var> <var>|<num>
void Script::sub()
{
    auto res = GetIntValue(m_currentInst, 0) - GetIntValue(m_currentInst, 1);
    rm.variablesList.Get(m_currentInst->paramsId[0])->value = res;
}

//
// Objetos
//
//#[!]here <obj>|<word_obj_noun>
void Script::here()
{
    std::string objLoc;
    switch (m_currentInst->paramsType[0]) {
        case zarpa::paramType::word_obj_noun:
            objLoc = rm.objectsList.Get(GetObjIdFromNounId(m_currentInst->paramsId[0]))->location;
            break;

        case zarpa::paramType::obj :
            objLoc = rm.objectsList.Get(m_currentInst->paramsId[0])->location;
            break;

        default: assert(false && "here() Unhandled paramType"); break;
    }

    m_condSuccess = (objLoc == rm.entitiesList.Get("_entPlayer")->location) || (objLoc == "_locCarried") || (objLoc == "_locWorn");
    if (m_currentInst->negation) m_condSuccess = !m_condSuccess;
}

//#[!]empty <obj>|<loc>|<ent>|<word_obj_noun>
void Script::empty()
{
    m_condSuccess = true;
    // El parámetro puede ser una localidad, entidad o un objeto, en cuyos casos se busca
    // la localidad de éstos.

    std::string locCont;
    switch (m_currentInst->paramsType[0]) {
        case zarpa::paramType::word_obj_noun:
            locCont = rm.objectsList.Get(GetObjIdFromNounId(m_currentInst->paramsId[0]))->container;
            break;
        case zarpa::paramType::obj: 
            locCont = rm.objectsList.Get(m_currentInst->paramsId[0])->container;
            break;
        case zarpa::paramType::ent:
            locCont = rm.entitiesList.Get(m_currentInst->paramsId[0])->container;
            break;
        case zarpa::paramType::loc:
            locCont = rm.locationsList.Get(m_currentInst->paramsId[0])->id;
            break;
        default: assert(false && "empty() Unhandled paramType."); break;

    }

    // Si el objeto no es contenedor, está vacío
    if (locCont != "") {
        for (auto& pair : rm.objectsList.GetItems()) {
            auto obj = pair.second;
            if (obj->location == locCont) {
                m_condSuccess = false;
                break;
            }
        }
    }
    if (m_currentInst->negation) m_condSuccess = !m_condSuccess;
}

//#swappos <obj> <obj>
void Script::swappos()
{
    //TODO: Tener en cta. peso, etc. si lo lleva el jugador. ¿Usar Move()?
    std::swap(rm.objectsList.Get(m_currentInst->paramsId[0])->location,
            rm.objectsList.Get(m_currentInst->paramsId[1])->location);
}

//#put <obj>|<word_obj_noun> <obj> (debe ser contenedor)
void Script::put()
{
    auto objDest = rm.objectsList.Get(m_currentInst->paramsId[1]);

    std::shared_ptr<Object> objSource;
    switch (m_currentInst->paramsType[0]) {
        case zarpa::paramType::word_obj_noun :
            objSource = rm.objectsList.Get(GetObjIdFromNounId(m_currentInst->paramsId[0]));
            break;

        case zarpa::paramType::obj :
            objSource = rm.objectsList.Get(m_currentInst->paramsId[0]);
            break;

        default : assert(false && "put() Unhandled paramType."); break;
    }
    // Sólo si el destino es obj. contenedor.
    if (objDest->container == "") {
        ShowMessage("_msgNotContainer", true);
        notdone();
    } 
    // No puede guardarse él mismo.
    else if (objDest == objSource) {
        ShowMessage("_msgCantDoIt", true);
        notdone();
    }
    else {
        Move(objSource, objDest);
    }
}

//#drop <obj>|<word_obj_noun> <loc>
void Script::drop()
{
    auto loc = rm.locationsList.Get(m_currentInst->paramsId[1]);
    std::shared_ptr<Object> obj;
    switch (m_currentInst->paramsType[0]) {
        case zarpa::paramType::word_obj_noun :
            obj = rm.objectsList.Get(GetObjIdFromNounId(m_currentInst->paramsId[0]));
            break;

        case zarpa::paramType::obj :
            obj = rm.objectsList.Get(m_currentInst->paramsId[0]);
            break;

        default : assert(false && "drop() Unhandled paramType."); break;
    }

    // Sólo para objetos que sean llevados
    if (obj->location == "_locCarried") {
        Move(obj, loc);
    } else {
        ShowMessage("_msgNotCarried", true);
        notdone();
    }
}

//#get <obj>|<word_obj_noun> <loc>
void Script::get()
{
    auto loc = rm.locationsList.Get(m_currentInst->paramsId[1]);
    std::shared_ptr<Object> obj;
    switch (m_currentInst->paramsType[0]) {
        case zarpa::paramType::word_obj_noun :
            obj = rm.objectsList.Get(GetObjIdFromNounId(m_currentInst->paramsId[0]));
            break;

        case zarpa::paramType::obj :
            obj = rm.objectsList.Get(m_currentInst->paramsId[0]);
            break;

        default : assert(false && "get() Unhandled paramType."); break;
    }

    // Sólo para objetos no que estén llevados o puestos
    if (obj->location == "_locCarried" && obj->location == "_locWorn") {
        ShowMessage("_msgAlreadyCarried", true);
        notdone();
    }
    // El objeto no está en la localidad indicada 
    else if (obj->location != loc->id) {
        ShowMessage("_msgGetNothing", true);
        notdone();
    }
    // El objeto no es llevable
    else if (!obj->isCarriable) {
        ShowMessage("_msgNotCarriable", true);
        notdone();
    }
    else {
        Move(obj, rm.locationsList.Get("_locCarried"));
    }        
}

///#remove <obj>|<word_obj_noun>
void Script::remove()
{
    std::shared_ptr<Object> obj;
    switch (m_currentInst->paramsType[0]) {
        case zarpa::paramType::word_obj_noun :
            obj = rm.objectsList.Get(GetObjIdFromNounId(m_currentInst->paramsId[0]));
            break;

        case zarpa::paramType::obj :
            obj = rm.objectsList.Get(m_currentInst->paramsId[0]);
            break;

        default : assert(false && "remove() Unhandled paramType."); break;
    }
    // El objeto no se lleva puesto
    if (obj->location != "_locWorn") {
        ShowMessage("_msgNotWorn", true);
        notdone();
    }
    else {
        Move(obj, rm.locationsList.Get("_locCarried"));
    }
}

//#wear <obj>|<word_obj_noun>
void Script::wear()
{
    std::shared_ptr<Object> obj;
    switch (m_currentInst->paramsType[0]) {
        case zarpa::paramType::word_obj_noun :
            obj = rm.objectsList.Get(GetObjIdFromNounId(m_currentInst->paramsId[0]));
            break;

        case zarpa::paramType::obj :
            obj = rm.objectsList.Get(m_currentInst->paramsId[0]);
            break;

        default : assert(false && "wear() Unhandled paramType."); break;
    }
    // El objeto ya está puesto
    if (obj->location == "_locWorn") {
        ShowMessage("_msgAlreadyWorn", true);
        notdone();
    }
    // El objeto no se lleva
    else if (obj->location != "_locCarried") {
        ShowMessage("_msgNotCarried", true);
        notdone();
    } 
    // el objeto no es ponible
    else if (!obj->isWearable) {
        ShowMessage("_msgCantWear", true);
        notdone();
    }
    else {
        Move(obj, rm.locationsList.Get("_locWorn"));
    }
}

void Script::exam()
{
    std::shared_ptr<Object> obj;
    switch (m_currentInst->paramsType[0]) {
        case zarpa::paramType::word_obj_noun :
            obj = rm.objectsList.Get(GetObjIdFromNounId(m_currentInst->paramsId[0]));
            break;
            
        case zarpa::paramType::obj :
            obj = rm.objectsList.Get(m_currentInst->paramsId[0]);
            break;

        default : assert(false && "exam() Unhandled paramType."); break;
    }    

    // Si no hay mensaje definido, muestra un "nada interesante..."
    if (rm.messagesGroupList.IdExists(obj->exam)) {
        ShowMessage(obj->exam, true);
    } 
    else {
        ShowMessage("_msgNothingInteresting", true);
    }
}


//
// Vocabulario
//
//#[!]voc <word_verb>|<word_mov_verb>|<word_obj_noun>|<word_noun>
// No tiene sentido usar un verbo directo:  "voc N" ???? 
void Script::voc()
{
    switch (m_currentInst->paramsType[0]) {
        case zarpa::paramType::word_obj_noun :
            m_condSuccess = (GetObjNoun() != "");
            break;

        case zarpa::paramType::word_noun :
            m_condSuccess = (GetNoun() != "");
            break;            

        case zarpa::paramType::word_mov_verb :
            m_condSuccess = (GetMovVerb() != "");
            break;

        case zarpa::paramType::word_verb :
            m_condSuccess = (GetVerb() != "");
            break;

        default: assert(false && "voc() Unhandled paramType.");
    }

    if (m_currentInst->negation) m_condSuccess = !m_condSuccess;  
}

//#doall <loc>
void Script::doall()
{
    if (rm.phrases.empty()) return;
    // Hay que crear una nueva cola de sentencias. La primera será la actual (para que el parser la
    // elimine al terminar las tablas), las siguientes las expandidas con el nombre del objeto y a 
    // continuación las que había, en el mismo orden
    std::queue<std::shared_ptr<Phrase>> newPhr;
    newPhr.push(rm.phrases.front());
    auto loc = rm.locationsList.Get(m_currentInst->paramsId[0]);
    for (const auto& pair : rm.objectsList.GetItems()) {
        auto id = pair.first; // ó obj->id
        auto obj = pair.second;
        if (obj->location == loc->id) {
            auto ph = std::make_shared<Phrase>();
            ph->verb = rm.phrases.front()->verb;
            ph->noun = obj->noun;
            ph->obj_noun = ph->noun;
            newPhr.push(ph);
        }
    }
    // Se encontró algún objeto?. Añadimos el resto de órdenes (si las hubiera) al nuevo objeto frases 
    if (newPhr.size() > 1) {
        rm.phrases.pop();
        while (!rm.phrases.empty()) {
            newPhr.push(rm.phrases.front());
            rm.phrases.pop();
        }
        rm.phrases = {};
        rm.phrases = newPhr;
    }    
}

//
//
//
//#[!]atloc <loc> <entity>|<obj>|<word_obj_noun>
void Script::atloc()
{
    // Se podría comparar el ID de la loc directamente con el parámetro, pero
    // tenemos que tratar con _locPlayer
    // std::string loc;
    // (...)
    // m_condSuccess =  (m_currentInst->paramsId[0] == loc);

    std::string targetLocId;
    switch (m_currentInst->paramsType[1]) {
        case zarpa::paramType::word_obj_noun :
            // Ahora objId contiene el ID del objeto, no el ID del noun
            targetLocId = rm.objectsList.Get(GetObjIdFromNounId(m_currentInst->paramsId[1]))->location;
            break;

        case zarpa::paramType::obj :
            targetLocId = rm.objectsList.Get(m_currentInst->paramsId[1])->location;
            break;

        case zarpa::paramType::ent :
            targetLocId = rm.entitiesList.Get(m_currentInst->paramsId[1])->location;
            break;
        
        default : assert(false && "atloc() Unhandled paramType."); break;
    }
    auto loc = rm.locationsList.Get(m_currentInst->paramsId[0]);

    m_condSuccess = (loc->id == targetLocId);
    if (m_currentInst->negation) m_condSuccess = !m_condSuccess;
}    

//#[!]atzone <zone> <entity>
void Script::atzone()
{
    auto zone = rm.entitiesList.Get(m_currentInst->paramsId[1])->zone;
    m_condSuccess =  (m_currentInst->paramsId[0] == zone);
    if (m_currentInst->negation) m_condSuccess = !m_condSuccess;
}

void Script::canexit()
{
    auto exit = m_currentInst->paramsId[1];
    auto entLoc = rm.entitiesList.Get(m_currentInst->paramsId[0])->location;
    m_condSuccess = (rm.locationsList.Get(entLoc)->exits[exit] != "");
    if (m_currentInst->negation) m_condSuccess = !m_condSuccess;
}




//#goto <word_mov_verb> <ent>
void Script::goto_()
{
    auto exit = GetMovVerb(); //m_currentInst->paramsId[0];
    auto entLoc = rm.entitiesList.Get(m_currentInst->paramsId[1]);
    auto locToId = rm.locationsList.Get(entLoc->location)->exits[exit];
    if (locToId == "") {
        ShowMessage("_msgCantGo", true);
        m_condSuccess = false;
        notdone();
    } 
    else {
        Move(entLoc, rm.locationsList.Get(locToId));
    }
}

//#moveto <loc> <ent>|<obj>|<word_obj_noun>
void Script::moveto()
{
    auto locTo = rm.locationsList.Get(m_currentInst->paramsId[0]);
    std::string objId, entId;
    switch (m_currentInst->paramsType[1]) {
        // Si el parámetro es un obj_noun, entoces hay que averiguar el objeto que tiene ese noun
        case zarpa::paramType::word_obj_noun :
            objId = GetObjIdFromNounId(m_currentInst->paramsId[1]);
            break;

        case zarpa::paramType::obj :
            objId = m_currentInst->paramsId[1];
            break;

        case zarpa::paramType::ent :
            entId = m_currentInst->paramsId[1];
            break;

        default : assert(false && "moveto() Unhandled paramType."); break;
    }
    if (objId != "") {
        Move(rm.objectsList.Get(objId), locTo);
    } 
    else if (entId != "") {
        Move(rm.entitiesList.Get(entId), locTo);
    }

}


//
// Mensajes y descripciones, done y notdone
//
void Script::done()
{
    m_state = zarpa::scriptState::done;
}

void Script::notdone()
{
    rm.phrases = {};
    m_state = zarpa::scriptState::notdone;
}

void Script::desc()
{
    m_state = zarpa::scriptState::desc;
}

void Script::setdesc()
{
    rm.locationsList.Get(m_currentInst->paramsId[1])->description = 
            rm.descriptionsList.Get(m_currentInst->paramsId[0])->text;
}

void Script::msgln()
{
    ShowMessage(m_currentInst->paramsId[0], true);
}

void Script::msg()
{
    ShowMessage(m_currentInst->paramsId[0], false);
}


//
//
//
void Script::chance()
{
    m_condSuccess = (rand() % 100) < GetIntValue(m_currentInst, 0);
    if (m_currentInst->negation) m_condSuccess = !m_condSuccess;
}