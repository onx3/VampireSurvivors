#pragma once
#include "BaseManager.h"

enum class EAbilityOptions
{
	// All must match in my ToString function
	SwordRange,
	Wand,
	Health,
	ExtraLife,
	ThrowingKnife,
	Total
};

class AbilityUIManager : public BaseManager
{
public:
	AbilityUIManager(GameManager * pGameManager);
	~AbilityUIManager();

	void DrawAbilitySelectionUI(EGameState & gameState);

private:
	void ApplySelectedAbility(EAbilityOptions ability);
	int GetAbilityCount(EAbilityOptions ability) const;

	EAbilityOptions mSelectedAbilityOption;
	std::unordered_map<EAbilityOptions, int> mAbilityUsageCounts;
};

