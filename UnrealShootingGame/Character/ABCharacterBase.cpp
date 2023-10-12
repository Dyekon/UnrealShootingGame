/*
* Writer : KimJunWoo
*
* This source code setup the default settings for the player or enemy.
*
* Last Update : 2023/10/12
*/


#include "Character/ABCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/ABCharacterMovementComponent.h"
#include "GameFramework/NavMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "ABCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "ABComboActionData.h" 
#include "Physics/ABCollision.h"
#include "Engine/DamageEvents.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "UI/ABWidgetComponent.h"
#include "UI/ABHpBarWidget.h"
#include "UI/ABEnergeBarWidget.h"
#include "Item/ABWeaponItemData.h"
#include "Prop/ABBullet.h"
#include "Prop/ABEnemyBullet.h"
#include "Prop/ABSpecialBullet.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Item/ABItemBox.h"


DEFINE_LOG_CATEGORY(LogABCharacter);

// Sets default values
AABCharacterBase::AABCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UABCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	//Pawn 설정
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Capsule 설정
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ABCAPSULE);

	// Movement 설정
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 1500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	//GetCharacterMovement()->MaxWalkSpeed = 450.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 100.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->bRequestedMoveUseAcceleration = true;
	

	// Mesh 설정
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("AICollision"));
	GetMesh()->SetGenerateOverlapEvents(true);

	//오브젝트 설정 /Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonWraith/Characters/Heroes/Wraith/Meshes/Wraith.Wraith'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}
	//애니메이션 설정 /Script/Engine.BlendSpace1D'/Game/ArenaBattle/Animation/BS_IdleWalkRun.BS_IdleWalkRun'
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	//CharacterControlManager에 알맞는 enum값과 데이터를 넣어줌
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
	if (ShoulderDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Shoulder, ShoulderDataRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuaterDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater'"));
	if (QuaterDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Quater, QuaterDataRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Attack.AM_Attack'"));
	if (ComboActionMontageRef.Object)
	{
		ComboActionMontage = ComboActionMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UABComboActionData> ComboActionDataRef(TEXT("/Script/ArenaBattle.ABComboActionData'/Game/ArenaBattle/CharaceterAction/ABA_Attack.ABA_Attack'"));
	if (ComboActionDataRef.Object)
	{
		ComboActionData = ComboActionDataRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> TagListRef(TEXT("/Script/Engine.DataTable'/Game/ArenaBattle/GameData/ABGameTagList.ABGameTagList'"));
	if (DeadMontageRef.Object)
	{
		CharacterTagTable = TagListRef.Object;
	}

	//Stat Component
	Stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));

	//Widget Component
	HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//Item Actions
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDeledate::CreateUObject(this, &AABCharacterBase::PowerUp)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDeledate::CreateUObject(this, &AABCharacterBase::DrinkPotion)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDeledate::CreateUObject(this, &AABCharacterBase::EnergeCharge)));

	//Weapon Component
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_l"));

	//Attack Component
	BulletClass = AABBullet::StaticClass();
	EnemyBulletClass = AABEnemyBullet::StaticClass();
	SpecialBulletClass = AABSpecialBullet::StaticClass();

	//Reward Component
	RewardBoxClass = AABItemBox::StaticClass();
}

void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);
	Stat->OnStatChanged.AddUObject(this, &AABCharacterBase::ApplyStat);
}

void AABCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	if (ActorHasTag(FName("Enemy")))
	{
		ApplyStat(Stat->GetBaseStat(), Stat->GetModifierStat());
	}
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;

}

void AABCharacterBase::ProcessCombeCommand()
{
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboCommand = false;
	}
	else
	{
		HasNextComboCommand = true;
	}
}

void AABCharacterBase::ComboActionBegin()
{
	//Combo Start
	CurrentCombo = 1;

	//Movement Setting
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None); //None일 시 이동기능이 없어짐

	//Animation Setting
	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);
	
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);

	bIsAttack = true;
	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();
}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensure(CurrentCombo != 0);
	CurrentCombo = 0;
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking); //이동기능 활성화
	bIsAttack = false;
	bIsSpecialAttack = false;

	NotifyComboActionEnd();
}

void AABCharacterBase::AttackActionEnd()
{
}

void AABCharacterBase::NotifyComboActionEnd()
{
}

void AABCharacterBase::SetComboCheckTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;
	//다음 연속 공격이 발동할 시간 체크
	float ComboEffectivetime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate) / AttackSpeedRate;
	if (ComboEffectivetime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck, ComboEffectivetime, false);
	}
}

void AABCharacterBase::ComboCheck()
{
	ComboTimerHandle.Invalidate();
	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);
		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo); //다음 콤보 이름 설정
		AnimInstance->Montage_JumpToSection(NextSection, ComboActionMontage);
		SetComboCheckTimer();
		HasNextComboCommand = false;
	}
}

