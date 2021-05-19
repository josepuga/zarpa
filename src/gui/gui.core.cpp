#include <cstdint>
#include <unordered_map>
#include <cassert>
#include <thread>
#include <chrono>
#include "gui.hpp"

void Gui::Init()
{

    texts[gui::resType::description].setPosition(theme.textStyles[gui::resType::description].x, 
            theme.textStyles[gui::resType::description].y);

    texts[gui::resType::message].setPosition(theme.textStyles[gui::resType::message].x, 
            theme.textStyles[gui::resType::message].y);

    std::wstring playerInput {L">_"};
    texts[gui::resType::prompt].setPosition(theme.textStyles[gui::resType::message].x,
            theme.textStyles[gui::resType::prompt].y);            
    texts[gui::resType::prompt].setString(playerInput);



    sf::RenderWindow rw(window.videoMode, StringToWString(window.title),
        sf::Style::Titlebar | sf::Style::Close);
    rw.setIcon(window.icon.getSize().x, window.icon.getSize().y, window.icon.getPixelsPtr());

    while (rw.isOpen()) {
        rw.clear(theme.backgroundColor);
        sf::Event event;
        while (rw.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                socket.Send("/quit");
                rw.close();
            }            

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Escape:
                        //rw.close(); 
                        break;
                    default: break;
                }
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8) { // Backspace
                    if (playerInput.size() > 2) {
                        playerInput.pop_back(); // "_"
                        playerInput.pop_back(); // Real char
                        playerInput.push_back(U'_');
                        texts[gui::resType::prompt].setString(playerInput);
                    }
                } else if (event.text.unicode == 13) {
                    playerInput.pop_back(); // "_"
                    AddMessage(playerInput + L'\n');
                    socket.Send(WStringToString(playerInput.substr(1,playerInput.size())));
                    playerInput = L">_";
                    texts[gui::resType::prompt].setString(playerInput);
                } else if (event.text.unicode >= 32) { // >= Space
                    playerInput.pop_back(); // "_"
                    playerInput.push_back(event.text.unicode);
                    playerInput.push_back(U'_');
                    texts[gui::resType::prompt].setString(playerInput);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                //
            }     

        } // while pollEvent

        // Se comprueba que el server nos haya enviado algo
        if (!bufferServer.empty()) {
            std::lock_guard<std::mutex> lck {mtxBufferServer};
            //mtxBufferServer.lock();
            ParseBufferServer();
            bufferServer.clear();            
            //mtxBufferServer.unlock();
        }
        for (unsigned i = 0; i < static_cast<unsigned>(gui::resType::resType_count); i++) {
            rw.draw(texts[i]);
        }
        rw.display();
    } // while isOpen

}