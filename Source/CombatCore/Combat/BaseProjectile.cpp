
#include "Combat/BaseProjectile.h"
#include "CombatComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/BaseCharacter.h"

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SetRootComponent(CollisionComp);
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComp->SetupAttachment(CollisionComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.f;
	
	CollisionComp->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnProjectileHit);
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(LifeSpan);
	
	if (GetInstigator())
	{
		CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);
	}
	
	MovementComp->Velocity = GetActorForwardVector() * Speed;
}

void ABaseProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetInstigator()) return;
	
	ABaseCharacter* Target = Cast<ABaseCharacter>(OtherActor);
	if (Target)
	{
		if (UCombatComponent* TargetCombat = Target->GetCombatComponent())
		{
			FDamageInfo Info;
			Info.Damage = Damage;
			Info.KnockbackForce = KnockbackForce;
			Info.KnockbackDirection = GetActorForwardVector();
			Info.Instigator = GetInstigator();
			Info.HitStopDuration = HitStopDuration;
			Info.HitResult = Hit;
			Info.VictimShakeClass = VictimShakeClass;
			
			TargetCombat->ReceiveDamage(Info);
		}
	}
	Destroy();
}


