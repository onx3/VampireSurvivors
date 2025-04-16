#include "AstroidsPrivate.h"
#include "AbilityUIManager.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include "PlayerManager.h"
#include "SwordSlashComponent.h"

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
	switch (index)
	{
		case 0:
		{
			mSelectedAbilityOption = EAbilityOptions::SwordRange;
			auto * pPlayerManager = gameManager.GetManager<PlayerManager>();
			if (pPlayerManager)
			{
				auto & players = pPlayerManager->GetPlayers();
				if (!pPlayerManager->GetPlayers().empty())
				{
					BD::Handle playerHandle = players[0];
					auto * pPlayer = gameManager.GetGameObject(playerHandle);
					if (pPlayer)
					{
						auto pSwordSlashComp = pPlayer->GetComponent<SwordSlashComponent>().lock();
						if (pSwordSlashComp)
						{
							pSwordSlashComp->SetRangeMultiplier(1.5f);
						}
					}
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