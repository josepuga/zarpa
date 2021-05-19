#include <ini/ini.hpp>
#include <cassert>
#include "gui.hpp"
#include "ttf_descriptions_default.hpp"
#include "ttf_messages_default.hpp"
#include "icon.hpp"

bool Gui::LoadData(const std::string& zarpaIni)
{
    Ini ini;
    if (!ini.LoadFromFile(zarpaIni)) {
        ShowError("Unable to open Zarpa config file '" + zarpaIni + "'.");
        return false;
    }

    // zarpa.ini contiene el directorio de la aventura, así como información en la sección [gui]
    // resolución
    // bpp
    // Máx. imagenes/sonidos precargados
    // Si se muestran imágenes

    folders[gui::folder::adventure] = ini.GetValue("adventure settings", "folder");

    auto sizes = ini.GetSplitValues<unsigned>("gui", "resolution", 'x');        
    if (sizes.size() != 2) {
        sizes = {1280, 1024};
    }
    window.videoMode.width = sizes[0];
    window.videoMode.height = sizes[1];
    window.videoMode.bitsPerPixel = ini.GetValue<unsigned>("gui", "bpp", sf::VideoMode::getDesktopMode().bitsPerPixel);
    displayImages = ini.GetValue<bool>("gui", "display images");

    // config.ini contiene algunos datos de la aventura, así como el tema activo.
    // También los diferentes directorios

    auto f = folders[gui::folder::adventure] + "config.ini";
    if (!ini.LoadFromFile(folders[gui::folder::adventure] + "config.ini")) {
        ShowError("Unable to open adventure config file '" + f + "'.");
        return false;
    }

    window.title = ini.GetValue("info", "title", "<NO TITLE>");
    window.title += ". " + ini.GetValue("info", "version");
    theme.fileName = ini.GetValue("info", "theme file");

    std::unordered_map<gui::folder, std::string> assets {
        {gui::folder::images, "images folder"},
        {gui::folder::music, "music folder"},
        {gui::folder::fonts, "fonts folder"},
        {gui::folder::sounds, "sounds folder"},
        {gui::folder::themes, "themes folder"},
    };
    for (const auto& pair : assets) {
        auto& folder = pair.first;
        auto& key = pair.second;
        folders[folder] = folders[gui::folder::adventure] + ini.GetValue("assets", key);
    }


    // Ya tenemos la información de la aventura, los directorios usados y lo más
    // importante: El fichero de tema activo. Es momento de cargar los assets....

    f =  folders[gui::folder::themes] + theme.fileName;
    if (!ini.LoadFromFile(f)) {
        ShowError("Unable to open theme file '" + f + "'.");
        return false;
    }

    // Altura proporcional de los distintos elementos
    std::string section;
    if (displayImages) {
        section = "image mode heights";
    } else {
        section = "text mode heights";
    }
    theme.imageStyle.maxHeight =  (window.videoMode.height * 
            ini.GetValue<unsigned>(section, "images")) / 100;
    theme.textStyles[gui::resType::description].maxHeight = (window.videoMode.height * 
            ini.GetValue<unsigned>(section, "descriptions")) / 100;
    theme.textStyles[gui::resType::message].maxHeight = (window.videoMode.height * 
            ini.GetValue<unsigned>(section, "messages")) / 100;
    theme.textStyles[gui::resType::prompt].maxHeight = (window.videoMode.height * 
            ini.GetValue<unsigned>(section, "prompt")) / 100;

    // Anchura, (de momento) es igual para todos.
    unsigned w = window.videoMode.width - (theme.margins[gui::margin::left] - theme.margins[gui::margin::left]);
    theme.imageStyle.maxWidth = w;
    theme.textStyles[gui::resType::description].maxWidth = w;
    theme.textStyles[gui::resType::message].maxWidth = w;
    theme.textStyles[gui::resType::prompt].maxWidth = w;


    // Márgenes y color de fondo
    theme.backgroundColor = sf::Color(ini.GetValueHex<uint32_t>("misc", "background color", 0x1e1e1eff));
    theme.margins[gui::margin::left] = ini.GetValue<unsigned>("misc", "margin sides", 20);
    theme.margins[gui::margin::right] = theme.margins[gui::margin::left];
    theme.margins[gui::margin::top] = ini.GetValue<unsigned>("misc", "margin top", 5);
    theme.margins[gui::margin::bottom] = ini.GetValue<unsigned>("misc", "margin bottom", 5);

    // Sabiendo tamaños, calculamos posiciones iniciales de cada elemento
    unsigned x, y, padding;
    x = theme.margins[gui::margin::left];
    y = theme.margins[gui::margin::top];
    padding = theme.margins[gui::margin::top]; // Como padding uso el mismo que el padding top.
    theme.imageStyle.x = x;
    theme.imageStyle.y = y;
    y += theme.imageStyle.maxHeight + padding;
    theme.textStyles[gui::resType::description].x = x;
    theme.textStyles[gui::resType::description].y = y;
    y += theme.textStyles[gui::resType::description].maxHeight + padding;
    theme.textStyles[gui::resType::message].x = x;
    theme.textStyles[gui::resType::message].y = y;
    // El prompt se calcula desde abajo
    // TODO:
    theme.textStyles[gui::resType::prompt].x = x;
    theme.textStyles[gui::resType::prompt].y = window.videoMode.height - 
            texts[gui::resType::prompt].getCharacterSize() - theme.margins[gui::margin::bottom];

            //window.videoMode.height - texts[gui::resType::prompt].getCharacterSize() - 20);//theme.margins[gui::margin::bottom]);


    // Fuentes TTF
    std::unordered_map<gui::resType, std::string> pairs {
        {gui::resType::description, "descriptions font"},
        {gui::resType::message, "messages font"},
        {gui::resType::prompt, "prompt font"},
    };

    for (const auto& pair : pairs) {
        auto type = pair.first;
        auto& section = pair.second;
        theme.textStyles[type].fontStyle.fileName = ini.GetValue(section, "file");
        theme.textStyles[type].fontStyle.color = sf::Color(ini.GetValueHex(section, "color", 0xddddddff));
        theme.textStyles[type].fontStyle.size = ini.GetValue<unsigned>(section, "size", 28);
        theme.textStyles[type].fontStyle.isBold = ini.GetValue<bool>(section, "bold");
        theme.textStyles[type].fontStyle.isItalic = ini.GetValue<bool>(section, "italic");
        theme.textStyles[type].fontStyle.letterSpacing = ini.GetValue<float>(section, "letter spacing", 0.0f);
        theme.textStyles[type].fontStyle.lineSpacing = ini.GetValue<float>(section, "line spacing", 0.0f);

        // Se prueba a cargar la fuente bien de fichero o por defecto en memoria si no tiene fichero
        if (theme.textStyles[type].fontStyle.fileName == "") {
            switch (type) {
                case gui::resType::description :
                    fonts[type].loadFromMemory(&ttf_descriptions_default, ttf_descriptions_default_len);
                    break;

                case gui::resType::message :
                    fonts[type].loadFromMemory(&ttf_messages_default, ttf_messages_default_len);
                    break;

                case gui::resType::prompt :
                    fonts[type].loadFromMemory(&ttf_messages_default, ttf_messages_default_len);
                    break;

                default : assert(false && "Unhandled resource type value."); break;
            }
        } else {
            auto f = folders[gui::folder::fonts] + theme.textStyles[type].fontStyle.fileName;
            if (!fonts[type].loadFromFile(f)) {
                ShowError("Unable to load TTF font file '" + f + "'.");
                return false;
            }

        }

        // Se ha cargado la fuente y tenemos su estilo. Ahora se aplica a los textos.
        // en SFML el estilo lo tiene el texto no la fuente. Aunque para intentar hacerlo más abstrato
        // y con vistas de usar otro GUI, el tema (exteriormente) se aplica a la fuente.
        texts[type].setFillColor(theme.textStyles[type].fontStyle.color);
        texts[type].setFont(fonts[type]);
        texts[type].setCharacterSize(theme.textStyles[type].fontStyle.size);
        texts[type].setLetterSpacing(theme.textStyles[type].fontStyle.letterSpacing);
        texts[type].setLineSpacing(theme.textStyles[type].fontStyle.lineSpacing);
        //TODO: AMBOS!
        if (theme.textStyles[type].fontStyle.isItalic) {
            texts[type].setStyle(sf::Text::Italic);            
        }
        if (theme.textStyles[type].fontStyle.isBold) {
            texts[type].setStyle(sf::Text::Bold);   
        }         
    }
    window.icon.loadFromMemory(&icon_png, icon_png_len);


    return true;
}
