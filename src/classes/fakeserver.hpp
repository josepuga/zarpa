#ifndef A4725FDF_FBF9_4C9A_B3B5_C491E47ECF10
#define A4725FDF_FBF9_4C9A_B3B5_C491E47ECF10
#include <functional>
#include <string>

// Clase dummy mientras creo o consigo un cliente/server TCP o UDP para C++ multiplataforma
// Esta clases es un intento de simular un cliente. Send() mandará el texto introducido por el
// jugador y SetRecvCallback() es la función que será llamada por el GUI cuando haya prompt

// El GUI tiene una clase igual pero llamada FakeSocket

class FakeServer
{
    using func_t = std::function <void(std::string)>;
    public:
    void Send(const std::string& s) {m_sendCallback(s);}
    void SetRecvCallback(func_t f) {m_recvCallback = f;}
    void SetSendCallback(func_t f) {m_sendCallback = f;}

    private:
    func_t m_sendCallback;
    func_t m_recvCallback;
};


#endif /* A4725FDF_FBF9_4C9A_B3B5_C491E47ECF10 */
