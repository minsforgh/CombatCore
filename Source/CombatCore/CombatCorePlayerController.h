
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CombatCorePlayerController.generated.h"

class UHealthBarWidget;

UCLASS(abstract)
class ACombatCorePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UHealthBarWidget> PlayerHUDClass;
	
	UPROPERTY()
	TObjectPtr<UHealthBarWidget> PlayerHUDWidget;
	
	virtual void OnPossess(APawn* InPawn) override;

};
