/*
* Writer : KimJunWoo
*
* This source code setup default values for player bullet
*
* Last Update : 2023/10/12
*/


#include "Prop/ABBullet.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Character/ABCharacterBase.h"

// Sets default values
AABBullet::AABBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	BulletParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletParticle"));
	BulletHitParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletHitParticle"));
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	BulletCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));

	RootComponent = BulletMesh;	//루트 오브젝트 설정
	BulletParticle->SetupAttachment(BulletMesh);
	BulletParticle->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));
	BulletHitParticle->SetupAttachment(BulletMesh);
	BulletHitParticle->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> BulletParticleRef(TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_waterBall.P_ky_waterBall'"));
	if (BulletParticleRef.Object)
	{
		BulletParticle->SetTemplate(BulletParticleRef.Object);
		BulletParticle->bAutoActivate = true;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> BulletHitParticleRef(TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_waterBallHit.P_ky_waterBallHit'"));
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
	Speed = 45;
	Angle = 0;
	LifeTime = 0;
	bIsHit = false;
}

// Called when the game starts or when spawned
void AABBullet::BeginPlay()
{
	Super::BeginPlay();	
}

void AABBullet::NotifyActorBeginOverlap(AActor* OtherActor)
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

void AABBullet::NotifyActorEndOverlap(AActor* OtherActor)
{
	//UE_LOG(LogTemp, Log, TEXT("NotifyOverlapOut"));
}

void AABBullet::MoveActor()
{
	SetActorRotation(FRotator(0, Angle, 0));

	FVector CurrentLocation = GetActorLocation() + (GetActorForwardVector() * Speed);
	SetActorLocation(CurrentLocation);
	BulletCollision->SetRelativeLocation(CurrentLocation);
}

// Called every frame
void AABBullet::Tick(float DeltaTime)
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

