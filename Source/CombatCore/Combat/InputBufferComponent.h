
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "InputBufferComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBATCORE_API UInputBufferComponent : public UActorComponent
{
	GENERATED_BODY()
	
	// 단일 슬롯 버퍼 — 마지막 입력만 유지하여 플레이어 최종 의도를 반영 (큐 방식은 연타 시 자동 진행 유발)
	EInputType BufferedInputType;

	// Tick 없이 조회 시점에 만료 판단 (lazy evaluation)
	double BufferExpiryTime = 0.0;

	// 시간 기반 만료 — 프레임 기반은 fps에 따라 체감이 달라짐
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float BufferDuration = 0.15f;

	bool bHasInput = false;
	
public:	
	
	UInputBufferComponent();

protected:
	
	virtual void BeginPlay() override;

public:	
	void BufferInput(EInputType InputType);
	
	bool ConsumeBuffer(EInputType& OutInputType);
	
	bool HasBufferedInput();
	
	void ClearBuffer();

		
};
