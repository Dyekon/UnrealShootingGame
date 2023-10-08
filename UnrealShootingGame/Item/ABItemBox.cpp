// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/ABCollision.h"
#include "Interface/ABCharacterItemInterface.h"
#include "Engine/AssetManager.h"
#include "ABItemData.h"


// Sets default values
AABItemBox::AABItemBox()
{
	Count = 0;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Mesh;
	Trigger->SetupAttachment(Mesh);	//Trigger밑으로 붙여줌

	Trigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	Trigger->SetBoxExtent(FVector(40.0f, 40.0f, 30.0f));

	/*static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1'"));
	if (BoxMeshRef.Object)
	{
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}*/
	//Mesh->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	NiagaraItem = CreateDefaultSubobject<UABNiagaraItem>(TEXT("NiagaraItem"));
	NiagaraItem->SetupAttachment(Mesh);
	NiagaraItem->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

void AABItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UAssetManager& Manager = UAssetManager::Get();

	TArray<FPrimaryAssetId> Assets;
	Manager.GetPrimaryAssetIdList(TEXT("ABItemData"), Assets);
	ensure(0 < Assets.Num());
	/*UE_LOG(LogTemp, Log, TEXT("Num : %d"), Assets.Num());
	for (int i = 0; i < Assets.Num(); i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Num : %s"), *Assets[i].PrimaryAssetName.ToString());
	}*/


	int32 RandomIndex = FMath::RandRange(0, Assets.Num() - 1);
	FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(Assets[RandomIndex]));
	if (AssetPtr.IsPending()) //로딩이 안됬을 경우
	{
		AssetPtr.LoadSynchronous(); //로딩 시켜줌
	}
	Item = Cast<UABItemData>(AssetPtr.Get());
	ensure(Item);
	switch (RandomIndex)
	{
		case 0:
			NiagaraItem->PotionEffect->Activate(true);
			NiagaraItem->PotionEffect->SetRelativeLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 78.0f));
			break;
		case 1:
			NiagaraItem->EnergeEffect->Activate(true);
			NiagaraItem->EnergeEffect->SetRelativeLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 78.0f));
			break;
		case 2:
			NiagaraItem->PowerUpEffect->Activate(true);
			NiagaraItem->PowerUpEffect->SetRelativeLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 78.0f));
			break;
	}
}

void AABItemBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor->Tags.Num() != 0 && Tags.Num() != 0 && Count == 0)
	{
		if (OtherActor->ActorHasTag(FName("Player")) && OtherActor->Tags.Num() == 1)
		{
			if (nullptr == Item)
			{

				Destroy();
				return;
			}

			IABCharacterItemInterface* OverlappingPawn = Cast<IABCharacterItemInterface>(OtherActor);
			if (OverlappingPawn)
			{
				OverlappingPawn->TakeItem(Item);
			}


			Mesh->SetHiddenInGame(true);
			SetActorEnableCollision(false);
			NiagaraItem->DestroyComponent();
			Destroy();
		}
	}
}
