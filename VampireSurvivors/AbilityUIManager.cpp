#include "AstroidsPrivate.h"
#include "AbilityUIManager.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include "PlayerManager.h"
#include "SwordSlashComponent.h"
#include "WandComponent.h"
#include "HealthComponent.h"
#include <random>
#include "ThrowingKnife.h"
#include "PlayerStatsComponent.h"
#include "BoomerangComponent.h"
#include "PhantomBladeComponent.h"
#include "FirePotComponent.h"

namespace
{
    const char * ToString(EAbilityOptions option)
    {
        // Must match with EAbilityOptions in AbilityUIManager.h
        switch (option)
        {
            case EAbilityOptions::SwordRange:			return "Sword Range";
            case EAbilityOptions::Wand:					return "Wand";
            case EAbilityOptions::Health:				return "Health";
            case EAbilityOptions::ExtraLife:			return "Extra Life";
            case EAbilityOptions::ThrowingKnife:		return "ThrowingKnife";
            case EAbilityOptions::ExtraPickupRange:		return "Extra Pickup Range";
            case EAbilityOptions::IncreaseDamageMult:	return "Increase Damage";
            case EAbilityOptions::BoomerangAxe:			return "Boomerang Axe";
            case EAbilityOptions::PhantomBlade:			return "Phantom Blade";
            case EAbilityOptions::FirePot:			    return "Fire Pot";
            default:									return "Unknown";
        }
    }

    //------------------------------------------------------------------------------------------------------------------------

    const char * GetAbilityDescription(EAbilityOptions option)
    {
        // Must match with EAbilityOptions in AbilityUIManager.h
        switch (option)
        {
            case EAbilityOptions::SwordRange:			return "Increases the range of your sword attack.";
            case EAbilityOptions::Wand:					return "Adds a magical homing wand shot that scales with damage.";
            case EAbilityOptions::Health:				return "Restores a portion of your current health.";
            case EAbilityOptions::ExtraLife:			return "Gives you an extra life.";
            case EAbilityOptions::ThrowingKnife:		return "Throw a knife where you're aiming that pierces enemies.";
            case EAbilityOptions::ExtraPickupRange:		return "Increase range that you attrack coins by 10%.";
            case EAbilityOptions::IncreaseDamageMult:	return "Increase damage 10%.";
            case EAbilityOptions::BoomerangAxe:			return "Returns to origin after set distance.";
            case EAbilityOptions::PhantomBlade:			return "Sword that travels between enemies.";
            case EAbilityOptions::FirePot:			    return "Fire pot that falls from the sky.";
            default:									return "No description available.";
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

AbilityUIManager::AbilityUIManager(GameManager * pGameManager)
    : BaseManager(pGameManager)
    , mSelectedAbilityOption(EAbilityOptions::Total)
    , mAbilityUsageCounts()
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

    static std::vector<EAbilityOptions> selectedAbilities;
    static bool initialized = false;

    if (!initialized)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        std::vector<EAbilityOptions> allAbilities;
        for (int i = 0; i < int(EAbilityOptions::Total); ++i)
        {
            allAbilities.push_back(EAbilityOptions(i));
        }

        std::shuffle(allAbilities.begin(), allAbilities.end(), gen);
        selectedAbilities.assign(allAbilities.begin(), allAbilities.begin() + 3);

        initialized = true;
    }

    for (EAbilityOptions ability : selectedAbilities)
    {
        int count = GetAbilityCount(ability);
        std::string label = std::string(ToString(ability)) + " (" + std::to_string(count) + ")";

        if (ImGui::Button(label.c_str(), ImVec2(200, 40)))
        {
            ApplySelectedAbility(ability);
            gameState = EGameState::Running;
            initialized = false;
        }

        // Show tooltip on hover
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("%s", GetAbilityDescription(ability));
        }
    }

    ImGui::End();
}

//------------------------------------------------------------------------------------------------------------------------

