/*
* Writer : KimJunWoo
*
* This source code setup default values for enemy bullet
*
* Last Update : 2023/10/12
*/

#include "Prop/ABEnemyBullet.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Character/ABCharacterBase.h"

// Sets default values
AABEnemyBullet::AABEnemyBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	BulletParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletParticle"));
	BulletHitParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletHitParticle"));
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	BulletCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));

	RootComponent = BulletMesh;	//루트 오브젝트 설정
	BulletParticle->SetupAttachment(BulletMesh);
	BulletParticle->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	BulletHitParticle->SetupAttachment(BulletMesh);
	BulletHitParticle->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> BulletParticleRef(TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_waterBall_2.P_ky_waterBall_2'"));
	if (BulletParticleRef.Object)
	{
		BulletParticle->SetTemplate(BulletParticleRef.Object);
		BulletParticle->bAutoActivate = true;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> BulletHitParticleRef(TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_waterBallHit_2.P_ky_waterBallHit_2'"));
	if (BulletHitParticleRef.Object)
	{
		BulletHitParticle->SetTemplate(BulletHitParticleRef.Object);
		BulletHitParticle->bAutoActivate = false;
	}

	BulletHitParticle->DeactivateSystem();
	BulletCollision->InitCapsuleSize(25.0f, 25.0f);
	BulletMesh->SetGenerateOverlapEvents(true);
	BulletMesh->SetCollisionProfileName(TEXT("Bullet"));
	//BulletMesh->SetSimulatePhysics(true);
	//bGenerateOverlapEventsDuringLevelStreaming = true;
	Speed = 10;
	Angle = 0;
	LifeTime = 0;
	bIsHit = false;

}

// Called when the game starts or when spawned
void AABEnemyBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void AABEnemyBullet::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor->Tags.Num() != 0 && Tags.Num() != 0)
	{
		if (!OtherActor->ActorHasTag(Tags[0]) && OtherActor->Tags.Num() == 1)
		{
			FDamageEvent DamageEvent;
			OtherActor->TakeDamage(Damage, DamageEvent, Controller, Controller->GetPawn());
			BulletParticle->DeactivateSystem();
			BulletHitParticle->ActivateSystem(true);
			bIsHit = true;
		}
	}
}

void AABEnemyBullet::NotifyActorEndOverlap(AActor* OtherActor)
{
}

void AABEnemyBullet::MoveActor()
{
	SetActorRotation(FRotator(0, Angle, 0));

	FVector CurrentLocation = GetActorLocation() + (GetActorForwardVector() * Speed);
	SetActorLocation(CurrentLocation);
	BulletCollision->SetRelativeLocation(CurrentLocation);
}

// Called every frame
void AABEnemyBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsHit)
	{
		MoveActor();
	}
	LifeTime += DeltaTime;

	if (LifeTime >= 3 || BulletHitParticle->bWasCompleted)
	{
		this->Destroy();
	}
}

