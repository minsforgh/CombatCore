#include "Combat/CombatComponent.h"
#include "CombatAbility.h"
#include "ComboDataAsset.h"
#include "HealthComponent.h"
#include "Combat/HitboxManager.h"
#include "InputBufferComponent.h"
#include "Character/BaseCharacter.h"
#include "Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/EnemyAIController.h"


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

	HitboxManager = OwnerCharacter->GetHitboxManager();
	if (HitboxManager)
	{
		HitboxManager->OnHitDetected.AddDynamic(this, &UCombatComponent::HandleHitDetected);
	}

	HealthComponent = OwnerCharacter->GetHealthComponent();

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
		UE_LOG(LogTemp, Error, TEXT("UCombatComponent: AnimInstance is NULL"));
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

	GetWorld()->GetTimerManager().ClearTimer(HitStopTimerHandle);

	Super::EndPlay(EndPlayReason);
}

ECombatState UCombatComponent::GetState() const
{
	return StateMachine.GetState();
}

FVector UCombatComponent::GetAttackImpulseVelocity(float DeltaTime)
{
	if (!ActiveComboData.IsValid()) return FVector::ZeroVector;
	const FComboStep* Step = ActiveComboData->GetStep(CurrentComboIndex);
	if (!Step || !Step->AttackImpulseCurve || Step->AttackImpulseSpeed <= 0.f) return FVector::ZeroVector;
	
	AttackImpulseElapsedTime += DeltaTime;
	const float Duration = Step->Montage ? Step->Montage->GetPlayLength() : 1.f;
	const float Alpha = FMath::Clamp(AttackImpulseElapsedTime / Duration, 0.f, 1.f);
	const float SpeedMultiplier = Step->AttackImpulseCurve->GetFloatValue(Alpha);
	
	return CachedAttackForward * Step->AttackImpulseSpeed * SpeedMultiplier;
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
	else if (GetState() == ECombatState::Dodging)
	{
		StartCombo(InputType);
	}
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

	if (!TryChangeState(ECombatState::Attacking)) return;

	OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
	CachedAttackForward = OwnerCharacter->GetActorForwardVector();
	AttackImpulseElapsedTime = 0;
	
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
	CachedAttackForward = OwnerCharacter->GetActorForwardVector();
	AttackImpulseElapsedTime = 0.f;
	bComboAdvanceReady = false;
}

// 콤보 데이터만 초기화, 상태 전이 없음 — 피격 시 사용
void UCombatComponent::ResetComboData()
{
	CurrentComboIndex = 0;
	bIsInCancelWindow = false;
	bComboAdvanceReady = false;
	ActiveCombatMontage = nullptr;
	ActiveComboData = nullptr;
	
	CachedAttackForward = FVector::ZeroVector;
	AttackImpulseElapsedTime = 0.f;
	
	if (InputBufferComponent.IsValid()) InputBufferComponent->ClearBuffer();

	if (HitboxManager) HitboxManager->StopDetection();
}

// 콤보 자연 종료 — 데이터 정리 + Idle 복귀
void UCombatComponent::EndCombo()
{
	ResetComboData();
	TryChangeState(ECombatState::Idle);
}


// 공격자 위치 기준으로 피격자의 Forward/Right 내적 비교 → 4방향 판별
EHitDirection UCombatComponent::CalcHitDirection(const FVector& InstigatorLocation) const
{
	const FVector ToAttacker = (InstigatorLocation - OwnerCharacter->GetActorLocation()).GetSafeNormal2D();
	const FVector Forward = OwnerCharacter->GetActorForwardVector();
	const FVector Right = OwnerCharacter->GetActorRightVector();

	const float ForwardDot = FVector::DotProduct(ToAttacker, Forward);
	const float RightDot = FVector::DotProduct(ToAttacker, Right);

	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		return ForwardDot >= 0.f ? EHitDirection::Forward : EHitDirection::Back;
	}

	return RightDot >= 0.f ? EHitDirection::Right : EHitDirection::Left;
}

void UCombatComponent::PlayHitReaction(EHitDirection Direction)
{
	UAnimMontage* Montage = nullptr;
	switch (Direction)
	{
	case EHitDirection::Forward: Montage = HitReaction_Front;
		break;
	case EHitDirection::Back: Montage = HitReaction_Back;
		break;
	case EHitDirection::Left: Montage = HitReaction_Left;
		break;
	case EHitDirection::Right: Montage = HitReaction_Right;
		break;
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayHitReaction: Montage is NULL for direction %d"),
		       static_cast<uint8>(Direction));
		return;
	}

	const float Duration = OwnerCharacter->PlayAnimMontage(Montage);
	if (Duration > 0.f)
	{
		ActiveHitReactionMontage = Montage;
	}
}