void AbilityUIManager::ApplySelectedAbility(EAbilityOptions ability)
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

    int index = int(ability);
    if (index >= 0 && index < int(EAbilityOptions::Total))
    {
        EAbilityOptions ability = EAbilityOptions(index);
        ++mAbilityUsageCounts[ability];
    }

    switch (ability)
    {
        case (EAbilityOptions::SwordRange):
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
        case (EAbilityOptions::Wand):
        {
            mSelectedAbilityOption = EAbilityOptions::Wand;
            if (pPlayer)
            {
                auto pWandComponent = pPlayer->GetComponent<WandComponent>().lock();
                if (!pWandComponent)
                {
                    pWandComponent = std::make_shared<WandComponent>(pPlayer, gameManager);
                    pPlayer->AddComponent(pWandComponent);
                }
                else
                {
                    pWandComponent->AddDamage(100.f);
                }
            }
            break;
        }
        case (EAbilityOptions::Health):
        {
            mSelectedAbilityOption = EAbilityOptions::Health;
            if (pPlayer)
            {
                auto pHealthComponent = pPlayer->GetComponent<HealthComponent>().lock();
                if (pHealthComponent)
                {
                    pHealthComponent->AddHealth(20);
                }
            }
            break;
        }
        case (EAbilityOptions::ExtraLife):
        {
            mSelectedAbilityOption = EAbilityOptions::ExtraLife;
            if (pPlayer)
            {
                auto pHealthComponent = pPlayer->GetComponent<HealthComponent>().lock();
                if (pHealthComponent)
                {
                    pHealthComponent->IncreaseMaxLives(1);
                    pHealthComponent->AddLife(1);
                }
            }
            break;
        }
        case (EAbilityOptions::ThrowingKnife):
        {
            mSelectedAbilityOption = EAbilityOptions::ThrowingKnife;
            if (pPlayer)
            {
                auto pThowingKnifeComponent = pPlayer->GetComponent<ThrowingKnifeComponent>().lock();
                if (!pThowingKnifeComponent)
                {
                    pThowingKnifeComponent = std::make_shared<ThrowingKnifeComponent>(pPlayer, gameManager);
                    pPlayer->AddComponent(pThowingKnifeComponent);
                }
                else
                {
                    pThowingKnifeComponent->AddDamage(55.f);
                }
            }
            break;
        }
        case (EAbilityOptions::ExtraPickupRange):
        {
            mSelectedAbilityOption = EAbilityOptions::ExtraPickupRange;
            auto * pDropManager = GetGameManager().GetManager<DropManager>();
            if (pDropManager)
            {
                pDropManager->MultRadius(1.1f);
            }
            break;
        }
        case (EAbilityOptions::IncreaseDamageMult):
        {
            mSelectedAbilityOption = EAbilityOptions::IncreaseDamageMult;
            if (pPlayer)
            {
                auto pPlayerStatsComp = pPlayer->GetComponent<PlayerStatsComponent>().lock();
                if (pPlayerStatsComp)
                {
                    pPlayerStatsComp->AddAttackMult(.1f);
                }
            }
            break;
        }
        case (EAbilityOptions::BoomerangAxe):
        {
            mSelectedAbilityOption = EAbilityOptions::BoomerangAxe;
            if (pPlayer)
            {
                auto pBoomerang = pPlayer->GetComponent<BoomerangComponent>().lock();
                if (!pBoomerang)
                {
                    pBoomerang = std::make_shared<BoomerangComponent>(pPlayer, gameManager);
                    pPlayer->AddComponent(pBoomerang);
                }
                else
                {
                    pBoomerang->AddDamage(50.f);
                }
            }
            break;
        }
        case (EAbilityOptions::PhantomBlade):
        {
            mSelectedAbilityOption = EAbilityOptions::PhantomBlade;
            if (pPlayer)
            {
                auto pPhantomBlade = pPlayer->GetComponent<PhantomBladeComponent>().lock();
                if (!pPhantomBlade)
                {
                    pPhantomBlade = std::make_shared<PhantomBladeComponent>(pPlayer, gameManager);
                    pPlayer->AddComponent(pPhantomBlade);
                }
                else
                {
                    pPhantomBlade->AddDamage(50.f);
                }
            }
            break;
        }
        case (EAbilityOptions::FirePot):
        {
            mSelectedAbilityOption = EAbilityOptions::FirePot;
            if (pPlayer)
            {
                auto pFirePot = pPlayer->GetComponent<FirePotComponent>().lock();
                if (!pFirePot)
                {
                    pFirePot = std::make_shared<FirePotComponent>(pPlayer, gameManager);
                    pPlayer->AddComponent(pFirePot);
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

int AbilityUIManager::GetAbilityCount(EAbilityOptions ability) const
{
    auto it = mAbilityUsageCounts.find(ability);
    if (it != mAbilityUsageCounts.end())
    {
        return it->second;
    }
    return 0;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------