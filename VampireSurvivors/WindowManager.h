#pragma once

namespace BD
{
	static float gsPixelCountCellSize = 16.f;
	static float gsPi = 3.14159f;
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