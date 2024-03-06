/*
* Writer : KimJunWoo
*
* This source code notify when fire bullet
*
* Last Update : 2024/03/06
*/

#include "Animation/AnimNotify_AttackHitCheck.h"
#include "Interface/ABAnimationAttackInterface.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	//Animation Montage���� Notify�� �����Ͽ��� �� �����ų �⺻ ���� �Լ� ����
	if (MeshComp)
	{
		IABAnimationAttackInterface* AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->FireBullet();
		}
	}
}
