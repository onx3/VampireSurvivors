#include "AstroidsPrivate.h"
#include "AbilityUIManager.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include "PlayerManager.h"
#include "SwordSlashComponent.h"
#include "WandComponent.h"

AbilityUIManager::AbilityUIManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
	, mSelectedAbilityOption(EAbilityOptions::None)
{

}

//------------------------------------------------------------------------------------------------------------------------

AbilityUIManager::~AbilityUIManager()
{
}

//------------------------------------------------------------------------------------------------------------------------

void AbilityUIManager::DrawAbilitySelectionUI(EGameState & gameState)
{
	ImGui::Begin("Choose Your Ability");

	const char * abilityNames[] = { "Sword Range", "Wand", "Health" };

	for (int i = 0; i < 3; ++i)
	{
		if (ImGui::Button(abilityNames[i], ImVec2(200, 40)))
		{
			ApplySelectedAbility(i);
			gameState = EGameState::Running;
		}
	}

	ImGui::End();
}

//------------------------------------------------------------------------------------------------------------------------

void AbilityUIManager::ApplySelectedAbility(int index)
{
	auto & gameManager = GetGameManager();
	auto * pPlayerManager = gameManager.GetManager<PlayerManager>();
	GameObject * pPlayer = nullptr;
	if (pPlayerManager)
	{
		auto & players = pPlayerManager->GetPlayers();
		if (!pPlayerManager->GetPlayers().empty())
		{
			BD::Handle playerHandle = players[0];
			pPlayer = gameManager.GetGameObject(playerHandle);
		}
	}

	switch (index)
	{
		case 0:
		{
			mSelectedAbilityOption = EAbilityOptions::SwordRange;
			if (pPlayer)
			{
				auto pSwordSlashComp = pPlayer->GetComponent<SwordSlashComponent>().lock();
				if (pSwordSlashComp)
				{
					pSwordSlashComp->SetRangeMultiplier(1.5f);
				}
			}
			break;
		}
		case (1):
		{
			mSelectedAbilityOption = EAbilityOptions::Wand;
			if (pPlayer)
			{
				auto pWandComponent = pPlayer->GetComponent<WandComponent>().lock();
				if (!pWandComponent)
				{
					auto pWandComponent = std::make_shared<WandComponent>(pPlayer, gameManager);
					pPlayer->AddComponent(pWandComponent);
				}
				else
				{
					pWandComponent->AddDamage(100.f);
				}
			}
			break;
		}
		default:
			break;
	}

}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------