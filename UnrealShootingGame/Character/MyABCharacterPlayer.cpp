// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyABCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ABCharacterControlData.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BluePrint/AIBlueprintHelperLibrary.h"
#include "UI/ABHUDWidget.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Gimmick/ABStageGimmick.h"
#include "Interface/ABGameInterface.h"

AMyABCharacterPlayer::AMyABCharacterPlayer()
{
	//UNavMovementComponent
	PrimaryActorTick.bCanEverTick = true;
	// Camera 기본값 설정
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1200.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//Tag 설정
	Tags.Add(FName("Player"));
	//CharacterTag = CharacterTagTable->FindRow<FGameplayTag>(TEXT("Character.Player"), TEXT(""));

	// Input 설정
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputChangeActionControlRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ChangeControl.IA_ChangeControl'"));
	if (nullptr != InputChangeActionControlRef.Object)
	{
		ChangeControlAction = InputChangeActionControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderMove.IA_ShoulderMove'"));
	if (nullptr != InputActionShoulderMoveRef.Object)
	{
		ShoulderMoveAction = InputActionShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook'"));
	if (nullptr != InputActionShoulderLookRef.Object)
	{
		ShoulderLookAction = InputActionShoulderLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionQuaterMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_QuaterMove.IA_QuaterMove'"));
	if (nullptr != InputActionQuaterMoveRef.Object)
	{
		QuaterMoveAction = InputActionQuaterMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionQuaterMouseRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_QuaterMouse.IA_QuaterMouse'"));
	if (nullptr != InputActionQuaterMouseRef.Object)
	{
		QuaterCharacterViewAction = InputActionQuaterMouseRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Attack.IA_Attack'"));
	if (nullptr != InputActionAttackRef.Object)
	{
		AttackAction = InputActionAttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSpecialAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_SpecialAttack.IA_SpecialAttack'"));
	if (nullptr != InputActionSpecialAttackRef.Object)
	{
		SpecialAttackAction = InputActionSpecialAttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> SpecialActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_SpecialAttack.AM_SpecialAttack'"));
	if (SpecialActionMontageRef.Object)
	{
		SpecialActionMontage = SpecialActionMontageRef.Object;
	}

	//UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetController()->GetWorld());
	//check(NavSystem);

	CurrentCharacterControlType = ECharacterControlType::Quater;
}

void AMyABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}
	FindMap();
	SetCharacterControl(CurrentCharacterControlType);
}

void AMyABCharacterPlayer::SetDead()
{
	Super::SetDead();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		DisableInput(PlayerController);

		IABGameInterface* ABGameMode = Cast<IABGameInterface>(GetWorld()->GetAuthGameMode());
		if (ABGameMode)
		{
			ABGameMode->OnPlayerDead();
		}
	}
}

void AMyABCharacterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
//Energe Section
	Stat->SetEnerge(Stat->GetCurrentEnerge() + (DeltaSeconds * 5));

//Mouse Section
	float MouseX, MouseY;

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	PlayerController->GetMousePosition(MouseX, MouseY);

	FVector3d PlayerLocation = GetActorLocation();
	FVector2d PlayerLocationInScreen;
	PlayerController->ProjectWorldLocationToScreen(PlayerLocation, PlayerLocationInScreen);

	FVector2d MouseLocation;
	MouseLocation.X = -(MouseY - PlayerLocationInScreen.Y);
	MouseLocation.Y = MouseX - PlayerLocationInScreen.X;

	CharacterAngle = FMath::RadiansToDegrees(FMath::Acos(MouseLocation.X / MouseLocation.Size()));

	if (MouseLocation.Y < 0)
	{
		CharacterAngle = 360 - CharacterAngle;
	}
	GetController()->SetControlRotation(FRotator(0, CharacterAngle, 0));
	BulletAngle = CharacterAngle;
}

void AMyABCharacterPlayer::FindMap()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AABStageGimmick::StaticClass(), OutActors);
	if (OutActors.Num() != 0)
	{
		for (AActor* Actor : OutActors)
		{
			UE_LOG(LogTemp, Log, TEXT("Actor Name : %s"), *Actor->GetName());
			Cast<AABStageGimmick>(Actor)->Player = this;
		}
	}
}

void AMyABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//CastCheck를 사용하여 원하는 클래스가 들어오도록 체크
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AMyABCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AMyABCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AMyABCharacterPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &AMyABCharacterPlayer::QuaterMove);
	EnhancedInputComponent->BindAction(QuaterCharacterViewAction, ETriggerEvent::Triggered, this, &AMyABCharacterPlayer::QuaterCharacterView);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMyABCharacterPlayer::Attack);
	EnhancedInputComponent->BindAction(SpecialAttackAction, ETriggerEvent::Triggered, this, &AMyABCharacterPlayer::ProcessSpecialCommand);
}

void AMyABCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterControlType == ECharacterControlType::Quater)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
	}
	else if (CurrentCharacterControlType == ECharacterControlType::Shoulder)
	{
		SetCharacterControl(ECharacterControlType::Quater);
		
	}
}

void AMyABCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UABCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());

	//기존에 적용되어 있던 매핑 컨텍스트를 지우고 새로운 매핑 컨텍스트를 입력함
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void AMyABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
	//NavMovement Acceleration 사용
	//GetCharacterMovement()->bRequestedMoveUseAcceleration = CharacterControlData->bUseAccelerationForPaths;
}

void AMyABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AMyABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);

}

void AMyABCharacterPlayer::QuaterMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	float InputSizeSquared = MovementVector.SquaredLength();
	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();
	if (MovementVectorSizeSquared > 1.0f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.0f;
	}
	else
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	//GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSize);
}

void AMyABCharacterPlayer::StopQuaterMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	float InputSizeSquared = MovementVector.SquaredLength();
	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();
	if (MovementVectorSizeSquared > 1.0f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.0f;
	}
	else
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
}

void AMyABCharacterPlayer::QuaterCharacterView(const FInputActionValue& Value)
{
	/*float MouseX, MouseY;

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	PlayerController->GetMousePosition(MouseX, MouseY);

	FVector3d PlayerLocation = GetActorLocation();
	FVector2d PlayerLocationInScreen;
	PlayerController->ProjectWorldLocationToScreen(PlayerLocation, PlayerLocationInScreen);

	FVector2d MouseLocation;
	MouseLocation.X = -(MouseY - PlayerLocationInScreen.Y);
	MouseLocation.Y = MouseX - PlayerLocationInScreen.X;

	CharacterAngle = FMath::RadiansToDegrees(FMath::Acos(MouseLocation.X / MouseLocation.Size()));

	if (MouseLocation.Y < 0)
	{
		CharacterAngle = 360 - CharacterAngle;
	}
	GetController()->SetControlRotation(FRotator(0, CharacterAngle, 0));
	BulletAngle = CharacterAngle;*/
	//UE_LOG(LogTemp, Log, TEXT("angle = %f"), CharacterAngle);
}

void AMyABCharacterPlayer::Attack()
{
	ProcessCombeCommand();
}

void AMyABCharacterPlayer::ProcessSpecialCommand()
{
	if (CurrentCombo == 0)
	{
		SpecialAttack();
		return;
	}
}

void AMyABCharacterPlayer::SpecialAttack()
{
	if (Stat->GetCurrentEnerge() - Stat->SkillCost >= 0)
	{
		Stat->SetEnerge(Stat->GetCurrentEnerge() - Stat->SkillCost);

		CurrentCombo = 1;

		const float AttackSpeedRate = Stat->GetTotalStat().AttackSpeed;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(SpecialActionMontage, AttackSpeedRate);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, SpecialActionMontage);

		bIsSpecialAttack = true;
		ComboTimerHandle.Invalidate();
		SetComboCheckTimer();
	}

}

void AMyABCharacterPlayer::SetupHUDWidget(UABHUDWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		InHUDWidget->UpdateStat(Stat->GetBaseStat(), Stat->GetModifierStat());
		InHUDWidget->UpdateHpBar(Stat->GetCurrentHp());
		InHUDWidget->UpdateEnergeBar(Stat->GetCurrentEnerge());

		Stat->OnStatChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateStat);
		Stat->OnHpChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateHpBar);
		Stat->OnEnergeChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateEnergeBar);
	}
}

