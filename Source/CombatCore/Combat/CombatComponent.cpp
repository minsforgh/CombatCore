#include "Combat/CombatComponent.h"
#include "ComboDataAsset.h"
#include "Combat/HitboxManager.h"
#include "InputBufferComponent.h"
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
	
	
	// 플레이어 전용 — 적 캐릭터에는 없으므로 nullptr 허용
	InputBufferComponent = OwnerCharacter->FindComponentByClass<UInputBufferComponent>();
	
	HitboxManager = OwnerCharacter->FindComponentByClass<UHitboxManager>();
	if (HitboxManager)
	{
		HitboxManager->OnHitDetected.AddDynamic(this, &UCombatComponent::HandleHitDetected);
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
	
	if (HitboxManager)
	{
		HitboxManager->OnHitDetected.RemoveDynamic(this, &UCombatComponent::HandleHitDetected);
	}

	Super::EndPlay(EndPlayReason);
}

ECombatState UCombatComponent::GetState() const
{
	return StateMachine.GetState();
}

// Idle이면 콤보 시작, Attacking 중이면 버퍼에 저장 후 소비 구간이면 즉시 진행
void UCombatComponent::HandleCombatInput(EInputType InputType)
{
	if (GetState() == ECombatState::Idle)
	{
		StartCombo(InputType);
	}
	else if (GetState() == ECombatState::Attacking)
	{
		if (InputBufferComponent.IsValid())
		{
			InputBufferComponent->BufferInput(InputType);
		}
		
		if (bComboAdvanceReady)
		{
			TryConsumeBufferedInput();
		}
	}
}

void UCombatComponent::StartCombo(EInputType InputType)
{	
	UComboDataAsset* ComboData = nullptr;
	if (InputType == EInputType::Light) ComboData = LightComboData;
	else if (InputType == EInputType::Heavy) ComboData = HeavyComboData;
	
	if (!ComboData)
	{
		UE_LOG(LogTemp, Error, TEXT("UCombatComponent: Invalid ComboData"));
		return;
	}

	const FComboStep* Step = ComboData->GetStep(0);
	if (!Step)
	{
		UE_LOG(LogTemp, Error, TEXT("UCombatComponent: ComboData has no Step 0"));
		return;
	}

	ActiveComboData = ComboData;
	
	if (!Step->Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UCombatComponent:MontageToPlay is NULL"));
		return;
	}
	
	if (!TryChangeState(ECombatState::Attacking))
	{
		return;
	}
	
	const float Duration = OwnerCharacter->PlayAnimMontage(Step->Montage);
	if (Duration <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayAnimMontage failed"));
		EndCombo();
		return;
	}
	ActiveCombatMontage = Step->Montage;
	CurrentComboIndex = 0;
}

void UCombatComponent::AdvanceCombo(EInputType InputType)
{	
	if (ActiveComboData == nullptr) return;

	const FComboStep* Step = ActiveComboData->GetStep(CurrentComboIndex);
	if (!Step) return;

	const int32* NextIndex = Step->BranchMap.Find(InputType);
	if (!NextIndex) return;

	const FComboStep* NextStep = ActiveComboData->GetStep(*NextIndex);
	if (!NextStep || !NextStep->Montage) return;

	const float Duration = OwnerCharacter->PlayAnimMontage(NextStep->Montage);
	if (Duration <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayAnimMontage failed"));
		EndCombo();
		return;
	}
	ActiveCombatMontage = NextStep->Montage;
	CurrentComboIndex = *NextIndex;
	bComboAdvanceReady = false;
}

// 버퍼에 유효한 입력이 있으면 소비하여 콤보 진행
void UCombatComponent::TryConsumeBufferedInput()
{
	if (!InputBufferComponent.IsValid()) return;

	EInputType ConsumedInputType;
	if (InputBufferComponent->ConsumeBuffer(ConsumedInputType))
	{
		AdvanceCombo(ConsumedInputType);
	}
	
}

void UCombatComponent::OnConsumeWindowEnter()
{
	bComboAdvanceReady = true;
	TryConsumeBufferedInput();
}

void UCombatComponent::OnConsumeWindowExit()
{
	bComboAdvanceReady = false;
}

void UCombatComponent::OnCancelWindowEnter()
{
	bIsInCancelWindow = true;
}

void UCombatComponent::OnCancelWindowExit()
{
	bIsInCancelWindow = false;
}

void UCombatComponent::OnComboResetNotify()
{
	if (GetState() == ECombatState::Attacking) EndCombo();
}

// 콤보 데이터만 초기화, 상태 전이 없음 — 피격 시 사용
void UCombatComponent::ResetComboData()
{
	CurrentComboIndex = 0;
	bIsInCancelWindow = false;
	bComboAdvanceReady = false;
	ActiveCombatMontage = nullptr;
	ActiveComboData = nullptr;
	
	if (InputBufferComponent.IsValid()) InputBufferComponent->ClearBuffer();
	
	if (HitboxManager) HitboxManager->StopDetection();
}

// 콤보 자연 종료 — 데이터 정리 + Idle 복귀
void UCombatComponent::EndCombo()
{
	ResetComboData();
	TryChangeState(ECombatState::Idle);
}

void UCombatComponent::HandleHitDetected(const FHitResult& HitResult, AActor* HitActor)
{
	if (!HitActor) return;
	UE_LOG(LogTemp, Log, TEXT("Hit detected: %s"), *HitActor->GetName());
}

// 모든 상태 전이는 이 함수를 통해야 한다 — Broadcast 경로를 단일화하여 누락 방지
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
