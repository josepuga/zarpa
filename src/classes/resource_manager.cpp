#include "resource_manager.hpp"
#include "ini_description.hpp"
#include <filesystem>
#include <iostream>

void Trim(std::string& s);
std::vector<std::string> Split(const std::string& str, const char delim);
void DEV_ShowTables();
bool IsNumber(const std::string& s);

namespace fs = std::filesystem;

ResourceManager::ResourceManager()
{
    descriptionsList.Reserve(zarpa::reserves::descs);
    entitiesList.Reserve(zarpa::reserves::ents);
    locationsList.Reserve(zarpa::reserves::locs);
    messagesGroupList.Reserve(zarpa::reserves::msgs);
    objectsList.Reserve(zarpa::reserves::objs);
    variablesList.Reserve(zarpa::reserves::vars);
}

bool ResourceManager::Init(const std::string& fileConfig)
{
    // Se asume que la aplicación está en el mismo directorio que zarpa.ini
    // Si no se indica un fileConfig se selecciona éste.
    if (fileConfig == "") {
        filePaths["zarpa ini"] =  fs::current_path().string() + "/" + zarpa::INI_FILE;      
    }
    else {
        filePaths["zarpa ini"] = fileConfig;
    }


    if (!ini.LoadFromFile(filePaths["zarpa ini"])) {
        error.Show("Zarpa config file not found.", zarpa::errorType::fatal_error, filePaths["zarpa ini"]);
        return false;
    }

    // Información de la aplicación
    appInfo.verbose = ini.GetValue<bool>("zarpa settings", "verbose");
    appInfo.checkOnly = ini.GetValue<bool>("zarpa settings", "check only");
    appInfo.enableDebug = ini.GetValue<bool>("zarpa settings", "enable debug");
    appInfo.showReport = ini.GetValue<bool>("zarpa settings", "show report");
    appInfo.devMode = ini.GetValue<bool>("zarpa settings", "dev mode");


    // Información de la aventura
    adventureInfo.lang = ini.GetValue("adventure settings", "lang", "es");
    adventureInfo.folder = ini.GetValue("adventure settings", "folder");

    if (!ini.LoadFromFile(adventureInfo.folder + "config.ini")) {
        error.Show("Adventure config file not found.", zarpa::errorType::fatal_error, adventureInfo.folder + "config.ini");
        return false;
    }
    adventureInfo.title = ini.GetValue("info", "title");
    adventureInfo.version = ini.GetValue("info", "version");
    adventureInfo.author = ini.GetValue("info", "author");
    adventureInfo.brief = ini.GetValue("info", "brief");


    // Ficheros de usuario
    if (appInfo.verbose) {
        std::cout << "Getting user file paths..." << std::endl;
    }
    filePaths["before_desc"] =  adventureInfo.folder + ini.GetValue("files", "before_desc");
    filePaths["after_desc"] =  adventureInfo.folder + ini.GetValue("files", "after_desc");
    filePaths["before_prompt"] =  adventureInfo.folder + ini.GetValue("files", "before_prompt");
    filePaths["after_prompt"] =  adventureInfo.folder + ini.GetValue("files", "after_prompt");

    filePaths["variables"] =  adventureInfo.folder + ini.GetValue("files", "variables");
    filePaths["locations"] =  adventureInfo.folder + ini.GetValue("files", "locations");
    filePaths["objects"] =  adventureInfo.folder + ini.GetValue("files", "objects");
    filePaths["entities"] =  adventureInfo.folder + ini.GetValue("files", "entities");
    filePaths["sounds"] =  adventureInfo.folder + ini.GetValue("files", "sounds");
    
    filePaths["vocabulary"] =  adventureInfo.folder + ini.GetValue("files", "vocabulary");
    filePaths["messages"] =  adventureInfo.folder + ini.GetValue("files", "messages");
    filePaths["descriptions"] =  adventureInfo.folder + ini.GetValue("files", "descriptions");
  

    // Ficheros de sistema
    if (appInfo.verbose) {
        std::cout << "Getting system file paths..." << std::endl;
    }
    filePaths["system after_prompt"] =  adventureInfo.folder + ini.GetValue("system files", "after_prompt");

    filePaths["system variables"] =  adventureInfo.folder + ini.GetValue("system files", "variables");
    filePaths["system locations"] =  adventureInfo.folder + ini.GetValue("system files", "locations");
    filePaths["system entities"] =  adventureInfo.folder + ini.GetValue("system files", "entities");

    filePaths["system vocabulary"] =  adventureInfo.folder + ini.GetValue("system files", "vocabulary");
    filePaths["system messages"] =  adventureInfo.folder + ini.GetValue("system files", "messages");    
    
    filePaths["system instructions"] =  adventureInfo.folder + ini.GetValue("system files", "instructions");


    // Ficheros log
    filePaths["log data"] =  adventureInfo.folder + ini.GetValue("log files", "data");
    filePaths["log text"] =  adventureInfo.folder + ini.GetValue("log files", "text");
 

    // Se recorren todos los nombres de ficheros y se sustituye (si lo hubiera) $lan$ por el idioma
    for (auto& pair : filePaths) {
        pair.second = std::regex_replace(pair.second, std::regex("\\$lang\\$"), adventureInfo.lang);
    }

    return (LoadTexts() && LoadData() && LoadCode()); 
}

