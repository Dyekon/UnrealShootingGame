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
	//Animation Montage���� Notify�� �����Ͽ��� �� �����ų ���� �ִϸ��̼� �� �Լ� ����
	if (MeshComp)
	{
		IABAnimationAttackInterface* AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->AttackActionEnd();
		}
	}
}
