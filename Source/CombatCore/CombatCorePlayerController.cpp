
#include "CombatCorePlayerController.h"
#include "Character/BaseCharacter.h"
#include "Combat/HealthComponent.h"
#include "UI/HealthBarWidget.h"


void ACombatCorePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (!IsLocalPlayerController())
	{
		return;
	}
	
	if (!PlayerHUDWidget && PlayerHUDClass)
	{
		PlayerHUDWidget = CreateWidget<UHealthBarWidget>(this, PlayerHUDClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}
	
	if (PlayerHUDWidget)
	{
		if (ABaseCharacter* Char = Cast<ABaseCharacter>(InPawn))
		{
			PlayerHUDWidget->BindToHealthComponent(Char->GetHealthComponent());
		}
	}
}