bool ResourceManager::LoadCode()
{
    if (appInfo.verbose) {
        std::cout << "\nLoading Zarpascript Code....\n";
    }
    error.ResetCounts();

    //
    // JUEGO DE INSTRUCCIONES
    //
    // Se usa durante el checkeo del código script para comprobar que cada instrucción, cantidad
    // de parámetros y tipo de los mismos sea correcto.
    if (!ini.LoadFromFile(filePaths["system instructions"])) {
        error.Show("Unable to load System Instructions file.", zarpa::errorType::fatal_error, filePaths["system instructions"]);
        return false;
    }

    std::shared_ptr<InstructionInfo> instInfo;
    for (const auto& id : ini.GetSections()) {
        instInfo = std::make_shared<InstructionInfo>();
        instInfo->conditional = ini.GetValue<bool>(id, "conditional");
        // Parámetros
        for (size_t paramN = 0; paramN < zarpa::max_params; paramN++) {
            zarpa::paramType paramTypeTmp {zarpa::paramType::no_param};
            for (const auto& tag : ini.GetSplitValues(id, "param" + std::to_string(paramN) + "Type")) {
                if (tag == "num") {
                    paramTypeTmp = zarpa::paramType::num;
                } else if (tag == "var") {
                    paramTypeTmp = zarpa::paramType::var;
                } else if (tag == "obj") {
                    paramTypeTmp = zarpa::paramType::obj;
                } else if (tag == "loc") {
                    paramTypeTmp = zarpa::paramType::loc;
                } else if (tag == "var") {
                    paramTypeTmp = zarpa::paramType::var;
                } else if (tag == "desc") {
                    paramTypeTmp = zarpa::paramType::desc;
                } else if (tag == "msg") {
                    paramTypeTmp = zarpa::paramType::msg;
                } else if (tag == "zone") {
                    paramTypeTmp = zarpa::paramType::zone;
                } else if (tag == "word_noun") {
                    paramTypeTmp = zarpa::paramType::word_noun;
                } else if (tag == "word_obj_noun") {
                    paramTypeTmp = zarpa::paramType::word_obj_noun;
                } else if (tag == "word_adj") {
                    paramTypeTmp = zarpa::paramType::word_adj;
                } else if (tag == "word_prep") {
                    paramTypeTmp = zarpa::paramType::word_prep;
                } else if (tag == "word_verb") {
                    paramTypeTmp = zarpa::paramType::word_verb;
                } else if (tag == "word_mov_verb") {
                    paramTypeTmp = zarpa::paramType::word_mov_verb;
                } else if (tag == "word_adv") {
                    paramTypeTmp = zarpa::paramType::word_adv;
                } else if (tag == "sound") {
                    paramTypeTmp = zarpa::paramType::sound;
                } else if (tag == "ent") {
                    paramTypeTmp = zarpa::paramType::ent;
                } else if (tag != "no_param") {
                    if (appInfo.devMode) {
                        std::cout << "[DEV] Instruction '" << id << "': Unknown paramType '" + tag +"'.\n";
                    }
                }
                instInfo->AddParamType(paramTypeTmp, paramN);
            }
        }
        instructionsInfoList.Add(instInfo, id);
    }

    if (appInfo.devMode) {
        std::cout << "[DEV] " << instructionsInfoList.Count() << " Info Instructions.\n";
    }


    //
    // GENERAR TABLAS, ENTRADAS E INTRUCCIONES
    //

    // Tenemos la información de cada instrucción, se carga el código, empieza en rock'n'roll....

    std::shared_ptr<Instruction> inst;
    std::shared_ptr<Entry> entry;
    std::shared_ptr<Table> table;

    std::map<zarpa::tableType, std::string> mapTables {
            {zarpa::tableType::userBeforeDesc, filePaths["before_desc"]},
            {zarpa::tableType::userAfterDesc, filePaths["after_desc"]},    
            {zarpa::tableType::userBeforePrompt, filePaths["before_prompt"]},
            {zarpa::tableType::userAfterPrompt, filePaths["after_prompt"]},    
            {zarpa::tableType::systemAfterPrompt, filePaths["system after_prompt"]}    
    };

    // No termino de procesar la tabla aunque haya un error. Si al final de procesar una tabla, se
    // encontraron errores, se termina sin procesar las demás. Esto ayuda al usuario a detectar de
    // una vez todos los problemas de la tabla.

    script = std::make_shared<Script>();
    for (const auto& pair : mapTables) {
        auto tableType = pair.first;
        auto fileName = pair.second;
        script->tables[tableType] = std::make_shared<Table>();
        auto table = script->tables[tableType]; // Alias for easy type
        table->fileName = fileName;
        if (appInfo.verbose) {
            std::cout << "Procesing " << fileName << "\n";
        }
        // Extraer la lineas en un formato "limpio"
        std::vector<std::string> lines = GetTableLines(fileName);
        // Que no haya código en una tabla, no debe ser error.
        if (lines.empty()) {
            if (appInfo.verbose) {
                std::cout << fileName << ": is empty.\n";
            }
            continue;
        }
        size_t linesCount {0};
        for (auto line : lines) {
            linesCount++;
            if (line == "") {
                continue;
            }
            if (line.at(0) == '\r' || line.at(0) == '\n' || line.at(0) == '#') {
                continue;
            }

            auto tokens = Split(line, ' ');

            // Si es entrada
            if (tokens.at(0) == ":") {
                if (tokens.size() != 3) { // : + verbId + nounId.                MAGIC NUMBER!!!!
                    error.Show(line + " . Bad params number in entry.", zarpa::errorType::error, fileName, linesCount);
                    continue;
                }
                auto verb = tokens.at(1);
                auto noun = tokens.at(2);
                // Si el número de parámetros está bien, comprobar que correspondan con "*" o ID válido
                if (!(verb == "*" || 
                        vocabularyList.IdExists(verb, zarpa::wordType::verb) || 
                        vocabularyList.IdExists(verb, zarpa::wordType::mov_verb))) {
                    error.Show(line + " . Entry first value, word ID '" + verb + "' not found.", zarpa::errorType::error,
                            fileName, linesCount);
                    continue;
                }
                if (!(noun == "*" || 
                        vocabularyList.IdExists(noun, zarpa::wordType::noun) || 
                        vocabularyList.IdExists(noun, zarpa::wordType::mov_verb))) {
                    error.Show(line + " . Entry second value, word ID '" + noun + "' not found.", zarpa::errorType::error,
                            fileName, linesCount);
                    continue;
                }

                // Llegados aquí es correcta, se añade la entrada a la tabla
                entry = std::make_shared<Entry>();
                entry->line = linesCount;
                entry->verb = verb;
                entry->noun = noun;
                table->entries.push_back(entry);

                continue;

            // Si es instrucción
            } else {
                // Si no hay una entrada, no se comprueban esas instrucciones.
                if (table->entries.empty()) {
                    error.Show(line + " . Instruction must be INSIDE entry.", zarpa::errorType::error, fileName, linesCount);
                    continue;
                }

                // Mas de x parámetros? (contamos aquí la instrucción como un parámetro)
                if (tokens.size() > zarpa::max_params + 1) { // +1 por la instruccion
                    error.Show(line + " . Too much parameters.", zarpa::errorType::error, fileName, linesCount);
                    continue;
                }

                // Hacemos tokens del mismo tamaño que zarpa::max_params+1 (+1 es la instruccion que 
                // contiene). Esto nos facilita el código luego más abajo para comprobar tipos de parám.
                // Y no tener que estar chequeando salidas de rango
                while (tokens.size() < zarpa::max_params + 1) {
                    tokens.push_back("");
                }

                inst = std::make_shared<Instruction>();
                inst->line = linesCount;
                inst->token = tokens.at(0);
                // Si es una negación, se le quita el "!" y se guarda si lo es.
                inst->negation = (inst->token.at(0) == '!');
                if (inst->negation) {
                    inst->token = inst->token.substr(1);
                }

                // Debe existir definida en instructionsInfoList.
                if (!instructionsInfoList.IdExists(inst->token)) {
                    error.Show(line + " . Unknown instruction.", zarpa::errorType::error, fileName, linesCount);
                    continue;
                }

                // Último paso para comprobar la instrucción. Que hayan puesto "!" sir ser condicional
                inst->info = instructionsInfoList.Get(inst->token);
                if (inst->negation && !inst->info->conditional) {
                    error.Show(line + " . Instruction is not conditional.", zarpa::errorType::error, fileName, linesCount);
                    continue;
                }

                // Siendo una instrucción válida. Ahora se comprueban que el número de parámetros
                // para la actual instrucción sea correcto.
                bool errorFound {false}; // Necesario, porque un continue, se aplica al este for
                for (size_t p = 0; p < zarpa::max_params; p++) {
                    inst->paramsId[p] = tokens.at(p+1);

                    // ¿Es ausencia de parámetro y se ha especificado?
                    if (inst->info->paramsType[p] == zarpa::paramType::no_param && inst->paramsId[p] != "") {
                        error.Show(line + " . Too much parameters.", zarpa::errorType::error, fileName, linesCount);
                        errorFound = true;
                        break;
                    }

                    // Caso contrario, que falte el parámetro
                    if (inst->info->paramsType[p] != zarpa::paramType::no_param && inst->paramsId[p] == "") {
                        error.Show(line + " . Too few parameters.", zarpa::errorType::error, fileName, linesCount);
                        errorFound = true;
                        break;

                    }
                } // for p
                if (errorFound) {
                    continue;
                }

                // Sabemos que el número de parámetros son correctos. Ahora hay que comprobar que
                // según el tipo, sea un valor válido.
                for (size_t p = 0; p < zarpa::max_params; p++) {
                    if (inst->info->paramsType[p] == zarpa::paramType::no_param) { //Bitwise comprobación
                        break;
                    }

                    // Un bucle que recorre todos los tipos posibles. InstructionInfo ya los tiene
                    // en forma de vector ;-)
                    bool idFound {false};
                    for (auto type : inst->info->vecParamsType[p]) {
                        // Si es un verbo se prueba con verbo normal y de movimiento. Algunos parámetros
                        // pueden ser verbo en general y otros solo de movimiento
                        if (type == zarpa::paramType::word_verb || type == zarpa::paramType::word_mov_verb) {
                            if (vocabularyList.IdExists(inst->paramsId[p], zarpa::wordType::verb)) {
                                inst->paramsType[p] = zarpa::paramType::word_verb;
                                idFound = true;
                            }
                            else if (vocabularyList.IdExists(inst->paramsId[p], zarpa::wordType::mov_verb)) {
                                inst->paramsType[p] = zarpa::paramType::word_mov_verb;
                                idFound = true;
                            }
                        }
                        // Si es un nombre se comprueba con nombre normal y de objeto
                        if (type == zarpa::paramType::word_noun || type == zarpa::paramType::word_obj_noun) {
                            if (vocabularyList.IdExists(inst->paramsId[p], zarpa::wordType::noun)) {
                                inst->paramsType[p] = zarpa::paramType::word_noun;
                                idFound = true;
                            }
                            else if (vocabularyList.IdExists(inst->paramsId[p], zarpa::wordType::obj_noun)) {
                                inst->paramsType[p] = zarpa::paramType::word_obj_noun;
                                idFound = true;
                            }                            
                        }
                        // Si es una variable
                        if (type == zarpa::paramType::var) {
                            if (variablesList.IdExists(inst->paramsId[p])) {
                                inst->paramsType[p] = zarpa::paramType::var;
                                idFound = true;
                            }
                        }
                        // Si es un número
                        if (type == zarpa::paramType::num) {
                            if (IsNumber(inst->paramsId[p])) {
                                inst->paramsType[p] = zarpa::paramType::num;
                                idFound = true;
                            }
                        }
                        // Mensaje
                        if (type == zarpa::paramType::msg) {
                            if (messagesGroupList.IdExists(inst->paramsId[p])) {
                                inst->paramsType[p] = zarpa::paramType::msg;
                                idFound = true;
                            }
                        }
                        // Localidad
                        if (type == zarpa::paramType::loc) {
                            if (locationsList.IdExists(inst->paramsId[p])) {
                                inst->paramsType[p] = zarpa::paramType::loc;
                                idFound = true;
                            }
                        }
                        // Entidad
                        if (type == zarpa::paramType::ent) {
                            if (entitiesList.IdExists(inst->paramsId[p])) {
                                inst->paramsType[p] = zarpa::paramType::ent;
                                idFound = true;
                            }
                        }
                        
                        //TODO: COMPROBAR NÚMERO TAMBIÉN

                        // Si se ha encontrado un tipo de parámetro, salimos del bucle 
                        // y buscamos para el siguiente parám.
                        if (idFound) {
                            break;
                        }

                    } //for auto type

                    // idFound nos indica si el ID del parámetro pertenecía a alguna lista de objetos
                    // alguna variable o número válidos.
                    if (!idFound) {
                        error.Show(line + " Parameter #" + std::to_string(p) + " has incorrect ID.", 
                        zarpa::errorType::error, fileName, linesCount);
                    }

                }

                // Es momento de meter la instrucción en la tabla, pero si hay errores, no se
                // hace, tan solo se recorre toda la tabla para informar de más errores
                if (!error.counts[zarpa::errorType::error]) {
                    entry->instructions.push_back(inst);
                }
            } // else token.at(0) :
        } // for line

        // Si hay errores, no se procesan más tablas
        if (error.counts[zarpa::errorType::error] != 0) {
            return false;
        }

    } // for pair

    if (appInfo.devMode) {
        DEV_ShowTables();
    }
    return true;

}


