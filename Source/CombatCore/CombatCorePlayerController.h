
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CombatCorePlayerController.generated.h"

class UPlayerHUDWidget;

UCLASS(abstract)
class ACombatCorePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDClass;
	
	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidget;
	
	virtual void OnPossess(APawn* InPawn) override;

};
