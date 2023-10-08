// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "ABNiagaraItem.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABNiagaraItem : public UNiagaraComponent
{
	GENERATED_BODY()
	
public:
	UABNiagaraItem();

	UPROPERTY(VisibleAnywhere, Category = Effect)
	TObjectPtr<UNiagaraComponent> PotionEffect;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	TObjectPtr<UNiagaraComponent> EnergeEffect;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	TObjectPtr<UNiagaraComponent> PowerUpEffect;
};