bool ResourceManager::LoadData()
{
    if (appInfo.verbose) {
        std::cout << "\nLoading Data....\n";
    }
    error.ResetCounts();


    //
    // VARIABLES
    //
    std::shared_ptr<Variable> var;

    // Variables del sistema y del usuario
    for (const auto& fileName : {filePaths["variables"], filePaths["system variables"]}) {
        if (!ini.LoadFromFile(fileName)) {
            error.Show("Unable to load variables file.", zarpa::errorType::fatal_error, fileName);
            return false;
        }

        for (const auto& section : ini.GetSections()) {
            for (const auto& id : ini.GetSectionKeys(section)) {
                if (variablesList.IdExists(id)) {
                    error.Show("variable ID '" + id + "' already exists. (ignored).", zarpa::errorType::warning, fileName);
                    continue;
                }                    
                var = std::make_shared<Variable>();
                // Se comprueba el tipo de variable
                if ( section == "normal") {
                    var->type = zarpa::varType::normal;
                } else if ( section == "increase") {
                    var->type = zarpa::varType::increase;
                } else if ( section == "decrease") {
                    var->type = zarpa::varType::decrease;
                } else {
                    error.Show("variable ID '" + id + "'. Unknown variable type [" + section +"].",
                            zarpa::errorType::warning, fileName);
                    var->type = zarpa::varType::normal;
                }
                var->value = ini.GetValue<int_t>(section, id);
                variablesList.Add(var, id);
            }
        }
    }


    //
    // LOCALIDADES (y zonas)
    //
    std::shared_ptr<Location> loc;
    std::shared_ptr<Zone> zone;

    for (const auto& fileName : {filePaths["system locations"], filePaths["locations"]}) {
        if (!ini.LoadFromFile(fileName)) {
            error.Show("Unable to load locations file.", zarpa::errorType::fatal_error, fileName);
            return false;
        }
        for (const auto& id : ini.GetSections()) {
            if (locationsList.IdExists(id)) {
                error.Show("Location ID '" + id + "' already exists. (ignored).", zarpa::errorType::warning, fileName);
            } else {
                loc = std::make_shared<Location>();
                loc->id = id;
                loc->description = ini.GetValue(id, "des");
                if (loc->description != "" && !descriptionsList.IdExists(loc->description)) {
                    error.Show("Location desc. '" + loc->description + "' doesn't exists.", zarpa::errorType::error, fileName);
                }
                loc->imageFile = ini.GetValue(id, "image");
                loc->musicFile = ini.GetValue(id, "music");            
                loc->zone = ini.GetValue(id, "zone");
                locationsList.Add(loc, id);
                // Las zonas se crean dinámicamente conforme van siendo asignadas a localidades
                if (loc->zone != "" && !zonesList.IdExists(id)) {
                    zone = std::make_shared<Zone>();
                    zonesList.Add(zone, loc->zone);
                }
            }
        }
    }


    // Creación de las conexiones una vez tenemos todas las localidades creadas.
    // Las localidades del sistema no están conectadas.
    for (const auto& id : ini.GetSections()) { // Fichero localides de usuario es el activo.
        for (const auto& exit : zarpa::exits ) {
            if (ini.KeyExists(id, exit)) {
                auto exitToLocId = ini.GetValue(id, exit);
                if (!locationsList.IdExists(exitToLocId)) {
                    error.Show("From location '" + id + "' to " + exit + "='" + exitToLocId + 
                            "'. Destination doesn't exits.", zarpa::errorType::warning);
                    continue;
                }
                locationsList.Get(id)->exits[exit] = exitToLocId;
            }
        }
    }


    //
    // OBJETOS
    //
    if (!ini.LoadFromFile(filePaths["objects"])) {
        error.Show("Unable to load objects file.", zarpa::errorType::fatal_error, filePaths["objects"]);
        return false;
    }
    
    std::shared_ptr<Object> obj;
    for (const auto& id : ini.GetSections()) {
        if (objectsList.IdExists(id)) {
            error.Show("Object ID '" + id + "' already exists. (ignored).", zarpa::errorType::warning, filePaths["objects"]);
            continue;
        }
        obj = std::make_shared<Object>();
        obj->id = id;

        obj->noun = ini.GetValue(id, "noun");
        if (obj->noun == "") {
            error.Show("In object ID '" + id + "'. Noun is empty.",
                    zarpa::errorType::error, filePaths["objects"]);                
        } else if (!vocabularyList.IdExists(obj->noun, zarpa::wordType::obj_noun)) {
            error.Show("In object ID '" + id + "'. Noun ID '" + obj->noun + "' doesn't exits in vocabulary.", 
                    zarpa::errorType::error, filePaths["objects"]);                
        }     

        // Textos que lo definen
        obj->exam = ini.GetValue(id, "exam");
        if (obj->exam == "") {
            error.Show("In object ID '" + id + "'. Exam message is empty.",
                    zarpa::errorType::error, filePaths["objects"]);
        } else if (!messagesGroupList.IdExists(obj->exam)) {
            error.Show("In object ID '" + id + "'. Exam message ID '" + obj->exam + "' doesn't exits in messages.", 
                    zarpa::errorType::error, filePaths["objects"]);                
        }   

        obj->adjective =  ini.GetValue(id, "adjective");
        // Aún siendo opcional, si se define debe existir para evitar errores posteriores
        if (obj->adjective != "") {
            if (!vocabularyList.IdExists(obj->adjective, zarpa::wordType::adjective)) {
                error.Show("In object ID '" + id + "', adjective ID '" + obj->adjective + "' doesn't exits.", 
                        zarpa::errorType::error, filePaths["objects"]);                
            }
        }

        obj->isPlural = ini.GetValue<bool>(id, "plural");
        obj->isDefinite = ini.GetValue<bool>(id, "definite");

        auto gender = ini.GetValue(id, "gender");
        if (gender == "m") {
            obj->gender = zarpa::gender::male;
        } else if (gender == "f") {
            obj->gender = zarpa::gender::female;
        } else {
            obj->gender = zarpa::gender::neutral;
        }

        // Características físicas
        obj->location = ini.GetValue(id, "location", "_locVoid");
        if (!locationsList.IdExists(obj->location)) {
            error.Show("In object ID '" + id + "', location ID '" + obj->location + "' doesn't exits.", 
                zarpa::errorType::fatal_error, filePaths["objects"]);  
            return false;              
        }
        obj->isCarriable = ini.GetValue<bool>(id, "carriable", true);
        obj->isWearable = ini.GetValue<bool>(id, "wearable");
        obj->weight = ini.GetValue<uint_t>(id, "weight");
        obj->container = ini.GetValue(id, "container location");

        // Si se indica contenedor, se crea esa localidad que actuará como lugar de destino
        if (obj->container != "" && !locationsList.IdExists(obj->container)) {
            loc = std::make_shared<Location>();
            loc->id = obj->container;
            locationsList.Add(loc, loc->id);
        }
        objectsList.Add(obj, obj->id);
    }    


    //
    // ENTIDADES
    // 
    std::shared_ptr<Entity> ent;

    for (const auto& fileName : {filePaths["entities"], filePaths["system entities"]}) {
        if (!ini.LoadFromFile(fileName)) {
            error.Show("Unable to load entities file.", zarpa::errorType::fatal_error, fileName);
            return false;
        }

        for (const auto& id : ini.GetSections()) {
            ent = std::make_shared<Entity>();
            ent->id = id;
            // _entPlayer no puede empezar en _locVoid por defecto, hay que ser explícito.
            if (id == "_entPlayer") {
                ent->location = ini.GetValue(id, "location");
            } else {
                ent->location = ini.GetValue(id, "location", "_locVoid");
            }
            if (!locationsList.IdExists(ent->location)) {
                error.Show("In entity ID '" + id + "', location ID '" + ent->location + "' doesn't exits.", 
                    zarpa::errorType::fatal_error, fileName);  
                return false;              
            }
            ent->startLocation = ent->location;  // Localidad importante en el jugador!
            ent->zone = ini.GetValue(id, "zone");
            ent->container = ini.GetValue(id, "container");
            // Si se indica contenedor, se crea esa localidad que actuará como lugar de destino
            if (ent->container != "" && !locationsList.IdExists(ent->container)) {
                loc = std::make_shared<Location>();
                loc->id = ent->container;
                locationsList.Add(loc, loc->id);
            }

            // La variable que se reseteará al cambiar de localidad
            ent->varResetOnNewLoc = ini.GetValue("id", "varResetOnNewLoc");
            if (ent->varResetOnNewLoc != "") {
                // Si no existe se pone de nuevo a ""
                if (!variablesList.IdExists(id)) {
                    error.Show("In entity ID'" + id + "', variable '" + ent->varResetOnNewLoc + "' doesn't extis.",
                    zarpa::errorType::fatal_error, fileName);  
                }
            }
            entitiesList.Add(ent, ent->id);
        }
    }

    //
    // SONIDOS
    //
    if (!ini.LoadFromFile(filePaths["sounds"])) {
        error.Show("Unable to load sounds file.", zarpa::errorType::fatal_error, filePaths["sounds"]);
        return false;
    }
    std::shared_ptr<Sound> sound;
    for (const auto& section : ini.GetSections()) {
        for (const auto& id : ini.GetSectionKeys(section)) {
            if (soundsList.IdExists(id)) {
                error.Show("Sound ID '" + id + "' already exists (ignored).", zarpa::errorType::warning, filePaths["sounds"]);
            }
            sound = std::make_shared<Sound>();
            sound->fileName = ini.GetValue(section, id);
            if (sound->fileName == "") {
                error.Show("Sound ID '" + id + "' is empty (ignored).", zarpa::errorType::warning, filePaths["sounds"]);
            } 
            soundsList.Add(sound, id);
        }
    } 

    if (appInfo.verbose) {
        std::cout << std::to_string(locationsList.Count()) << " System and User Locations generated.\n";
        std::cout << std::to_string(zonesList.Count()) << " Zones generated.\n";
        std::cout << std::to_string(objectsList.Count()) << " Objects generated.\n";
        std::cout << std::to_string(entitiesList.Count()) << " Entities generated.\n";
        std::cout << std::to_string(soundsList.Count()) << " Sounds generated.\n";
    }

    if (appInfo.verbose) {
        error.Summary();
    }
    if (appInfo.devMode) {
        std::cout << "[DEV] " << std::to_string(error.counts[zarpa::errorType::dev_error]) << " DEV. Errors.\n";
    }

    return true;
}



