#ifndef AC02EB0C_4A5E_48ED_8723_B0811C1B37EA
#define AC02EB0C_4A5E_48ED_8723_B0811C1B37EA

#include <array>
#include <vector>
#include <memory>
#include "zarpa_types.hpp"
#include "instructioninfo.hpp"
#include "zarpa.hpp"

/*
Todas las clases que se relacionan con el código Zarpascript, en un sólo fichero
*/

struct Instruction
{
    bool negation {false};
    std::string token;
    std::array<std::string, zarpa::max_params> paramsId;
    std::array<zarpa::paramType, zarpa::max_params> paramsType;
    std::shared_ptr<InstructionInfo> info;
    //Para depurar código
    size_t line {0}; 
    bool breakpoint {false};
};


struct Entry
{
    std::string verb;
    std::string noun;
    std::vector<std::shared_ptr<Instruction>> instructions {};
    //Para depurar código
    size_t line {0}; 
    bool breakpoint {false};
};


struct Table
{
    std::vector<std::shared_ptr<Entry>> entries {};
    //Para depurar código
    std::string fileName; 
};

// Estado de la última tabla procesada. Se guarda el estado tabla/entrada/instr.
// Usado cuando se depure, volver al último punto
// Uso un struct. Me aburren los setters y getters.

struct Script
{
    std::array<std::shared_ptr<Table>, zarpa::tableType::tableType_count> tables;
    std::shared_ptr<Table> m_currentTable;
    std::shared_ptr<Entry> m_currentEntry;
    std::shared_ptr<Instruction> m_currentInst;

    zarpa::scriptState m_state;
    bool foundBreakpoint {false};
    bool debugMode {false};


    // Lleva el indicador de que la última comando condicional acabó bien o no.
    bool m_condSuccess {false};

    // Si se tiene en cuenta la frase. La frases que haya en la cola de frases,
    // sólo se tienen en cuenta en los script AFTER PROMPT.
    bool usePhrase {false};

    void Run();    

    private:
    
    void ShowMessage(std::shared_ptr<MessageGroup> mg, bool newLine = false);
    void ShowMessage(std::string id, bool newLine = false);
    void ShowDescription();

    // Actualiza la localidad especial "_locPlayer" para que apunte a la actual del jugador
    void SetLocPlayer();

    // Hay que recorrer la tabla de objetos para averiguar cual tiene un determinado ID NOUN
    // asociado. Este método es útil para comandos como carried. que se basan el el ID NOUN pero
    // necesitan acceder al objeto
    std::string GetObjIdFromNounId(const std::string& nounId);

    // Para tratar con _MOV, _OBJ, etc. y prevenir chashes si no hay prompt
    std::string GetMovVerb();
    std::string GetVerb();
    std::string GetObjNoun();
    std::string GetNoun();


    // Para variables y números
    int_t GetIntValue(std::shared_ptr<Instruction> inst, unsigned paramN);

    // Devuelve si la entrada es válida comprobando VERBO/NOMBRE.
    bool EntryIsValid(std::shared_ptr<Entry> entry);
    void RunTable();
    void RunEntry();

    // Este método llama a las siguientes instrucciones
    void RunInstruction();
    void nop();
    
    // Manejo de variables
    void eq();
    void le();
    void ge();
    void set();
    void add();
    void sub();

    // Vocabulario
    void CheckVocWord(zarpa::paramType);
    void CheckVocWord2(zarpa::paramType);

    void voc();
    void adverb();
    void adject();
    void adject2();
    void noun();
    void noun2();
    void verb();
    void prep();


    void atloc();
    void atzone();

    // Manejo de objetos
    void swappos();
    void doall();
    void empty();
    void here();
    void put();
    void drop();
    void get();
    void remove();
    void wear();
    void exam();
    
/** ELIMINADO
    void carried();
    void worn();

**/

    void msg();
    void msgln();
    void desc(); //TODO:
    void setdesc();
    void done();
    void notdone();

    void canexit();
    void goto_();
    void moveto();
    void Move(std::shared_ptr<Entity> ent, std::shared_ptr<Location> loc);
    void Move(std::shared_ptr<Object> obj, std::shared_ptr<Location> loc);
    void Move(std::shared_ptr<Object> obj, std::shared_ptr<Object> objDest);

    void chance();
};

class ZarpaScript {
    public:
    Script script;
    //zarpa::scriptState exitStatus;
    // En modo debug el programa se detiene si encuentra un breakpoint.
    bool debugMode;
    void SetTable(std::shared_ptr<Table> table);
    void ToggleBreakpoint(size_t lineNumber);
    // Ejecuta la siguiente instrucción y se detiene
    void Step();
    // Ejecuta las instrucciones y se detiene en la siguiente tabla
    void GotoNextTable();
    void Run();

};


#endif /* AC02EB0C_4A5E_48ED_8723_B0811C1B37EA */
