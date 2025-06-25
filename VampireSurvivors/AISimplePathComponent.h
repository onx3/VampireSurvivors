#pragma once
#include "GameComponent.h"
class AISimplePathComponent : public GameComponent
{
public:
	AISimplePathComponent(GameObject * pOwner, GameManager & gameManager, BD::Handle followHandle);
	~AISimplePathComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual const std::string & GetClassName() override;

private:
	bool HasLineOfSight(const sf::Vector2f & from, const sf::Vector2f & to);
	std::vector<sf::Vector2f> FindPath(const sf::Vector2f & from, const sf::Vector2f & to);

	BD::Handle mFollowHandle;
	float mMoveSpeed;
	std::vector<sf::Vector2f> mPath;
	float mPathRecalcTimer;
	const float mPathRecalcInterval;
	std::string mName;
};

