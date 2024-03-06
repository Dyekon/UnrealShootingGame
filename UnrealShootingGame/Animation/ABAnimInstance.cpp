/*
* Writer : KimJunWoo
*
* This source code stores various information about the player to set animations.
*
* Last Update : 2024/03/06
*/


#include "Animation/ABAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/MyABCharacterPlayer.h"

UABAnimInstance::UABAnimInstance()
{
	MovingThreshould = 3.0f;
	JumpingThreshould = 100.0f;
}

void UABAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();

	}
	CharacterPlayer = Cast<AMyABCharacterPlayer>(GetOwningActor());
	if (CharacterPlayer)
	{
		Tag = CharacterPlayer->Tags[0];
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Velocity = Movement->Velocity;
		VelocityX = Velocity.X;
		VelocityY = Velocity.Y;
		if (CharacterPlayer)
		{
			CharacterMoveVelocity();
			bIsAttacking = CharacterPlayer->bIsAttack;
		}
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);
	}
}

void UABAnimInstance::CharacterMoveVelocity()
{
	//Setting Modifier Move Velocity
	//플레이어가 움직이는 방향에 따라 계산하는 각도 설정
	float CharacterAngle = CharacterPlayer->CharacterAngle;
	float MoveDirX = CharacterPlayer->MoveDirection.X;
	float MoveDirY = CharacterPlayer->MoveDirection.Y;

	if (CharacterAngle >= 0 && CharacterAngle <= 180)
	{
		VelocityX = Velocity.X > 0 ? 450 - (CharacterAngle * 5) : -450 + (CharacterAngle * 5);
		VelocityY = Velocity.Y > 0 ? 450 - (CharacterAngle * 5) : -450 + (CharacterAngle * 5);
	}
	else
	{
		VelocityX = Velocity.X > 0 ? -450 + (CharacterAngle - 180) * 5 : 450 - (CharacterAngle - 180) * 5;
		VelocityY = Velocity.Y > 0 ? -450 + ((CharacterAngle - 180) * 5) : 450 - ((CharacterAngle - 180) * 5);
	}
	if (CharacterAngle >= 0 && CharacterAngle < 90)
	{
		if (MoveDirX == 0)
		{
			VelocityX = Velocity.Y > 0 ? 0 + (CharacterAngle * 5) : 0 - (CharacterAngle * 5);
		}
		if (MoveDirY == 0)
		{
			VelocityY = Velocity.X > 0 ? 0 - (CharacterAngle * 5) : 0 + (CharacterAngle * 5);
		}
		if (MoveDirX != 0 && MoveDirY != 0)
		{
			if (MoveDirX > 0 && MoveDirY > 0)
			{
				VelocityX = 450;
				VelocityY = 450 - (CharacterAngle * 10);
			}
			else if (MoveDirX > 0 && MoveDirY < 0)
			{
				VelocityX = 450 - (CharacterAngle * 10);
				VelocityY = -450;
			}
			else if (MoveDirX < 0 && MoveDirY > 0)
			{
				VelocityX = -450 + (CharacterAngle * 10);
				VelocityY = 450;
			}
			else if (MoveDirX < 0 && MoveDirY < 0)
			{
				VelocityX = -450;
				VelocityY = -450 + (CharacterAngle * 10);
			}
		}
	}else if (CharacterAngle >= 90 && CharacterAngle < 270){
		if (MoveDirX == 0)
		{
			VelocityX = Velocity.Y > 0 ? 450 - ((CharacterAngle - 90) * 5) : -450 + ((CharacterAngle - 90) * 5);
		}
		if (MoveDirY == 0)
		{
			VelocityY = Velocity.X > 0 ? -450 + ((CharacterAngle - 90) * 5) : 450 - ((CharacterAngle - 90) * 5);
		}
		if (MoveDirX != 0 && MoveDirY != 0)
		{
			if (CharacterAngle >= 90 && CharacterAngle < 180)
			{
				if (MoveDirX > 0 && MoveDirY > 0)
				{
					VelocityX = 450 - ((CharacterAngle - 90) * 10);
					VelocityY = -450;
				}
				else if (MoveDirX > 0 && MoveDirY < 0)
				{
					VelocityX = -450;
					VelocityY = -450 + ((CharacterAngle - 90) * 10);
				}
				else if (MoveDirX < 0 && MoveDirY > 0)
				{
					VelocityX = 450;
					VelocityY = 450 - ((CharacterAngle - 90) * 10);
				}
				else if (MoveDirX < 0 && MoveDirY < 0)
				{
					VelocityX = -450 + ((CharacterAngle - 90) * 10);
					VelocityY = 450;
				}
			}else{
				if (MoveDirX > 0 && MoveDirY > 0)
				{
					VelocityX = -450;
					VelocityY = -450 + ((CharacterAngle - 180) * 10);
				}
				else if (MoveDirX > 0 && MoveDirY < 0)
				{
					VelocityX = -450 + ((CharacterAngle - 180) * 10);
					VelocityY = 450;
				}
				else if (MoveDirX < 0 && MoveDirY > 0)
				{
					VelocityX = 450 - ((CharacterAngle - 180) * 10);
					VelocityY = -450;
				}
				else if (MoveDirX < 0 && MoveDirY < 0)
				{
					VelocityX = 450;
					VelocityY = 450 - ((CharacterAngle - 180) * 10);
				}
			}
		}
	}else{
		if (MoveDirX == 0)
		{
			VelocityX = Velocity.Y > 0 ? -450 + ((CharacterAngle - 270) * 5) : 450 - ((CharacterAngle - 270) * 5);
		}
		if (MoveDirY == 0)
		{
			VelocityY = Velocity.X > 0 ? 450 - ((CharacterAngle - 270) * 5) : -450 + ((CharacterAngle - 270) * 5);
		}
		if (MoveDirX != 0 && MoveDirY != 0)
		{
			if (MoveDirX > 0 && MoveDirY > 0)
			{
				VelocityX = -450 + ((CharacterAngle - 270) * 10);
				VelocityY = 450;
			}
			else if (MoveDirX > 0 && MoveDirY < 0)
			{
				VelocityX = 450;
				VelocityY = 450 - ((CharacterAngle - 270) * 10);
			}
			else if (MoveDirX < 0 && MoveDirY > 0)
			{
				VelocityX = -450;
				VelocityY = -450 + ((CharacterAngle - 270) * 10);
			}
			else if (MoveDirX < 0 && MoveDirY < 0)
			{
				VelocityX = 450 - ((CharacterAngle - 270) * 10);
				VelocityY = -450;
			}
		}
	}
}
