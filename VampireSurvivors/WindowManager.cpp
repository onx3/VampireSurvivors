#include "AstroidsPrivate.h"
#include "WindowManager.h"
#include <cassert>
#include <imgui-SFML.h>
#include <imgui.h>

WindowManager::WindowManager()
	: mEvent()
{
    mpWindow = new sf::RenderWindow(sf::VideoMode(1920, 1088), "Astroids", sf::Style::Default);
    mpWindow->setFramerateLimit(300);

    mpWindow->setMouseCursorVisible(false);

    ImGui::CreateContext();
    ImGuiIO & io = ImGui::GetIO();
    ImGui::SFML::Init(*mpWindow);
}

//------------------------------------------------------------------------------------------------------------------------

WindowManager::~WindowManager()
{
    ImGui::SFML::Shutdown();
    delete mpWindow;
    mpWindow = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------

void WindowManager::PollEvents()
{
    ImGui::SFML::ProcessEvent(mEvent);
    while (mpWindow->pollEvent(mEvent))
    {
        switch (mEvent.type)
        {
            case sf::Event::Closed:
            {
                mpWindow->close();
                break;
            }
            case sf::Event::KeyPressed:
            {
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

sf::RenderWindow * WindowManager::GetWindow()
{
	return mpWindow;
}

//------------------------------------------------------------------------------------------------------------------------

sf::Event WindowManager::GetEvent() const
{
    return mEvent;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------