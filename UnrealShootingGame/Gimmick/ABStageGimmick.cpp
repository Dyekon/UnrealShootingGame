/*
* Writer : KimJunWoo
*
* This source code setup default values for map and set up spawn enemy
*
* Last Update : 2023/10/12
*/


#include "Gimmick/ABStageGimmick.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Physics/ABCollision.h"
#include "Character/ABCharacterNonPlayer.h"
#include "Item/ABItemBox.h"
#include "Interface/ABGameInterface.h"

// Sets default values
AABStageGimmick::AABStageGimmick()
{
	PrimaryActorTick.bCanEverTick = true;

	Stage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	RootComponent = Stage;

	Tags.Add(FName("Map"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Stages/Ground.Ground'"));
	if (StageMeshRef.Object)
	{
		Stage->SetStaticMesh(StageMeshRef.Object);
		Stage->SetRelativeScale3D(FVector(400.0, 400.0f, 1.0f));
	}

	//Fight Section
	OpponentSpawnTime = 5.0f;
	OpponentClass = AABCharacterNonPlayer::StaticClass();
	EnemyPoint = 100;

	//Stage Stat
	CurrentStageNum = 0;
	CurrentSpawnEnemyNum = 0;
	CurrentSpawnTime = 0;
	IsGameCleared = false;
	IsGameStarted = false;
}

void AABStageGimmick::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(true);
	IsGameStarted = true;
}

void AABStageGimmick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Players.IsEmpty() && !IsGameCleared && IsGameStarted)
	{
		CurrentSpawnTime += DeltaTime;
		//UE_LOG(LogTemp, Log, TEXT("SpawnTime : %f"), CurrentSpawnTime);
		if (CurrentSpawnTime >= OpponentSpawnTime)
		{
			//GetWorld()->GetTimerManager().SetTimer(OpponentTimerHandle, this, &AABStageGimmick::OnOpponentSpawn, OpponentSpawnTime, false);
			OnOpponentSpawn();
			CurrentSpawnTime = 0;
			++CurrentSpawnEnemyNum;
			if (CurrentSpawnEnemyNum >= 7 && CurrentStageNum < 9)
			{
				CurrentStageNum++;
				CurrentSpawnEnemyNum = 0;
				IABGameInterface* ABGameMode = Cast< IABGameInterface>(GetWorld()->GetAuthGameMode());
				if (ABGameMode)
				{
					ABGameMode->OnStageLevelChanged(CurrentStageNum);
				}
			}
		}
	}
	
}

void AABStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetState(EStageState::FIGHT);
}

void AABStageGimmick::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	check(OverlappedComponent->ComponentTags.Num() == 1);
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	check(Stage->DoesSocketExist(SocketName));

	FVector NewLocation = Stage->GetSocketLocation(SocketName);
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(GateTrigger), false, this);
	bool bResult = GetWorld()->OverlapMultiByObjectType(//범위에 맵이 있는지 확인
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams::InitType::AllStaticObjects,
		FCollisionShape::MakeSphere(775.0f),
		CollisionQueryParam
	);

	if (!bResult)
	{
		FTransform NewTransform(NewLocation);
		AABStageGimmick* NewGimmick = GetWorld()->SpawnActorDeferred<AABStageGimmick>(AABStageGimmick::StaticClass(), NewTransform);
		if (NewGimmick)
		{
			NewGimmick->SetStageNum(CurrentStageNum + 1);
			NewGimmick->FinishSpawning(NewTransform);
		}
	}
}

void AABStageGimmick::OpenAllGates()
{
	for (auto Gate : Gates)
	{
		(Gate.Value)->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void AABStageGimmick::CloseAllGates()
{
	for (auto Gate : Gates)
	{
		(Gate.Value)->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

void AABStageGimmick::SetState(EStageState InNewState)
{
	CurrentState = InNewState;

	if (StateChangeActions.Contains(InNewState))
	{
		StateChangeActions[CurrentState].StageDelegate.ExecuteIfBound();
	}
}
void AABStageGimmick::SetReady()
{
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	OpenAllGates();

	GetWorld()->GetTimerManager().SetTimer(OpponentTimerHandle, this, &AABStageGimmick::OnOpponentSpawn, OpponentSpawnTime, false);
}
void AABStageGimmick::SetFight()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	CloseAllGates();
}
void AABStageGimmick::SetChooseReward()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}

	CloseAllGates();
	SpawnRewardBoxes();
}
void AABStageGimmick::SetChooseNext()
{
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for (auto GateTrigger : GateTriggers)
	{
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	}

	OpenAllGates();
}

void AABStageGimmick::OnOpponentDestroyed(AActor* DestroyedActor)
{
	IABGameInterface* ABGameMode = Cast< IABGameInterface>(GetWorld()->GetAuthGameMode());
	if (ABGameMode)
	{
		ABGameMode->OnPlayerScoreChanged(EnemyPoint);
		if (ABGameMode->IsGameCleared())
		{
			IsGameCleared = true;
		}
	}
}

void AABStageGimmick::OnOpponentSpawn()
{
	for (TObjectPtr<class AActor> Player : Players)
	{
		//const FTransform SpawnTransform(Player->GetActorLocation() + FVector::UpVector * 88.0f);
		const FTransform SpawnTransform(Player->GetActorLocation() + FVector(FMath::FRandRange(-2000, 2000), FMath::FRandRange(-2000, 2000), 0));
		AABCharacterNonPlayer* ABOpponentCharacter = GetWorld()->SpawnActorDeferred<AABCharacterNonPlayer>(OpponentClass, SpawnTransform);
		if (ABOpponentCharacter)
		{
			ABOpponentCharacter->OnDestroyed.AddDynamic(this, &AABStageGimmick::OnOpponentDestroyed);
			ABOpponentCharacter->SetLevel(CurrentStageNum);
			ABOpponentCharacter->FinishSpawning(SpawnTransform);
		}
	}
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	for (const auto& RewardBox : RewardBoxes)
	{
		if (RewardBox.IsValid())
		{
			AABItemBox* ValidItemBox = RewardBox.Get();
			AActor* OverlappedBox = OverlappedComponent->GetOwner();
			if (OverlappedBox != ValidItemBox)
			{
				ValidItemBox->Destroy();
			}
		}
	}

	SetState(EStageState::NEXT);
}

void AABStageGimmick::SpawnRewardBoxes()
{
	for (const auto& RewardBoxLocation : RewardBoxLocations)
	{
		FTransform SpawnTransform(GetActorLocation() + RewardBoxLocation.Value + FVector(0.0f, 0.0f, 30.0f));
		AABItemBox* RewardBoxActor = GetWorld()->SpawnActorDeferred<AABItemBox>(RewardBoxClass, SpawnTransform);
		if (RewardBoxActor)
		{
			RewardBoxActor->Tags.Add(RewardBoxLocation.Key);
			RewardBoxActor->GetTrigger()->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnRewardTriggerBeginOverlap);
			RewardBoxes.Add(RewardBoxActor);
		}
	}

	for (const auto& RewardBox : RewardBoxes)
	{
		if (RewardBox.IsValid())
		{
			RewardBox.Get()->FinishSpawning(RewardBox.Get()->GetActorTransform());
		}
	}
}
