#pragma once

#include "GameComponent.h"

class SpriteComponent : public GameComponent
{
public:
	SpriteComponent(GameObject * pOwner, GameManager & gameManager);
	~SpriteComponent();
	
	void SetSprite(std::shared_ptr<sf::Texture> pTexture, const sf::Vector2f & scale);
	sf::Sprite & GetSprite();

	sf::Vector2f GetPosition() const;
	void SetPosition(const sf::Vector2f & position);

	float GetWidth() const;
	float GetHeight() const;

	void Move(const sf::Vector2f & offset);
	void Move(float x, float y);

	void RotateClockwise();
	void RotateCounterClockwise();

	void SetRotation(float angle);
	float GetRotation() const;

	void SetOriginToCenter();
	sf::Vector2f GetOrigin();
	void SetOrigin(sf::Vector2f newOrigin);

	virtual void Update(float deltaTime) override;
	virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

private:
	sf::Texture mTexture;
	sf::Sprite mSprite;
	float mRotationSpeed;
	float mCurrentRotation;
	std::string mName;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------