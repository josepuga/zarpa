#include <cstdlib>
#include <ctime>
#include "script.hpp"
#include "resource_manager.hpp"

extern ResourceManager rm;

void Script::Run()
{

    srand (time(NULL));

    // Bucle para ejecutar los distintos scripts.
    // Y sí, uso goto. Después de tantos años me hacía ilusión usarlo. Niños no hagáis esto en casa.
    using namespace zarpa;

    // Apuntamos _locPlayer a la localidad del jugador. 
    SetLocPlayer();

    m_state = scriptState::desc;
    while (true) {
        if (m_state == scriptState::desc) {
            m_currentTable = tables[tableType::userBeforeDesc];
            RunTable();
            ShowDescription();
            m_currentTable = tables[tableType::userAfterDesc];
            RunTable();
        }

        m_state = scriptState::running;
        m_currentTable = tables[tableType::userBeforePrompt];
        RunTable();

        while (rm.phrases.empty()) {
            // DESCOMENTAR PARA TESTEAR SIN GUI
            /******
            std::cout << "> ";
            std::string line;
            for (; std::getline(std::cin, line);) {
                break;
            }
            //TEST_Parse(line); // Destruye la frase
            Parse(line);
            *****/
        }

        // SOLUCIÓN TEMPORAL
        if (!rm.phrases.empty() && rm.phrases.front()->verb == "/quit") return;

        m_state = scriptState::running;
        m_currentTable = tables[tableType::userAfterPrompt];
        RunTable();
        if (m_state == scriptState::desc) {
            if (!rm.phrases.empty()) rm.phrases.pop();
            continue;
        }
        if (m_state == scriptState::notdone) {
            rm.phrases = {};
        }    
        if (m_state == scriptState::done) {
            rm.phrases.pop();
        }

        if (m_state != scriptState::notdone && m_state != scriptState::done) {
            m_currentTable = tables[tableType::systemAfterPrompt];
            RunTable();
            if (m_state == scriptState::desc) {
                if (!rm.phrases.empty()) rm.phrases.pop();
                continue;
            }
            if (m_state == scriptState::notdone) {
                rm.phrases = {};
            }    
            if (m_state == scriptState::done) {
                rm.phrases.pop();
            }
            m_state = scriptState::running;
        }
    }
}


void Script::RunTable()
{
    // Hace falta esta variable, ya que a diferencia de PAW, las entradas vacias anidadas,
    // cumplen condición en la próxima tabla si CUALQUIER entrada la cumplió.
    bool entryCond {false}; 
    for (size_t i = 0; i < m_currentTable->entries.size(); i++) {
        m_currentEntry = m_currentTable->entries[i];
        if (EntryIsValid(m_currentEntry)) {
            entryCond = true;
        }
        // La entrada no está vacía y se cumple cond. de ésta o alguna vacía anterior
        if (!m_currentEntry->instructions.empty() && entryCond) {
            RunEntry();
            entryCond = false;
        }

        if ((m_state == zarpa::scriptState::desc) || (m_state == zarpa::scriptState::done) || 
                (m_state == zarpa::scriptState::notdone)) {
            break;
        }
    }
}


void Script::RunEntry()
{
    m_condSuccess = true;
    for (size_t i = 0; i < m_currentEntry->instructions.size(); i++) {
        m_currentInst = m_currentEntry->instructions[i];

SetLocPlayer();

        RunInstruction();
        if (!m_condSuccess) break;
        if ((m_state == zarpa::scriptState::desc) || (m_state == zarpa::scriptState::done) || 
                (m_state == zarpa::scriptState::notdone)) {
            break;
        }        
    }
}


void Script::RunInstruction()
{

    //std::cout << "[" << m_currentInst->line << "]"
    //<< "(" << m_currentInst->token << " " << m_currentInst->paramsId[0] << " . " << m_currentInst->paramsId[1] << ")" << std::endl;
    switch (hash_(m_currentInst->token.c_str()))
    {

        case "eq"_hash : eq(); break;
        case "le"_hash : le(); break;
        case "ge"_hash : ge(); break;
        case "set"_hash : set(); break;
        case "add"_hash : add(); break;
        case "sub"_hash : sub(); break;


        case "atloc"_hash : atloc(); break;
        case "atzone"_hash : atzone(); break;



        //TODO: Vocabulary

        case "voc"_hash : voc(); break;
        case "here"_hash : here(); break;
        case "empty"_hash : empty(); break;
        case "swap"_hash : swappos(); break;
        case "put"_hash : put(); break;
        case "drop"_hash : drop(); break;
        case "get"_hash : get(); break;
        case "remove"_hash : remove(); break;
        case "wear"_hash : wear(); break;
        case "exam"_hash : exam(); break;
        case "doall"_hash : doall(); break;

        case "msg"_hash : msg(); break;
        case "msgln"_hash : msgln(); break;
        case "desc"_hash : desc(); break;
        case "setdesc"_hash : setdesc(); break;
        case "done"_hash : done(); break;
        case "notdone"_hash : notdone(); break;


        case "canexit"_hash : canexit(); break;
        case "goto"_hash : goto_(); break;
        case "moveto"_hash : moveto(); break;
        case "chance"_hash : chance(); break;
        default: nop(); break;
    }
}
