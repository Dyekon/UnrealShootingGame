/*
* Writer : KimJunWoo
*
* This source code notify when fire special bullet
*
* Last Update : 2024/03/06
*/


#include "AnimNotify_SpecialAttackFire.h"
#include "Interface/ABAnimationAttackInterface.h"

void UAnimNotify_SpecialAttackFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	//Animation Montage내의 Notify로 설정하였을 때 실행시킬 스킬 공격 함수 설정
	if (MeshComp)
	{
		IABAnimationAttackInterface* AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->SpecialAttackFire();
		}
	}
}