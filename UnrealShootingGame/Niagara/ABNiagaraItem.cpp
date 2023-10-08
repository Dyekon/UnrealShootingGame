// Fill out your copyright notice in the Description page of Project Settings.


#include "Niagara/ABNiagaraItem.h"
#include "Engine/AssetManager.h"

UABNiagaraItem::UABNiagaraItem()
{
	PotionEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Potion"));
	EnergeEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Energe"));
	PowerUpEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PowerUp"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> PotionEffectRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/sA_PickupSet_1/Fx/NiagaraSystems/NS_Pickup_3.NS_Pickup_3'"));
	if (PotionEffectRef.Object)
	{
		PotionEffect->SetAsset(PotionEffectRef.Object);
		PotionEffect->bAutoActivate = false;
		PotionEffect->Deactivate();
		PotionEffect->SetRelativeScale3D(FVector(1.3f, 1.3f, 1.3f));
	}
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> EnergeEffectRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/sA_PickupSet_1/Fx/NiagaraSystems/NS_Pickup_2.NS_Pickup_2'"));
	if (EnergeEffectRef.Object)
	{
		EnergeEffect->SetAsset(EnergeEffectRef.Object);
		EnergeEffect->bAutoActivate = false;
		EnergeEffect->Deactivate();
		EnergeEffect->SetRelativeScale3D(FVector(1.3f, 1.3f, 1.3f));
	}
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> PowerUpEffectRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/sA_PickupSet_1/Fx/NiagaraSystems/NS_Pickup_5.NS_Pickup_5'"));
	if (PowerUpEffectRef.Object)
	{
		PowerUpEffect->SetAsset(PowerUpEffectRef.Object);
		PowerUpEffect->bAutoActivate = false;
		PowerUpEffect->Deactivate();
		PowerUpEffect->SetRelativeScale3D(FVector(1.3f, 1.3f, 1.3f));
	}
}
