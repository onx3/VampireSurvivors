#pragma once
#include "BaseManager.h"

enum class EAbilityOptions
{
	SwordRange,
	Wand,
	Health,
	None
};

class AbilityUIManager : public BaseManager
{
public:
	AbilityUIManager(GameManager * pGameManager);
	~AbilityUIManager();

	void DrawAbilitySelectionUI(EGameState & gameState);

private:
	void ApplySelectedAbility(int index);

	EAbilityOptions mSelectedAbilityOption;

};

