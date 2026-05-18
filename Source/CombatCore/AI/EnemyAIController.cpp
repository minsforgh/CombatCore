#include "AI/EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "DrawDebugHelpers.h"

AEnemyAIController::AEnemyAIController()
{
	UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SetPerceptionComponent(*PerceptionComp);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2500.f;
	SightConfig->LoseSightRadius = 2700.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.f;

	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AEnemyAIController::ReportDamageThreat(AActor* Attacker)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB || !Attacker) return;
	
	BB->SetValueAsObject(TEXT("TargetActor"), Attacker);
	BB->SetValueAsBool(TEXT("bCurrentlySeen"), true);
	BB->SetValueAsFloat(TEXT("LastSeenTime"), GetWorld()->GetTimeSeconds());
	BB->SetValueAsVector(TEXT("LastSeenLocation"), Attacker->GetActorLocation());
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
	if (UAIPerceptionComponent* PerceptionComp = GetPerceptionComponent())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	}
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if ENABLE_DRAW_DEBUG
	if (!bShowDebug) return;

	APawn* MyPawn = GetPawn();
	if (!MyPawn || !SightConfig) return;

	const FVector Loc = MyPawn->GetActorLocation();
	const FVector Forward = MyPawn->GetActorForwardVector();
	const float Radius = SightConfig->SightRadius;
	const float HalfAngle = SightConfig->PeripheralVisionAngleDegrees;

	constexpr int32 ArcSegments = 24;
	const float StepAngle = (HalfAngle * 2.f) / ArcSegments;

	for (int32 i = 0; i < ArcSegments; ++i)
	{
		const float A0 = -HalfAngle + StepAngle * i;
		const float A1 = -HalfAngle + StepAngle * (i + 1);
		FVector P0 = Loc + Forward.RotateAngleAxis(A0, FVector::UpVector) * Radius;
		FVector P1 = Loc + Forward.RotateAngleAxis(A1, FVector::UpVector) * Radius;
		DrawDebugLine(GetWorld(), P0, P1, FColor::Green, false, -1.f, 0, 2.f);
	}

	FVector LeftDir = Forward.RotateAngleAxis(-HalfAngle, FVector::UpVector);
	FVector RightDir = Forward.RotateAngleAxis(HalfAngle, FVector::UpVector);
	DrawDebugLine(GetWorld(), Loc, Loc + LeftDir * Radius, FColor::Green, false, -1.f, 0, 2.f);
	DrawDebugLine(GetWorld(), Loc, Loc + RightDir * Radius, FColor::Green, false, -1.f, 0, 2.f);
#endif
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || Actor != PC->GetPawn()) return;

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB) return;

	const float Now = GetWorld()->GetTimeSeconds();

	BB->SetValueAsVector(TEXT("LastSeenLocation"), Actor->GetActorLocation());
	BB->SetValueAsFloat(TEXT("LastSeenTime"), Now);

	if (Stimulus.WasSuccessfullySensed())
	{
		BB->SetValueAsObject(TEXT("TargetActor"), Actor);
		BB->SetValueAsBool(TEXT("bCurrentlySeen"), true);
	}
	else
	{
		BB->SetValueAsBool(TEXT("bCurrentlySeen"), false);
	}
}
