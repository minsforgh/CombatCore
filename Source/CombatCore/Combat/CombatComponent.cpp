#include "Combat/CombatComponent.h"
#include "Character/BaseCharacter.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UCombatComponent: OwnerCharacter cast failed"));
		SetActive(false);
		return;
	}

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	if (!Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("UCombatComponent: Mesh is null"));
		return;
	}

	if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &UCombatComponent::OnActiveMontageEnded);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCombatComponent: AnimInstace is NULL"));
	}
}

// BeginPlay에서 등록한 몽타주 콜백 해제 — 리스너 누수 방지
void UCombatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OwnerCharacter)
	{
		if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->OnMontageEnded.RemoveDynamic(this, &UCombatComponent::OnActiveMontageEnded);
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}

ECombatState UCombatComponent::GetState() const
{
	return StateMachine.GetState();
}

// 콤보 데이터만 초기화, 상태 전이 없음 — 피격 시 사용
void UCombatComponent::ResetComboData()
{
	CurrentComboIndex = 0;
	bIsInCancelWindow = false;
	ActiveCombatMontage = nullptr;
}

// 콤보 자연 종료 — 데이터 정리 + Idle 복귀
void UCombatComponent::EndCombo()
{
	ResetComboData();
	TryChangeState(ECombatState::Idle);
}

// 상태 전이를 여기서만 하면 델리게이트 브로드캐스트 누락 방지
bool UCombatComponent::TryChangeState(ECombatState NewState)
{
	const ECombatState OldState = StateMachine.GetState();

	if (!StateMachine.TryTransition(NewState, bIsInCancelWindow))
	{
		return false;
	}
	OnCombatStateChanged.Broadcast(OldState, NewState);
	return true;
}

// 현재 추적 중인 전투 몽타주가 끝났을 때만 콤보 종료
// 콤보 연계로 이미 다음 몽타주가 재생 중이면 불일치 -> 무시
void UCombatComponent::OnActiveMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage && Montage == ActiveCombatMontage.Get() && StateMachine.GetState() == ECombatState::Attacking)
	{
		EndCombo();
	}
}
