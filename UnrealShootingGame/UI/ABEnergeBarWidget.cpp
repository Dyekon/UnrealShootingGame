/*
* Writer : KimJunWoo
*
* This source code setup for player energe
*
* Last Update : 2023/10/12
*/


#include "UI/ABEnergeBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/ABCharacterWidgetInterface.h"

UABEnergeBarWidget::UABEnergeBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxEnerge = 0.0f;
}

void UABEnergeBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EnergeProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbEnergeBar")));
	ensure(EnergeProgressBar);

	IABCharacterWidgetInterface* CharacterWidget = Cast<IABCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void UABEnergeBarWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	MaxEnerge = (BaseStat + ModifierStat).MaxEnerge;

	if (EnergeProgressBar)
	{
		EnergeProgressBar->SetPercent(CurrentHp / MaxEnerge);
	}
}

void UABEnergeBarWidget::UpdateEnergeBar(float NewCurrentEnerge)
{
	CurrentHp = NewCurrentEnerge;

	ensure(MaxEnerge >= 0.0f);
	if (EnergeProgressBar)
	{
		EnergeProgressBar->SetPercent(NewCurrentEnerge / MaxEnerge);
	}
}