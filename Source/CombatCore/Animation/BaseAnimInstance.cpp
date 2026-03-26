
#include "Animation/BaseAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner());
	
	if (!Character) return;
	
	UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
	Speed = Character->GetVelocity().Size();
	bIsInAir =  CharacterMovement->IsFalling();
}
