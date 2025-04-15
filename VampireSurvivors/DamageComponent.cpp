#include "AstroidsPrivate.h"
#include "DamageComponent.h"

DamageComponent::DamageComponent(GameObject * pOwner, GameManager & gameManager, float damage)
	: GameComponent(pOwner, gameManager)
	, mDamage(damage)
	, mName("DamageComponent")
{
}

//------------------------------------------------------------------------------------------------------------------------

DamageComponent::~DamageComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

void DamageComponent::Update(float deltaTime)
{
}

//------------------------------------------------------------------------------------------------------------------------

void DamageComponent::DebugImGuiComponentInfo()
{
}

//------------------------------------------------------------------------------------------------------------------------

std::string & DamageComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------

float DamageComponent::GetDamageAmount()
{
	return mDamage;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------