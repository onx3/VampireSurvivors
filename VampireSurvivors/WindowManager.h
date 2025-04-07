#pragma once

namespace BD
{
	static float gsPixelCountCellSize = 16.f;
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