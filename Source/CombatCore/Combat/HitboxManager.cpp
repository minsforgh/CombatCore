#include "Combat/HitboxManager.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

UHitboxManager::UHitboxManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UHitboxManager::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);

	if (AActor* Owner = GetOwner())
	{
		WeaponMesh = Cast<UStaticMeshComponent>(Owner->GetDefaultSubobjectByName(TEXT("Weapon")));
	}

	TraceParams.AddIgnoredActor(GetOwner());
	TraceParams.bTraceComplex = false;
	TraceParams.bReturnPhysicalMaterial = false;

	CurrentTracePoints.SetNum(TracePointCount);
	PreviousTracePoints.SetNum(TracePointCount);
}


void UHitboxManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!WeaponMesh) return;

	// 1. 소켓 간 보간점 계산
	const FVector StartPos = WeaponMesh->GetSocketLocation(TraceStartSocket);
	const FVector EndPos = WeaponMesh->GetSocketLocation(TraceEndSocket);

	for (int32 i = 0; i < TracePointCount; i++)
	{
		const float Alpha = (TracePointCount > 1)
			                    ? static_cast<float>(i) / (TracePointCount - 1)
			                    : 0.5f;
		CurrentTracePoints[i] = FMath::Lerp(StartPos, EndPos, Alpha);
	}

	// 2. 이전 프레임 위치가 있으면 각 보간점을 Sweep — 첫 프레임은 스킵
	if (PreviousTracePoints.Num() > 0)
	{
		for (int32 i = 0; i < TracePointCount; i++)
		{
			SweepHitResults.Reset();
			GetWorld()->SweepMultiByChannel(
				SweepHitResults,
				PreviousTracePoints[i],
				CurrentTracePoints[i],
				FQuat::Identity,
				ECC_Pawn,
				FCollisionShape::MakeSphere(TraceRadius),
				TraceParams
			);

			for (const FHitResult& Hit : SweepHitResults)
			{
				AActor* HitActor = Hit.GetActor();
				if (!HitActor) continue;

				TWeakObjectPtr<AActor> WeakHit(HitActor);
				if (AlreadyHitActors.Contains(WeakHit)) continue;

				AlreadyHitActors.Add(WeakHit);
				OnHitDetected.Broadcast(Hit, HitActor);

				if (bDebugDraw)
				{
					DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.0f, FColor::Yellow, false, 0.5f);
				}
			}

			if (bDebugDraw)
			{
				DrawDebugLine(GetWorld(), PreviousTracePoints[i], CurrentTracePoints[i], FColor::Red, false, 0.5f);
				DrawDebugSphere(GetWorld(), CurrentTracePoints[i], TraceRadius, 8, FColor::Green, false, 0.5f);
			}
		}
	}

	// 3. 현재 → 이전으로 교체 (다음 프레임 Sweep용)
	Swap(PreviousTracePoints, CurrentTracePoints);
}


void UHitboxManager::StartDetection()
{
	if (IsComponentTickEnabled()) return;

	AlreadyHitActors.Reset();
	PreviousTracePoints.Reset();
	CurrentTracePoints.SetNum(TracePointCount);
	SetComponentTickEnabled(true);
}

void UHitboxManager::StopDetection()
{
	SetComponentTickEnabled(false);
	AlreadyHitActors.Reset();
	PreviousTracePoints.Reset();
}
