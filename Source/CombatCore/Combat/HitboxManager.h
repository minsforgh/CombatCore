#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitboxManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHitDetected, const FHitResult&, HitResult, AActor*, HitActor);
class UStaticMeshComponent;

// 무기 소켓 간 N개 보간점을 프레임마다 Sphere Sweep하여 히트 판정 (Ghost Trail 패턴)
// 이전 프레임 → 현재 프레임 궤적을 추적하므로 빠른 스윙에서도 관통을 방지한다
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATCORE_API UHitboxManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UHitboxManager();

	void StartDetection();
	void StopDetection();

	UPROPERTY(EditAnywhere, Category = "Hitbox")
	FName TraceStartSocket = FName("weapon_start");

	UPROPERTY(EditAnywhere, Category = "Hitbox")
	FName TraceEndSocket = FName("weapon_end");

	UPROPERTY(EditAnywhere, Category = "Hitbox")
	float TraceRadius = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Hitbox")
	int32 TracePointCount = 3;

	UPROPERTY(EditAnywhere, Category = "Hitbox")
	bool bDebugDraw = false;

	UPROPERTY(BlueprintAssignable, Category = "Hitbox")
	FOnHitDetected OnHitDetected;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	TSet<TWeakObjectPtr<AActor>> AlreadyHitActors;
	TArray<FVector> PreviousTracePoints;
	TArray<FVector> CurrentTracePoints;
	TArray<FHitResult> SweepHitResults;
	FCollisionQueryParams TraceParams;
};
