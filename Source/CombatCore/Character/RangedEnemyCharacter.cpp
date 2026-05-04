
#include "Character/RangedEnemyCharacter.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Combat/BaseProjectile.h"
#include "Components/SkeletalMeshComponent.h"

void ARangedEnemyCharacter::FireProjectile() 
{
	if (!ProjectileClass) return;
	
	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (!SkelMesh) return;
	
	const FVector SpawnLoc = SkelMesh->GetSocketLocation(MuzzleSocketName);
	FRotator SpawnRot = GetActorRotation();
	
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
			if (Target)
			{	
				FVector TargetLoc = Target->GetActorLocation();
				TargetLoc.Z += AimZOffset;
				FVector Direction = TargetLoc - SpawnLoc;
				SpawnRot = Direction.Rotation();
			}
		}
	}
	
	FActorSpawnParameters Params;
	Params.Instigator = this;
	Params.Owner = this;
	
	GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params);
}
