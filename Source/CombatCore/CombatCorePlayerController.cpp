
#include "CombatCorePlayerController.h"
#include "Character/PlayerCharacter.h"
#include "UI/PlayerHUDWidget.h"


void ACombatCorePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (!IsLocalPlayerController())
	{
		return;
	}
	
	if (!PlayerHUDWidget && PlayerHUDClass)
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(this, PlayerHUDClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	if (PlayerHUDWidget)
	{
		if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(InPawn))
		{
			PlayerHUDWidget->BindToPlayer(PlayerChar);
		}
	}
}
