
#include "Combat/CombatAbility.h"
#include "Combat/CombatComponent.h"
#include "Character/BaseCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

// 몽타주 interrupt로 ANS_InvincibleFrame의 NotifyEnd 누락 시의 안전 장치
void UCombatAbility::End(UCombatComponent* Comp)
{
	if (Comp && Comp->IsInvincible())
	{
		Comp->OnInvincibleFrameEnd();
	}
}

// 바인드 + 재생, 중복 바인드 방지
bool UCombatAbility::PlayMontageAndTrack(UAnimMontage* Montage)
{
	if (!OwnerComponent || !Montage) return false;
	
	ABaseCharacter* Character = OwnerComponent->GetOwner<ABaseCharacter>();
	if (!Character) return false;
	
	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh) return false;
	
	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance) return false;
	
	if (!AnimInstance->OnMontageEnded.IsAlreadyBound(this, &UCombatAbility::HandleTrackedMontageEnded))
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &UCombatAbility::HandleTrackedMontageEnded);
	}
	
	TrackedMontage = Montage;
	
	const float Duration = Character->PlayAnimMontage(Montage);
	if (Duration <= 0.f)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UCombatAbility::HandleTrackedMontageEnded);
		TrackedMontage = nullptr;
		return false;
	}
	return true;
	
}

void UCombatAbility::HandleTrackedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{	
	// 필터 : 어빌리티 몽타주(TracktedMontage)의 종료로 인한 것인가
	if (Montage != TrackedMontage) return;
	
	if (OwnerComponent)
	{
		if (ABaseCharacter* Character = OwnerComponent->GetOwner<ABaseCharacter>())
		{
			if (USkeletalMeshComponent* Mesh = Character->GetMesh())
			{
				if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
				{
					AnimInstance->OnMontageEnded.RemoveDynamic(this, &UCombatAbility::HandleTrackedMontageEnded);
				}
			}
		}
	}
	
	TrackedMontage = nullptr;
	
	End(OwnerComponent);
	
	if (!bInterrupted && OwnerComponent)
	{
		if (const UWorld* World = OwnerComponent->GetWorld())
		{
			LastActivationTime = World->GetTimeSeconds();
		}
		OwnerComponent->FinishAbility();
	}
}

bool UCombatAbility::IsOnCooldown(float CurrentTime) const
{
	if (Cooldown <= 0.f) return false;
	return (CurrentTime - LastActivationTime) < Cooldown;
}

