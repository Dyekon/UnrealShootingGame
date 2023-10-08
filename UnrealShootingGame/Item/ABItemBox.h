// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Niagara/ABNiagaraItem.h"
#include "ABItemBox.generated.h"

UCLASS()
class ARENABATTLE_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItemBox();

	FORCEINLINE class UBoxComponent* GetTrigger() { return Trigger; }

protected:

	virtual void PostInitializeComponents() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UStaticMeshComponent> Mesh;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NiagaraItem, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABNiagaraItem> NiagaraItem;

	UPROPERTY(EditAnywhere, Category = Item)
	TObjectPtr<class UABItemData> Item;

	//UFUNCTION()
	//void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	int32 Count;
};
