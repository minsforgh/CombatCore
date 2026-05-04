
#include "Character/EnemyCharacter.h"
#include "AI/EnemyAIController.h"
#include "Combat/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HealthBarWidget.h"
#include "Components/CapsuleComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidgetComponent->SetupAttachment(GetMesh());
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	HealthBarWidgetComponent->SetDrawAtDesiredSize(true);
	HealthBarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	HealthBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthBarWidgetComponent->SetGenerateOverlapEvents(false);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UHealthBarWidget* Widget =  Cast<UHealthBarWidget>(HealthBarWidgetComponent->GetUserWidgetObject()))
	{
		if (UHealthComponent* HealthComp = GetHealthComponent())
		{
			Widget->BindToHealthComponent(HealthComp);
			HealthComp->OnDeath.AddDynamic(this, &AEnemyCharacter::HandleSelfDeath);
		}
	}
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UHealthComponent* HealthComp = GetHealthComponent())
	{
		HealthComp->OnDeath.RemoveDynamic(this, &AEnemyCharacter::HandleSelfDeath);
	}

	Super::EndPlay(EndPlayReason);
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateHealthBar();
}

void AEnemyCharacter::HandleSelfDeath(const FDamageInfo& Info)
{
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(false);
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->StopMovementImmediately();
	DetachFromControllerPendingDestroy();
	SetLifeSpan(DeathLifeSpan);
	
}

void AEnemyCharacter::UpdateHealthBar()
{
	if (!HealthBarWidgetComponent)
	{
		return;
	}

	APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!PCM)
	{
		return;
	}

	UHealthComponent* HealthComp = GetHealthComponent();
	const bool bAlive = HealthComp && HealthComp->IsAlive();
	const FVector CameraLocation = PCM->GetCameraLocation();
	const float Dist = FVector::Dist(GetActorLocation(), CameraLocation);
	const bool bVisible = bAlive && (Dist < HealthBarMaxVisibleDistance);

	if (HealthBarWidgetComponent->IsVisible() != bVisible)
	{
		HealthBarWidgetComponent->SetVisibility(bVisible);
	}

	if (bVisible)
	{
		const FVector WidgetLocation = HealthBarWidgetComponent->GetComponentLocation();
		const FVector LookDir = (CameraLocation - WidgetLocation).GetSafeNormal();
		if (!LookDir.IsNearlyZero())
		{
			HealthBarWidgetComponent->SetWorldRotation(LookDir.Rotation());
		}
	}
}
