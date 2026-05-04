
#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyCharacter.h"
#include "RangedEnemyCharacter.generated.h"

class ABaseProjectile;

UCLASS()
class COMBATCORE_API ARangedEnemyCharacter : public AEnemyCharacter
{
	GENERATED_BODY()
	
public:
	void FireProjectile();
	
	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	float AimZOffset = 60.f;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	TSubclassOf<ABaseProjectile> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	FName MuzzleSocketName = TEXT("Muzzle");
};
