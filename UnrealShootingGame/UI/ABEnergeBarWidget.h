// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ABUserWidget.h"
#include "GameData/ABCharacterStat.h"
#include "ABEnergeBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABEnergeBarWidget : public UABUserWidget
{
	GENERATED_BODY()
public:
	UABEnergeBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat);
	void UpdateEnergeBar(float NewCurrentEnerge);

protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> EnergeProgressBar;

	UPROPERTY()
	float CurrentHp;

	UPROPERTY()
	float MaxEnerge;
};