void AABCharacterBase::FireBullet()
{
	const FTransform SpawnTransform(GetMesh()->GetSocketLocation("Muzzle_01"));
	if (ActorHasTag(FName("Enemy")))
	{
		AABEnemyBullet* ABEnemyBullet = GetWorld()->SpawnActorDeferred<AABEnemyBullet>(EnemyBulletClass, SpawnTransform);
		if (ABEnemyBullet)
		{
			BulletAngle = GetActorRotation().Yaw;
			ABEnemyBullet->Tags.Add(Tags[0]);
			ABEnemyBullet->Tags.Add(FName("Bullet"));
			ABEnemyBullet->Controller = GetController();
			ABEnemyBullet->Damage = Stat->GetTotalStat().Attack;
			ABEnemyBullet->Angle = BulletAngle;
			ABEnemyBullet->FinishSpawning(SpawnTransform);
		}
	}
	else
	{
		AABBullet* ABBullet = GetWorld()->SpawnActorDeferred<AABBullet>(BulletClass, SpawnTransform);
		if (ABBullet)
		{
			ABBullet->Tags.Add(Tags[0]);
			ABBullet->Tags.Add(FName("Bullet"));
			ABBullet->Controller = GetController();
			ABBullet->Damage = Stat->GetTotalStat().Attack;
			ABBullet->Angle = BulletAngle;
			ABBullet->FinishSpawning(SpawnTransform);
		}
	}

}

void AABCharacterBase::SpecialAttackFire()
{
	const FTransform SpawnTransform(GetMesh()->GetSocketLocation("Muzzle_03"));
	AABSpecialBullet* ABSpecialBullet = GetWorld()->SpawnActorDeferred<AABSpecialBullet>(SpecialBulletClass, SpawnTransform);

	if (ABSpecialBullet)
	{
		ABSpecialBullet->Tags.Add(Tags[0]);
		ABSpecialBullet->Tags.Add(FName("Bullet"));
		ABSpecialBullet->Controller = GetController();
		ABSpecialBullet->Damage = Stat->GetTotalStat().Attack * 2;
		ABSpecialBullet->Angle = BulletAngle;
		ABSpecialBullet->FinishSpawning(SpawnTransform);
	}
}

float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//EventInstigator : 나에게 피해를 입힌 가해자, DamageCauser : 가해자가 빙의한 무기, 폰, 액터정보 등 여러가지가 들어있음
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Stat->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void AABCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None); //캐릭터 움직임을 멈춤
	PlayDeadAnimation();
	SetActorEnableCollision(false); //원하는 액터의 콜리젼을 해제시킴
	HpBar->SetHiddenInGame(true); //WidgetComponent에 있는 함수로 게임 내의 위젯을 숨겨줌
	if (ActorHasTag(FName("Enemy")))
	{
		SpawnRewardBoxes();
	}
}

void AABCharacterBase::SpawnRewardBoxes()
{
	int32 RandomIndex = FMath::RandRange(0, 9);
	if (RandomIndex >= 5)
	{
		FTransform SpawnTransform(GetActorLocation());
		AABItemBox* RewardBoxActor = GetWorld()->SpawnActorDeferred<AABItemBox>(RewardBoxClass, SpawnTransform);
		if (RewardBoxActor)
		{
			RewardBoxActor->Tags.Add(FName("Reward"));
			RewardBoxActor->FinishSpawning(GetActorTransform());
		}
	}
}

void AABCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}

void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
	UABHpBarWidget* HpBarWidget = Cast<UABHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		HpBarWidget->UpdateStat(Stat->GetBaseStat(), Stat->GetModifierStat());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateHpBar);
		Stat->OnStatChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateStat);
	}
	UABEnergeBarWidget* EnergeBarWidget = Cast<UABEnergeBarWidget>(InUserWidget);
	if (EnergeBarWidget)
	{
		EnergeBarWidget->UpdateStat(Stat->GetBaseStat(), Stat->GetModifierStat());
		EnergeBarWidget->UpdateEnergeBar(Stat->GetCurrentEnerge());
		Stat->OnEnergeChanged.AddUObject(EnergeBarWidget, &UABEnergeBarWidget::UpdateEnergeBar);
		Stat->OnStatChanged.AddUObject(EnergeBarWidget, &UABEnergeBarWidget::UpdateStat);
	}
}

void AABCharacterBase::TakeItem(UABItemData* InItemData)
{
	if (InItemData)
	{
		TakeItemActions[(uint8)InItemData->Type].ItemDelegate.ExecuteIfBound(InItemData);
	}
}

void AABCharacterBase::DrinkPotion(UABItemData* InItemData)
{
	UE_LOG(LogABCharacter, Log, TEXT("Drink Potion"));
	float MaxHP = Stat->GetTotalStat().MaxHp;
	Stat->SetHp(Stat->GetCurrentHp() + (MaxHP / 4));
}

void AABCharacterBase::PowerUp(UABItemData* InItemData)
{
	UABWeaponItemData* PowerUpData = Cast<UABWeaponItemData>(InItemData);
	if (PowerUpData)
	{
		Stat->SetModifierStat(Stat->GetModifierStat() + PowerUpData->ModifierStat);
		Stat->SetHp(Stat->GetCurrentHp() + PowerUpData->ModifierStat.MaxHp);
	}
}

void AABCharacterBase::EnergeCharge(UABItemData* InItemData)
{
	UE_LOG(LogABCharacter, Log, TEXT("Energe Charge"));
	float MaxEnerge = Stat->GetTotalStat().MaxEnerge;
	Stat->SetEnerge(Stat->GetCurrentEnerge() + (MaxEnerge / 2));
}

int32 AABCharacterBase::GetLevel()
{
	return Stat->GetCurrenLevel();
}

void AABCharacterBase::SetLevel(int32 InNewLevel)
{
	Stat->SetLevelStat(InNewLevel);
}

void AABCharacterBase::ApplyStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	float MovementSpeed = (BaseStat + ModifierStat).MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