UCombatAbility* UCombatComponent::GetOrCreateAbility(TSubclassOf<UCombatAbility> AbilityClass)
{
	if (TObjectPtr<UCombatAbility>* Found = AbilityInstances.Find(AbilityClass))
	{
		return Found->Get();
	}
	
	UCombatAbility* NewAbility = NewObject<UCombatAbility>(this, AbilityClass, NAME_None, RF_Transient);
	AbilityInstances.Add(AbilityClass, NewAbility);
	return NewAbility;
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

void UCombatComponent::OnInvincibleFrameBegin()
{
	bIsInvincible = true;
}

void UCombatComponent::OnInvincibleFrameEnd()
{
	bIsInvincible = false;
}

// 공격자 측 — FDamageInfo를 조립하여 피격자의 ReceiveDamage로 전달
void UCombatComponent::HandleHitDetected(const FHitResult& HitResult, AActor* HitActor)
{
	if (!HitActor) return;

	UCombatComponent* TargetCombat = HitActor->FindComponentByClass<UCombatComponent>();
	if (!TargetCombat) return;

	FDamageInfo DamageInfo;
	DamageInfo.Instigator = GetOwner();
	DamageInfo.HitResult = HitResult;

	if (ActiveComboData.IsValid())
	{
		const FComboStep* Step = ActiveComboData->GetStep(CurrentComboIndex);
		if (Step)
		{
			DamageInfo.Damage = Step->Damage;
			DamageInfo.KnockbackForce = Step->KnockbackForce;
			DamageInfo.HitStopDuration = Step->HitStopDuration;
			DamageInfo.AttackerShakeClass = Step->AttackerShakeClass;
			DamageInfo.VictimShakeClass = Step->VictimShakeClass;
		}
	}

	FVector Direction = (HitActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
	DamageInfo.KnockbackDirection = Direction;
	
	TargetCombat->ReceiveDamage(DamageInfo);

	// 공격자 히트스톱 + 셰이크 — 광역 히트 시 중첩 정책(longer wins)으로 1회분 수렴
	ApplyHitStop(DamageInfo.HitStopDuration);
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OwnerCharacter))
	{
		Player->PlayHitCameraShake(DamageInfo.AttackerShakeClass);
	}
}

// 피격자 측 — 데미지 적용 + 상태 전이 (HitStun/Dead)
// Attacking 중이면 콤보 데이터를 먼저 정리하고, 상태와 무관하게 몽타주 정지
void UCombatComponent::ReceiveDamage(const FDamageInfo& DamageInfo)
{
	if (GetState() == ECombatState::Dead) return;
	if (bIsInvincible) return;
	
	if (!HealthComponent) return;

	HealthComponent->ApplyDamage(DamageInfo);
	
	if (AEnemyAIController* EIC = Cast<AEnemyAIController>(OwnerCharacter->GetController()))
	{
		EIC->ReportDamageThreat(DamageInfo.Instigator.Get());
	}
	
	if (GetState() == ECombatState::Attacking)
	{
		ResetComboData();
	}

	OwnerCharacter->StopAnimMontage();

	if (HealthComponent->IsAlive())
	{
		if (TryChangeState(ECombatState::HitStun))
		{
			// Instigator가 소멸한 경우 자기 위치를 폴백 — 방향 계산 불가 시 Front로 귀결
			const FVector InstigatorLocation = DamageInfo.Instigator.IsValid()
				                                   ? DamageInfo.Instigator->GetActorLocation()
				                                   : GetOwner()->GetActorLocation();
			const EHitDirection Direction = CalcHitDirection(InstigatorLocation);
			PlayHitReaction(Direction);
			
			if (!DamageInfo.KnockbackDirection.IsNearlyZero())
			{
				OwnerCharacter->LaunchCharacter(
					DamageInfo.KnockbackDirection * DamageInfo.KnockbackForce,
					true, true);
			}
		}
	}
	else
	{
		TryChangeState(ECombatState::Dead);
		if (DeathMontage)
		{
			OwnerCharacter->PlayAnimMontage(DeathMontage);
		}
	}
	
	ApplyHitStop(DamageInfo.HitStopDuration);
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OwnerCharacter))
	{
		Player->PlayHitCameraShake(DamageInfo.VictimShakeClass);
	}
}

// CustomTimeDilation=0으로 액터를 일시 정지, WorldTimer로 복원
// 중첩 정책: 진행 중인 히트스톱보다 짧은 요청은 무시 (longer wins)
void UCombatComponent::ApplyHitStop(float Duration)
{
	if (Duration <= 0.f) return;
	Duration = FMath::Min(Duration, MaxHitStopDuration);

	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;

	// WorldTimer 사용 — Actor Timer는 CustomTimeDilation=0에서 멈춰 복원 불능
	FTimerManager& TM = GetWorld()->GetTimerManager();
	const float Remaining = TM.GetTimerRemaining(HitStopTimerHandle);
	if (Remaining >= Duration) return;

	TM.ClearTimer(HitStopTimerHandle);
	Owner->CustomTimeDilation = 0.f;

	TWeakObjectPtr<AActor> WeakOwner = Owner;
	TM.SetTimer(HitStopTimerHandle, [WeakOwner]()
	{
		if (AActor* O = WeakOwner.Get())
		{
			O->CustomTimeDilation = 1.f;
		}
	}, Duration, false);
}

void UCombatComponent::ExecuteAbility(TSubclassOf<UCombatAbility> AbilityClass)
{
	if (!AbilityClass) return;
	
	UCombatAbility* Ability = GetOrCreateAbility(AbilityClass);
	if (!Ability) return;
	
	if (!TryChangeState(Ability->GetRequiredState()))
	{
		return;
	}
	
	Ability->Activate(this);
}

void UCombatComponent::FinishAbility()
{
	TryChangeState(ECombatState::Idle);
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
	if (Montage && Montage == ActiveCombatMontage.Get() && !bInterrupted && StateMachine.GetState() == ECombatState::Attacking)
	{
		EndCombo();
	}
	// 몽타주 매칭 시 포인터를 항상 정리하되, 중단(interrupted)된 경우는 Idle 복귀하지 않음
	// — HitStun 중 재피격 시 StopAnimMontage로 기존 몽타주가 중단되므로, 이때 Idle로 돌아가면 안 됨
	else if (Montage && Montage == ActiveHitReactionMontage.Get())
	{
		ActiveHitReactionMontage = nullptr;
		if (!bInterrupted && StateMachine.GetState() == ECombatState::HitStun)
		{
			TryChangeState(ECombatState::Idle);
		}
	}
}
