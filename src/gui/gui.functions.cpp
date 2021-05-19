#include <iostream>
#include <codecvt>
#include <stack>
#include "gui.hpp"

void Gui::ParseBufferServer()
{
    // Para procesar todos los mensajes de una vez
    bool messageAdded;
    for (auto s : bufferServer) {
        //Mínimo 5 caracteres por el "tag" [xxx]
        if (s.size() < 5) {
            ShowError("From Zarpa server: '" + s + "' command unknown.");
            continue;
        }
        // Estraemos el tag y "saneamos la cadena"
        auto tag = s.substr(0,5);
        s = s.substr(5, s.size());
        if (tag == "[des]") {
            texts[gui::resType::description].setString(StringToWString(s));
            TextWrap(texts[gui::resType::description]);
        }
        else if (tag == "[img]") {
            //TODO: IMAGEN
        }
        else if (tag == "[snd]") {
            ShowError("Play sound '" + s + "' not implemented yet.");
        }
        else if (tag == "[msc]") {
            ShowError("Play music '" + s + "' not implemented yet.");
        }
        else if (tag == "[dbg]") {
            ShowError("Debug command '" + s + "' not implemented yet.");
        }
        else if (tag == "[msg]") {
            messageAdded = true;
            AddMessage(s);
        }
        else {
            ShowError("Tag '" + tag + "' not implemented yet.");
        }
    }
    if (messageAdded) {
        //TODO: SEPARAR FitMessageText
    }
}

void Gui::RecvData(const std::string& message)
{
    mtxBufferServer.lock();
    bufferServer.push_back(message);
    mtxBufferServer.unlock();
}

void Gui::ShowError(const std::string& message)
{
    std::cerr << "[GUI] " << message << std::endl;
}


void Gui::AddMessage(const std::wstring& message)
{
    // https://www.techiedelight.com/iterate-deque-forward-backward-cpp/
    // http://www.cplusplus.com/forum/general/68915/
    messagesBuffer.push_back(message);
    //TODO: COMPROBAR LONGITUD Y BORRAR ANTIGUA LINEA
    FitMessagesText();
}

void Gui::FitMessagesText()
{
    unsigned width = window.videoMode.width - (theme.margins[gui::margin::left] + theme.margins[gui::margin::right]);
    unsigned height = theme.textStyles[gui::resType::message].maxHeight;

    // Se recorre la cola primero en orden inverso, primero el más nuevo. Esto optimiza ya que vamos
    // a buscar cuantas lineas de texto caben desde la última. Así que empezamos con la ultima y
    // añadimos abajo las siguientes. Luego sabiendo las lineas que caben, se mostrarán en orden
    // normal, para ello se usa un stack.
    std::wstring buffer; 
    std::stack<std::wstring> bufferStack;
    for (auto it = messagesBuffer.rbegin(); it != messagesBuffer.rend(); it++) {
        buffer += *it;// + L'\n';
        // Hay que "wrapeando" cada linea una a una. Me parece un gasto de proceso increible
        // aparte de tener que estar creando un string cada vez, pero no he encontrado otra manera
        // debido a la forma en que funciona el texto en SFML.
        texts[gui::resType::message].setString(buffer);

        TextWrap(texts[gui::resType::message], width);
        
        // No salimos de la altura? 
        if (texts[gui::resType::message].findCharacterPos(buffer.size()-1).y > 
                theme.textStyles[gui::resType::message].y + height) {
            break;
        }
        bufferStack.push(*it);// + L'\n');
    }   
    // Reinvertimos las lineas, ya que aparece la más nueva abajo. Usamos el mismo buffer
    buffer.clear(); 
    while (!bufferStack.empty()) {
        buffer += bufferStack.top();
        bufferStack.pop();
    }
    texts[gui::resType::message].setString(buffer);
}


// Genera los saltos de linea en el texto para justificar las palabras
void Gui::TextWrap(sf::Text& text, unsigned int width)
{
    if (width == 0) {
        width = window.videoMode.width - (theme.margins[gui::margin::left] + theme.margins[gui::margin::right]);
    }
    auto newText = text.getString().toWideString();
    size_t spacePos {0}; // Posición del último espacio
    int index {-1};
    for (auto curChar : text.getString().toWideString()) {
        index++;
        switch (curChar) {
           case U' ': spacePos = index; break;
           case U'\r': continue;
           default: break;
        }

        if (text.findCharacterPos(index).x > width) {
            //No cambia la longitud, tan sólo se sustituye ' ' por '\n'
            newText.at(spacePos) = U'\n';
            text.setString(newText); 
        }
    }
}

// Snipper extraido de https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
// Gracias al usuario Martin Ba
std::wstring Gui::StringToWString(const std::string& str)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

std::string Gui::WStringToString(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}