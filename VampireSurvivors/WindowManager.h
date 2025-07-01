#pragma once

namespace BD
{
	constexpr float gsPixelCountCellSize = 16.f;
    constexpr float gsPi = 3.14159f;
    constexpr float gsPixelsPerMeter = 100.f;
};

class WindowManager
{
public:
	WindowManager();
	~WindowManager();

	void PollEvents();

	sf::RenderWindow * GetWindow();
	sf::Event GetEvent() const;

private:
	sf::RenderWindow * mpWindow;
	sf::Event mEvent;
};