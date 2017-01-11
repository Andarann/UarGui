
#include "main.hpp"
#include <vector>
#include <SFML/System/Clock.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include "mingw.thread.h"
#include "mingw.mutex.h"
#include <queue>

int main()
{
    /*sf::RenderWindow test(sf::VideoMode(800,640), "Hey");

    sf::Text shit;
    sf::Font fuck;
    sf::RenderTexture myTexture;
    shit.setCharacterSize(20);
    shit.setFont(fuck);
    myTexture.create(400,500);

    fuck.loadFromFile("FATPIXEL.TTF");

    std::stringstream myStrem;
    //std::string myString("Hello thisstrsssing");

    sf::Clock myTime;
    sf::Clock dummyTime;
    dummyTime.restart();

    while (1)
    {

    switch(static_cast<int>(dummyTime.getElapsedTime().asMicroseconds()) % 5)
    {
    case 0:
        myStrem << "Hi";
        break;
    case 1:
        myStrem << "Eh";
        break;
    case 2:
        myStrem << "Bo";
        break;
    case 3:
        myStrem << "NO";
        break;
    case 4:
        myStrem << "La";
        break;
    }


    shit.setString(myStrem.str());

    if (shit.getLocalBounds().width - shit.getLocalBounds().left > myTexture.getSize().x)
        shit.setPosition(-(shit.getLocalBounds().width - shit.getLocalBounds().left) + myTexture.getSize().x -2,0);
    else
        shit.setPosition(2,0);

    myTexture.clear(sf::Color(255,2,0));

    myTexture.draw(shit);

    myTexture.display();
    sf::Sprite mySprite(myTexture.getTexture());

    mySprite.setPosition(50,50);

        test.clear();

        test.draw(mySprite);

        test.display();

    myTime.restart();
    while (myTime.getElapsedTime().asSeconds() < 0.1);
    }*/
    gameLoop();
    return 0;
}
