
#include "Character/EnemyCharacter.h"

#include "AI/EnemyAIController.h"
#include "Combat/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HealthBarWidget.h"

AEnemyCharacter::AEnemyCharacter()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidgetComponent->SetupAttachment(GetMesh());
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	HealthBarWidgetComponent->SetDrawAtDesiredSize(true);
	HealthBarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
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
	
	GetWorldTimerManager().SetTimer(
		VisibilityTimerHandle,
		this, &AEnemyCharacter::UpdateHealthBarVisibility,
		0.2f, true);
	
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{	
	GetWorldTimerManager().ClearTimer(VisibilityTimerHandle);
	
	if (UHealthComponent* HealthComp = GetHealthComponent())
	{
		HealthComp->OnDeath.RemoveDynamic(this, &AEnemyCharacter::HandleSelfDeath);
	}
	
	Super::EndPlay(EndPlayReason);
}

void AEnemyCharacter::HandleSelfDeath(const FDamageInfo& Info)
{
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(false);
	}
}

void AEnemyCharacter::UpdateHealthBarVisibility()
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
	const float Dist = FVector::Dist(GetActorLocation(), PCM->GetCameraLocation());
	const bool bVisible = bAlive && (Dist < HealthBarMaxVisibleDistance);

	if (HealthBarWidgetComponent->IsVisible() != bVisible)
	{
		HealthBarWidgetComponent->SetVisibility(bVisible);
	}
}
