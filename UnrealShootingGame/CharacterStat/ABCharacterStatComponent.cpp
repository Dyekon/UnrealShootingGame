/*
* Writer : KimJunWoo
*
* This source code set up the default and modifier stat setting for characters
*
* Last Update : 2023/10/12
*/

#include "CharacterStat/ABCharacterStatComponent.h"
#include "GameData/ABGameSingleton.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	CurrentLevel = 1;
	AttackRadius = 250.0f;
	CurrentEnerge = 0;
	SkillCost = 50.0f;
	IsDead = false;

	bWantsInitializeComponent = true;
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetLevelStat(CurrentLevel);
	SetHp(BaseStat.MaxHp);
	SetEnerge(BaseStat.MaxEnerge);
}

void UABCharacterStatComponent::SetLevelStat(int32 InNewLevel)
{
	CurrentLevel = FMath::Clamp(InNewLevel, 1, UABGameSingleton::Get().CharacterMaxLevel);
	SetBaseStat(UABGameSingleton::Get().GetCharacterStat(CurrentLevel));
	check(BaseStat.MaxHp > 0.0f);
	check(BaseStat.MaxEnerge >= 0.0f);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= KINDA_SMALL_NUMBER && !IsDead)
	{
		OnHpZero.Broadcast();
		IsDead = true;
	}
	return 0.0f;
}

void UABCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, GetTotalStat().MaxHp);

	OnHpChanged.Broadcast(CurrentHp);
}

void UABCharacterStatComponent::SetEnerge(float NewEnerge)
{
	CurrentEnerge = FMath::Clamp<float>(NewEnerge, 0.0f, GetTotalStat().MaxEnerge);

	OnEnergeChanged.Broadcast(CurrentEnerge);
}

