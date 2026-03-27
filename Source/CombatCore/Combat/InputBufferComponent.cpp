
#include "Combat/InputBufferComponent.h"

UInputBufferComponent::UInputBufferComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;


}

void UInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void UInputBufferComponent::BufferInput(EInputType InputType)
{
	BufferedInputType = InputType;
	BufferExpiryTime = GetWorld()->GetTimeSeconds() + BufferDuration;
	bHasInput = true;
}

bool UInputBufferComponent::ConsumeBuffer(EInputType& OutInputType)
{
	if (!bHasInput) return false;
	
	if (GetWorld()->GetTimeSeconds() > BufferExpiryTime)
	{
		ClearBuffer();
		return false;
	}
	
	OutInputType = BufferedInputType;
	ClearBuffer();
	return true;
}

bool UInputBufferComponent::HasBufferedInput()
{
	if (!bHasInput) return false;
	if (GetWorld()->GetTimeSeconds() > BufferExpiryTime)
	{
		ClearBuffer();
		return false;
	}
	return true;
}

void UInputBufferComponent::ClearBuffer()
{
	bHasInput = false;
	BufferExpiryTime = 0.0f;
}



