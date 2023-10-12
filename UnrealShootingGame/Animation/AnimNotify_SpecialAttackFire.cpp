/*
* Writer : KimJunWoo
*
* This source code notify when fire special bullet
*
* Last Update : 2023/10/12
*/


#include "AnimNotify_SpecialAttackFire.h"
#include "Interface/ABAnimationAttackInterface.h"

void UAnimNotify_SpecialAttackFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp)
	{
		IABAnimationAttackInterface* AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->SpecialAttackFire();
		}
	}
}