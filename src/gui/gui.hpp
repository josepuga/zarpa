#ifndef DBA02CDD_679A_4ABE_B8C6_4F0FEA6DB8A6
#define DBA02CDD_679A_4ABE_B8C6_4F0FEA6DB8A6

#include <array>
#include <vector>
#include <string>
#include <deque>
#include <SFML/Graphics.hpp>
//#include <SFML/OpenGL.hpp> //Para glflush()
#include <mutex>


namespace gui {

    // Limites para la "caché" de imágenes y sonidos
    const unsigned maxPreloadImages = 5;
    const unsigned maxPreloadSounds = 5;

    enum mode : unsigned {
        text_only,
        show_images,
        mode_count
    };

    enum resType : unsigned {
        description, 
        message, 
        prompt, 
        resType_count    
    };

    enum margin : unsigned {
        top,
        bottom,
        left,
        right,
        margin_count
    };

    enum folder : unsigned {
        adventure,
        images,
        sounds,
        fonts,
        music,
        themes,
        folder_count
    };
}

// Clase dummy mientras creo o consigo un cliente/server TCP o UDP para C++ multiplataforma
// Esta clases es un intento de simular un cliente. Send() mandará el texto introducido por el
// jugador y SetRecvCallback() es la función que será llamada por el parser cuando haya
// salidas de texto.

class FakeSocket
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

struct FontStyle 
{
    bool isFromFile {true}; // Si no se especifica, se coge la default de memoria
    std::string fileName;
    unsigned size;
    sf::Color color;
    float letterSpacing;
    float lineSpacing;
    bool isItalic {false};
    bool isBold {false};   
};

struct TextStyle
{
    FontStyle fontStyle;
    float x;
    float y;
    unsigned maxHeight;
    unsigned maxWidth;
    std::string content;
};

struct ImageStyle
{
    float x;
    float y;
    unsigned maxHeight;
    unsigned maxWidth;
};

struct Window
{
    sf::VideoMode videoMode;
    std::string title;
    sf::Image icon;

};

struct Theme
{
    std::string author;
    std::string version;
    std::string name;
    std::string brief;
    std::string fileName;
    std::array<TextStyle, gui::resType::resType_count> textStyles;
    ImageStyle imageStyle;
    sf::Color backgroundColor;
    std::array<unsigned, gui::margin::margin_count> margins;
};

struct Gui 
{
    FakeSocket socket;

    Window window;
    //sf::RenderWindow renderWindow; --> No se puede, ya que no se puede inicializ. luego el estilo de la ventana
    Theme theme;
    std::array<sf::Font, gui::resType::resType_count> fonts;
    std::array<sf::Text, gui::resType::resType_count> texts;
    std::array<std::string, gui::folder::folder_count> folders;
    bool displayImages;
    std::deque<std::wstring> messagesBuffer;
    sf::Context context; //Previene error "Failed to activate OpenGL context: The handle is invalid"

    void Init();
    bool LoadData(const std::string&);
    void ShowError(const std::string& message);
    void AddMessage(const std::wstring& message);
    void AddMessage(const std::string& message) {AddMessage(StringToWString(message));}

    // Gestión de los datos enviados del servidor
    void RecvData(const std::string& s);
    void ParseBufferServer();
    std::mutex mtxBufferServer;
    std::vector<std::string> bufferServer;

    private:
    std::wstring StringToWString(const std::string& str);
    std::string WStringToString(const std::wstring& wstr);
    void TextWrap(sf::Text& text, unsigned width = 0);  
    // La salida de mensajes funciona de forma especial y es complicado ajustar el texto con
    // SFML. Hay que hacer algunos malavares.
    void FitMessagesText();  

};

#endif /* DBA02CDD_679A_4ABE_B8C6_4F0FEA6DB8A6 */
