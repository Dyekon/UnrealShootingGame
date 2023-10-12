/*
* Writer : KimJunWoo
*
* This source code setup default values for player special bullet
*
* Last Update : 2023/10/12
*/

#include "Prop/ABSpecialBullet.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Character/ABCharacterBase.h"

// Sets default values
AABSpecialBullet::AABSpecialBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	BulletParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletParticle"));
	BulletHitParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletHitParticle"));
	TornadoParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TornadoParticle"));
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	BulletCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));

	RootComponent = BulletMesh;	//루트 오브젝트 설정
	BulletParticle->SetupAttachment(BulletMesh);
	BulletParticle->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	BulletHitParticle->SetupAttachment(BulletMesh);
	BulletHitParticle->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	TornadoParticle->SetupAttachment(BulletMesh);
	TornadoParticle->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.9f));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> BulletParticleRef(TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_thunderBall.P_ky_thunderBall'"));
	if (BulletParticleRef.Object)
	{
		BulletParticle->SetTemplate(BulletParticleRef.Object);
		BulletParticle->bAutoActivate = true;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BulletHitParticleRef(TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_ThunderBallHit.P_ky_ThunderBallHit'"));
	if (BulletHitParticleRef.Object)
	{
		BulletHitParticle->SetTemplate(BulletHitParticleRef.Object);
		BulletHitParticle->bAutoActivate = false;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> TornadoParticleRef(TEXT("/Script/Engine.ParticleSystem'/Game/FXVarietyPack/Particles/P_ky_darkStorm.P_ky_darkStorm'"));
	if (TornadoParticleRef.Object)
	{
		TornadoParticle->SetTemplate(TornadoParticleRef.Object);
		TornadoParticle->bAutoActivate = false;
	}

	BulletHitParticle->DeactivateSystem();
	TornadoParticle->DeactivateSystem();
	BulletCollision->InitCapsuleSize(25.0f, 25.0f);
	BulletMesh->SetGenerateOverlapEvents(true);
	BulletMesh->SetCollisionProfileName(TEXT("SpecialBullet"));

	Speed = 15;
	Angle = 0;
	LifeTime = 0;
	TornadoLifeTime = 0;
	bIsHit = false;
}

// Called when the game starts or when spawned
void AABSpecialBullet::BeginPlay()
{
	Super::BeginPlay();
}
void AABSpecialBullet::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor->Tags.Num() != 0 && Tags.Num() != 0)
	{
		if (!OtherActor->ActorHasTag(Tags[0]) && OtherActor->Tags.Num() == 1 && OtherActor->ActorHasTag(FName("Enemy")))
		{
			if (!bIsHit)
			{
				SetActorLocation(FVector(OtherActor->GetActorLocation().X, OtherActor->GetActorLocation().Y, 78.0f));
				FDamageEvent DamageEvent;
				OtherActor->TakeDamage(Damage, DamageEvent, Controller, Controller->GetPawn());
				BulletParticle->DeactivateSystem();
				BulletHitParticle->ActivateSystem(true);
				TornadoParticle->ActivateSystem(true);
				PrimaryActorTick.TickInterval = 0.25f;
				bIsHit = true;
			}
		}
	}
}

void AABSpecialBullet::NotifyActorEndOverlap(AActor* OtherActor)
{
}

void AABSpecialBullet::CheckEnemy()
{
	TArray<FHitResult> OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const float AttackRange = 180.0f;
	const float AttackRadius = 180.0f;
	const FVector Start = GetActorLocation();
	const FVector End = Start;

	bool HitDetected = GetWorld()->SweepMultiByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, 
														FCollisionShape::MakeSphere(AttackRadius), Params);
	if (HitDetected)
	{
		for (FHitResult Enemy : OutHitResult)
		{
			if (Enemy.GetActor()->Tags.Num() != 0 && Tags.Num() != 0)
			{
				if (!Enemy.GetActor()->ActorHasTag(Tags[0]) && Enemy.GetActor()->Tags.Num() == 1 && 
					Enemy.GetActor()->ActorHasTag(FName("Enemy")))
				{
					FDamageEvent DamageEvent;
					Enemy.GetActor()->TakeDamage(Damage / 2, DamageEvent, Controller, Controller->GetPawn());
				}
			}
		}	
	}

#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 0.25f);

#endif
}

void AABSpecialBullet::MoveActor()
{
	SetActorRotation(FRotator(0, Angle, 0));

	FVector CurrentLocation = GetActorLocation() + (GetActorForwardVector() * Speed);
	SetActorLocation(CurrentLocation);
	BulletCollision->SetRelativeLocation(CurrentLocation);
}

// Called every frame
void AABSpecialBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsHit)
	{
		++TornadoLifeTime;
		CheckEnemy();	
	}
	else
	{
		MoveActor();
	}
	LifeTime += DeltaTime;

	if (LifeTime >= 6 || TornadoLifeTime >= 10)
	{
		this->Destroy();
	}
}
