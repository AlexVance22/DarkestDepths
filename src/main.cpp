#include "pch.h"
#include "input.h"
#include "level.h"
#include "gui.h"


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
    window.setVerticalSyncEnabled(true);
    sf::Clock clock;
    sf::Event event;

    Input::init();

    Level level = Level();
    level.load_test_level();

    auto gui = Vec<Menu>::make();

    while (window.isOpen())
    {
        const f32 deltatime = clock.restart().asSeconds();

        while (window.pollEvent(event))
        {
            if (auto inner = gui.last_mut().if_let_mut()) {
                (*inner)->handle_event(event);
            } else {
                level.handle_event(event);
            }

            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            default:
                break;
            }
        }

        if (gui.is_empty()) {
            level.update(deltatime);
        }

        window.clear();

        level.render(window);

        window.display();
    }
}


