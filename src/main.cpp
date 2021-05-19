// Hack para que la consola DOS de Windows muestre UTF8 (Probado con Windows 10)
#if defined(_WIN32)
#include <cstdio>
#include <windows.h>
    //Algunas versiones antiguas de MinGW/CYGWIN no definen este valor:
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#endif // _WIN32

#include <iostream>
#include <iomanip>
#include <functional>
#include <thread>
#include "zarpa.hpp"
#include "resource_manager.hpp"
#include "reflection.hpp"
#include "gui.hpp"


extern void Parse(const std::string&);

void TEST_Parse(const std::string&);

ResourceManager rm;


// Hack para que la consola DOS de Windows muestre UTF8 (Probado con Windows 10)
#if defined(_WIN32)
static DWORD dwMode1 = 0;
static auto dummyFoo = GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dwMode1);
static DWORD dwMode2 = dwMode1 |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
static auto dummyFoo2 = SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), dwMode2);
// UTF8 for Windows.
static auto dummyFoo3 = SetConsoleOutputCP( CP_UTF8 );
#endif // _WIN32

using namespace std;

int main () 
{

    Gui gui;
    if (!gui.LoadData("./zarpa.ini")) return 1;
    
    if (!rm.Init("./zarpa.ini")) return 1;



    // DESCOMENTAR PARA PROBAR SIN GUI. (también el código en en script.cpp dentro de Run())
    //rm.script->Run();

    // El fake servidor server y cliente socket. Se asignan las llamadas para comunicarse
    gui.socket.SetSendCallback(Parse);
    rm.server.SetSendCallback( // Uso lambda porque es un método no estático.
            [&] (const std::string& s) {
                gui.RecvData(s);
            }
    );

    // Creo 2 hilos, ya que en un futuro espero tener el GUI y el parser en ejecutables separados
    // y comunicarlos a través de un socket tcp o udp. Es una forma de dejarlo preparado.
    thread thrGui(&Gui::Init, &gui); //gui.Init();
    thread thrScript(&Script::Run, rm.script); //rm.script->Run(); (no lleva "&" porque ya es un puntero)

    thrGui.join();
    thrScript.join();

    cout << "\nEND.\n";
    return 0;
}


void TEST_Parse(const string& s)
{
    Parse(s);
    cout << "\n[DEV] " 
    << setw(10) << left << "VERB."
    << setw(7) << left << "ART." 
    << setw(10) << left << "NOUN."
    << setw(10) << left << "ADJ."
    << setw(7) << left << "PREP."
    << setw(7) << left << "ART2." 
    << setw(10) << left << "NOUN2."
    << setw(10) << left << "ADJ2." << endl;
    while (!rm.phrases.empty()) {
        auto phr = rm.phrases.front();
        rm.phrases.pop();
        cout << "[DEV] "
        << setw(10) << left << phr->verb
        << setw(7) << left << phr->article 
        << setw(10) << left << phr->noun
        << setw(10) << left << phr->adjective
        << setw(7) << left << phr->preposition
        << setw(7) << left << phr->article2 
        << setw(10) << left << phr->noun2
        << setw(10) << left << phr->adjective2 << "\n";
    }
}
