#pragma once
#include <iostream>
#include <vector>
#include "box2d/box2d.h"
#include "EnemyAIManager.h"
#include "GameObject.h"
#include "UIManager.h"
#include "BaseManager.h"
#include "WindowManager.h"
#include "CollisionListener.h"
#include "TPool.h"

enum class EGameState
{
	Running,
	Paused,
	AbilitySelect,
	EndGame,
};

class BaseManager;

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
	void AddManager()
	{
		static_assert(std::is_base_of<BaseManager, T>::value, "T must inherit from BaseManager");
		if (std::none_of(mManagers.begin(), mManagers.end(),
			[](const auto & pair) { return pair.first == std::type_index(typeid(T)); }))
		{
			mManagers.emplace_back(std::type_index(typeid(T)), new T(this));
		}
	}

	//------------------------------------------------------------------------------------------------------------------------

	template <typename T, typename... Args>
	void AddManager(Args&&... args)
	{
		static_assert(std::is_base_of<BaseManager, T>::value, "T must inherit from BaseManager");
		if (std::none_of(mManagers.begin(), mManagers.end(),
			[](const auto & pair) { return pair.first == std::type_index(typeid(T)); }))
		{
			mManagers.emplace_back(std::type_index(typeid(T)), new T(this, std::forward<Args>(args)...));
		}
	}

	//------------------------------------------------------------------------------------------------------------------------

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
	void SetGameState(EGameState state);

	sf::RenderWindow & GetWindow();

	const sf::Font & GetFont() const;

	std::vector<BD::Handle> GetGameObjectsByTeam(ETeam team);

	// Window
	WindowManager & mWindowManager;
	sf::Event mEvent;

private:

	void CleanUpDestroyedGameObjects(BD::Handle rootHandle);

	void RenderGameObjectImGui();
	void RenderConCommands();
	void DrawPhysicsDebug(sf::RenderTarget & target);

	void InitWindow();

	void InitImGui();
	
	void GameOverScreen();

	bool mShowImGuiWindow;
	std::vector<std::pair<std::type_index, BaseManager *>> mManagers;
	BD::Handle mRootHandle;
	TPool<GameObject> mPool;

	// Audio
	sf::SoundBuffer mSoundBuffer;
	sf::Sound mSound;
	bool mSoundPlayed;

	EGameState mGameState;
	int mNextAbilityScoreThreshold;

	// GameOver
	sf::Text mGameOverText;
	sf::Text mRunTimeText;
	sf::Text mScoreText;
	sf::Font mFont;

	// Box2d
	b2World mPhysicsWorld;
	CollisionListener mCollisionListener;

	// Debug
	bool mPaused;

	sf::RenderWindow * mpWindow;
};