bool ResourceManager::LoadTexts()
{
    if (appInfo.verbose) {
        std::cout << "\nLoading Texts...." << std::endl;
    }
    error.ResetCounts();

    //
    // DESCRIPCIONES
    //
    std::shared_ptr<Description> desc;
    IniDescription iniDesc;
    // IniDescription es una clase específica para leer este tipo de ficheros. La cabecera sería el
    // id y el resto de texto el mensaje. No he podido usar la clase Ini normal, ya que la diseñé
    // para que ordenara lineas y eliminara lineas vacías.
    if (!iniDesc.LoadFromFile(filePaths["descriptions"])) {
        error.Show("Unable to load descriptions file ", zarpa::errorType::fatal_error, filePaths["descriptions"]);
        return false;
    }  

    for (auto& pairValue : iniDesc.GetPairValues()) {
        desc = std::make_shared<Description>();
        desc->text = pairValue.second;
        descriptionsList.Add(desc, pairValue.first);
    }

    if (appInfo.verbose) {
        std::cout << std::to_string(descriptionsList.Count()) << " Descriptions generated." << std::endl;
    }

    //
    // MENSAJES
    //
    std::shared_ptr<MessageGroup> msgGroup;
    for (const auto& fileName : {filePaths["system messages"], filePaths["messages"]}) {
        if (!ini.LoadFromFile(fileName)) {
            error.Show("Unable to load messages file.", zarpa::errorType::fatal_error, fileName);
            return false;
        }
        for (const auto& section : ini.GetSections()) {
            for (const auto& msgGroupId : ini.GetSectionKeys(section)) {
                msgGroup = std::make_shared<MessageGroup>();
                auto text = ini.GetValue(section, msgGroupId);
                if (text == "") {
                    error.Show("Message '" + msgGroupId + "' is empty.", zarpa::errorType::warning);
                }
                msgGroup->Add(text);
                messagesGroupList.Add(msgGroup, msgGroupId);
            }
        }
    }


    //
    // VOCABULARIO
    //
    std::shared_ptr<Vocabulary> voc;   
    for (const auto& fileName : {filePaths["system vocabulary"], filePaths["vocabulary"]}) {
        if (!ini.LoadFromFile(fileName)) {
            error.Show("Unable to load vocabulary file.", zarpa::errorType::fatal_error, fileName);
            return false;
        }
        zarpa::wordType type;
        for (const auto& section : ini.GetSections()) {
            if (section == "verbs") {
                type = zarpa::wordType::verb;
            } else if (section == "movement verbs") {
                type = zarpa::wordType::mov_verb;
            } else if (section == "nouns") {
                type = zarpa::wordType::noun;
            } else if (section == "personal nouns") {
                type = zarpa::wordType::per_noun;
            } else if (section == "object nouns") {
                type = zarpa::wordType::obj_noun;
            } else if (section == "prepositions") {
                type = zarpa::wordType::preposition;
            } else if (section == "pronouns") {
                type = zarpa::wordType::pronoun;
            } else if (section == "adjectives") {
                type = zarpa::wordType::adjective;
            } else if (section == "articles") {
                type = zarpa::wordType::article;
            } else if (section == "indefinite articles") {
                type = zarpa::wordType::indef_article;
            } else if (section == "conjunctions") {
                type = zarpa::wordType::conjunction;
            } else if (section == "adverbs") {
                type = zarpa::wordType::adverb;
            } else if (section == "debug") {
                type = zarpa::wordType::debug;
            } else {
                error.Show("Invalid vocabulary section [" + section + "] (ignored).", zarpa::errorType::error, fileName);
                continue;
            }
            
            for (const auto& key : ini.GetSectionKeys(section)) {
                voc = std::make_shared<Vocabulary>();
                voc->id = key;
                voc->type = type;
                voc->words = ini.GetSplitValues(section, key);
                // Ignorar IDs vacíos sólo se aplica a IDs de usuario
                if (voc->words.empty()) {
                    error.Show("At [" + section + "] ID '" + key +"' is empty (ignored).", 
                        zarpa::errorType::warning, fileName);
                }
                else {
                    //TODO: Revisar que la palabra exista y avisar de palabra duplicada.
                    // Se comprueba que no exista ese ID.
                    if (vocabularyList.IdExists(voc->id, voc->type)) {
                        error.Show("At [" + section + "], duplicated ID '" + key +"' (ignored).", 
                            zarpa::errorType::warning, fileName);
                        
                    } // Hay que comprobar todas las palabras
                    else {
                        for (const auto& word : voc->words) {
                            if (vocabularyList.WordExits(word, voc->type)) {
                                error.Show("At [" + section + "] word '" + word + "' duplicated in ID '" + key + "'. (ignored).", 
                                    zarpa::errorType::warning, fileName);
                            }
                        }
                    }
                    vocabularyList.Add(voc);
                }
            }
        }
    }

    {
        if (appInfo.verbose) {            
            std::cout << std::to_string(messagesGroupList.Count()) << " Messages generated." << std::endl;
            size_t idsCount {0}, wordsCount {0};
            for (size_t type = 0; type < static_cast<size_t>(zarpa::wordType::wordType_count); type++) {
                idsCount += vocabularyList.IdsCount(type);
                wordsCount += vocabularyList.WordsCount(type);
            }
            std::cout << std::to_string(wordsCount) << " Vocabulary words generated." << std::endl;
            //FIXME: std::cout << std::to_string(idsCount) << " Vocabulary IDs generated." << std::endl;
        }
    }

    if (appInfo.verbose) {
        error.Summary();
    }
    if (appInfo.devMode) {
        std::cout << "[DEV] " << std::to_string(error.counts[zarpa::errorType::dev_error]) << " DEV. Errors.\n";
    }

    return true;
}


// Formatea cada linea de del script en una linea con cada elemento separado por un espacio
// la entradas las formatea con 3 palabras ": verbo nombre"
std::vector<std::string> ResourceManager::GetTableLines(const std::string& fileName)
{
    std::ifstream file;
    file.open(fileName);
    //TODO: Pendiente de diferenciar un error de acceso al fichero con un fichero vacío
    if (file.bad() || file.fail()) {
        return {""};
    }
    std::vector<std::string> buffer;
    for (std::string line; getline(file, line);) {
        //Quitar comentarios
        auto pos = line.find('#');
        if (pos != std::string::npos) {
            line = line.substr(0,pos);
        }
        //Quitar doble espacio.
        line = std::regex_replace(line, std::regex("  "), " ");
        Trim(line);
        buffer.push_back(line);
    }
    return buffer;
}