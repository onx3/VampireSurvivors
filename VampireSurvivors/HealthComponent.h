#pragma once
#include "GameComponent.h"
#include <functional>

class HealthComponent : public GameComponent
{
public:
	HealthComponent(GameObject * pOwner, GameManager & gameManager, float initialHealth, float maxHealth, int lifeCount, int maxLives, float hitCooldown = 0.f);

	float GetHealth() const;
	void AddHealth(float amount);
	void LoseHealth(float amount);

	int GetLives() const;
	void AddLife(int amount);
	void LoseLife();

	float GetMaxHealth() const;
	void AddMaxHealth(float amount);

	void SetDeathCallBack(std::function<void()> callback);
	void SetLifeLostCallback(std::function<void()> callback);

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

private:
	float mHealth;
	float mMaxHealth;
	int mLifeCount;
	int mMaxLives;
	float mHitCooldown;
	float mTimeSinceLastHit;
	std::string mName;
	
	std::function<void()> mLifeLostCallback;
	std::function<void()> mDeathCallback;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------