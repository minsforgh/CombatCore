#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UCameraShakeBase;

UCLASS()
class COMBATCORE_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABaseProjectile();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> MovementComp;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAccess = true))
	float Speed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAccess = true))
	float LifeSpan = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Damage", meta = (AllowPrivateAccess = true))
	float Damage = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Damage", meta = (AllowPrivateAccess = true))
	float KnockbackForce = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Damage", meta = (AllowPrivateAccess = true))
	float HitStopDuration = 0.05f;


	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Damage", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> VictimShakeClass;

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	                     UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
