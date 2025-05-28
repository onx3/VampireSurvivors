#pragma once
#include "BaseManager.h"

enum class EAbilityOptions
{
	// Must match in ToString function
	// Must match in GetAbilityDescription function
	SwordRange,
	Wand,
	Health,
	ExtraLife,
	ThrowingKnife,
	ExtraPickupRange,
	IncreaseDamageMult,
	BoomerangAxe,
	PhantomBlade,
    FirePot,
	Total
};

class AbilityUIManager : public BaseManager
{
public:
	AbilityUIManager(GameManager * pGameManager);
	~AbilityUIManager();

	void DrawAbilitySelectionUI();

private:
	void ApplySelectedAbility(EAbilityOptions ability);
	int GetAbilityCount(EAbilityOptions ability) const;

	EAbilityOptions mSelectedAbilityOption;
	std::unordered_map<EAbilityOptions, int> mAbilityUsageCounts;
};

