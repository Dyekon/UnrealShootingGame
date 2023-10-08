// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "Interface/ABCharacterWidgetInterface.h"
#include "Interface/ABCharacterItemInterface.h"
#include "Engine/DataTable.h"
#include "GameData/ABCharacterStat.h"
#include "ABCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogABCharacter, Log, All);

UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quater
};

DECLARE_DELEGATE_OneParam(FOnTakeItemDeledate, class UABItemData* /*InItemData*/);
USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()
	FTakeItemDelegateWrapper() {}
	FTakeItemDelegateWrapper(const FOnTakeItemDeledate& InItemDelegate) : ItemDelegate(InItemDelegate) {}
	FOnTakeItemDeledate ItemDelegate;
};

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter, public IABAnimationAttackInterface, public IABCharacterWidgetInterface, public IABCharacterItemInterface
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AABCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	//protected 형식의 bUsetAccelerationForPaths 설정을 바꿔주기 위하여 직접 CharacterMovementComponent클래스를 만들어서 적용

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

protected:
	virtual void SetCharacterControlData(const class UABCharacterControlData* CharacterControlData);
	
	UPROPERTY(EditAnyWhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;

//Combo Action Section
protected:

	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = Attack, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABComboActionData> ComboActionData;

	void ProcessCombeCommand();
	void ComboActionBegin();

	virtual void AttackActionEnd() override;
	virtual void NotifyComboActionEnd();
	void SetComboCheckTimer();
	void ComboCheck();

	int32 CurrentCombo = 0;
	FTimerHandle ComboTimerHandle;
	bool HasNextComboCommand = false;

public:
	void ComboActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);
//Attack Section
protected:
	virtual void FireBullet() override;
	virtual void SpecialAttackFire() override;
	//Actor 설정에 TakeDamage함수가 들어있음
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsAttack : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsSpecialAttack : 1;
	
	UPROPERTY(EditAnywhere, Category = Bullet, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AABBullet> BulletClass;

	UPROPERTY(EditAnywhere, Category = Bullet, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AABEnemyBullet> EnemyBulletClass;

	UPROPERTY(EditAnywhere, Category = Bullet, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AABSpecialBullet> SpecialBulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	float BulletAngle;

//Dead Section
protected:
	UPROPERTY(EditAnywhere, BlueprintreadWrite, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

	virtual void SetDead();
	void PlayDeadAnimation();

	float DeadEventDelayTime = 5.0f;

//Stat Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABCharacterStatComponent> Stat;

//UI Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABWidgetComponent> HpBar;

	virtual void SetupCharacterWidget(class UABUserWidget* InUserWidget) override;

//Item Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Weapon;

	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions;

	virtual void TakeItem(class UABItemData* InItemData) override;
	virtual void DrinkPotion(class UABItemData* InItemData);
	virtual void PowerUp(class UABItemData* InItemData);
	virtual void EnergeCharge(class UABItemData* InItemData);

//Reward Section
protected:
	UPROPERTY(VisibleAnywhere, Category = Reward, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AABItemBox> RewardBoxClass;

	void SpawnRewardBoxes();

//Stat Section
public:
	int32 GetLevel();
	void SetLevel(int32 InNewLevel);
	void ApplyStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat);

//Tag Section
public:
	UPROPERTY()
	FString ActorTag;

	UPROPERTY()
	UDataTable* CharacterTagTable;
};
