/*
* Writer : KimJunWoo
*
* This source code notify animation end
*
* Last Update : 2024/03/06
*/


#include "Animation/AnimNotify_AttackAnimEnd.h"
#include "Interface/ABAnimationAttackInterface.h"

void UAnimNotify_AttackAnimEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	//Animation Montage내의 Notify로 설정하였을 때 실행시킬 공격 애니메이션 끝 함수 설정
	if (MeshComp)
	{
		IABAnimationAttackInterface* AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->AttackActionEnd();
		}
	}
}
