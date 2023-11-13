#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "core/mod.h"
#include "level.h"


#ifdef NDEBUG
    #define VIDEO_MODE sf::VideoMode(1920, 1080)
    #define VIDEO_STYLE sf::Style::Fullscreen
#else
    #define VIDEO_MODE sf::VideoMode(1920, 980)
    #define VIDEO_STYLE sf::Style::Default
#endif


int main()
{
    sf::RenderWindow window(VIDEO_MODE, "Darkest Depths", VIDEO_STYLE);
    sf::Clock clock;
    sf::Event event;

    Level level = Level();
    level.load("res/testlevel.json");

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            default:
                break;
            }
        }

        window.clear(sf::Color::Magenta);

        level.render(window);

        window.display();
    }
}


