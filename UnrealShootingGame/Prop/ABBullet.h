// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABBullet.generated.h"

UCLASS()
class ARENABATTLE_API AABBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, Category = Mesh)
	class UStaticMeshComponent* BulletMesh;

	UPROPERTY(EditAnywhere, Category = Collision)
	class UCapsuleComponent* BulletCollision;
	void MoveActor();
	uint8 bIsHit : 1;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = Particle)
	TObjectPtr<class UParticleSystemComponent> BulletParticle;

	UPROPERTY(VisibleAnywhere, Category = Particle)
	TObjectPtr<class UParticleSystemComponent> BulletHitParticle;

	FString Owner;
	AController* Controller;
	float Angle;
	float Damage;
	float LifeTime;
	float Speed;
};
