#pragma once
#include <iostream>
#include <vector>
#include "box2d/box2d.h"
#include "EnemyAIManager.h"
#include "GameObject.h"
#include "ScoreManager.h"
#include "BaseManager.h"
#include "WindowManager.h"
#include "CollisionListener.h"
#include "TPool.h"

class BaseManager;
struct ParallaxLayer
{
	std::vector<sf::Sprite> mSprites;
	float parallaxSpeed;
};

//------------------------------------------------------------------------------------------------------------------------

class GameManager
{
public:
	GameManager(WindowManager & windowManager);
	~GameManager();

	void EndGame();

	void Update(float deltaTime);

	void DebugUpdate(float deltaTime);

	void UpdateGameObjects(float deltaTime);

	void Render(float deltaTime);

	template <typename T>
	void AddManager();
	template <typename T, typename... Args>
	void AddManager(Args&&... args);

	template <typename T>
	T * GetManager()
	{
		auto it = std::find_if(mManagers.begin(), mManagers.end(),
			[](const auto & pair) { return pair.first == std::type_index(typeid(T)); });
		if (it != mManagers.end())
		{
			return dynamic_cast<T *>(it->second);
		}
		return nullptr;
	}

	BD::Handle CreateNewGameObject(ETeam team, BD::Handle parentHandle);

	GameObject * GetGameObject(BD::Handle handle);

	void RemoveGameObject(BD::Handle handle);

	GameObject * GetRootGameObject();
	BD::Handle GetRootGameObjectHandle();

	bool IsGameOver() const;

	b2World & GetPhysicsWorld();

	void SetPausedState(bool pause);

	sf::RenderWindow & GetWindow();

	std::vector<BD::Handle> GetGameObjectsByTeam(ETeam team);

	// Window
	WindowManager & mWindowManager;
	sf::Event mEvent;

private:

	void CleanUpDestroyedGameObjects(BD::Handle rootHandle);

	void RenderImGui();

	void InitWindow();

	void InitImGui();
	
	void GameOverScreen();

	std::vector<std::string> GetCommonResourcePaths();

	bool mShowImGuiWindow;
	std::vector<std::pair<std::type_index, BaseManager *>> mManagers;
	BD::Handle mRootHandle;
	TPool<GameObject> mPool;

	// Audio
	sf::SoundBuffer mSoundBuffer;
	sf::Sound mSound;
	bool mSoundPlayed;

	// GameOver
	bool mIsGameOver;
	sf::Text mGameOverText;
	sf::Text mScoreText;
	sf::Font mFont;

	// Box2d
	b2World mPhysicsWorld;
	CollisionListener mCollisionListener;

	std::vector<ParallaxLayer> mParallaxLayers;

	// Debug
	bool mPaused;

	sf::RenderWindow * mpWindow;
};