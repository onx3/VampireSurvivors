#include "AstroidsPrivate.h"
#include "PlayerStatsComponent.h"
#include <imgui.h>

PlayerStatsComponent::PlayerStatsComponent(GameObject * pOwner, GameManager & gameManager)
	: GameComponent(pOwner, gameManager)
	, mAttackMult(1.f)
	, mAttackRange(1.f)
	, mName("PlayerStatsComponent")
{
}

//------------------------------------------------------------------------------------------------------------------------

PlayerStatsComponent::~PlayerStatsComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerStatsComponent::Update(float deltaTime)
{

}

//------------------------------------------------------------------------------------------------------------------------

void PlayerStatsComponent::DebugImGuiComponentInfo()
{
	ImGui::InputFloat("Current Damage per Slash", &mAttackMult);
	ImGui::InputFloat("Range Multiplier", &mAttackRange);
}

//------------------------------------------------------------------------------------------------------------------------

std::string & PlayerStatsComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerStatsComponent::AddAttackMult(float mult)
{
	mAttackMult += mult;
}

//------------------------------------------------------------------------------------------------------------------------

void PlayerStatsComponent::MultAttackRange(float rangeMult)
{
	mAttackRange *= rangeMult;
}

//------------------------------------------------------------------------------------------------------------------------

float PlayerStatsComponent::GetDamageMult() const
{
	return mAttackMult;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------