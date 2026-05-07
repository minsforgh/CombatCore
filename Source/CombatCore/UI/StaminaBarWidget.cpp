
#include "UI/StaminaBarWidget.h"
#include "Combat/StaminaComponent.h"


void UStaminaBarWidget::BindToStaminaComponent(UStaminaComponent* InStamina)
{
	UnbindCurrent();
	
	if (!IsValid(InStamina)) return;
	
	BoundStamina = InStamina;
	InStamina->OnStaminaChanged.AddDynamic(this, &UStaminaBarWidget::HandleStaminaChanged);
	
	OnStaminaUpdated(InStamina->GetStaminaPercent(), InStamina->GetCurrentStamina(), InStamina->GetMaxStamina());
}

void UStaminaBarWidget::NativeDestruct()
{	
	UnbindCurrent();
	Super::NativeDestruct();
}

void UStaminaBarWidget::HandleStaminaChanged(float Current, float Max, float Delta)
{
	const float Percent = (Max > 0.f) ? (Current / Max) : 0.f;
	OnStaminaUpdated(Percent, Current, Max);
}

void UStaminaBarWidget::UnbindCurrent()
{
	if (UStaminaComponent* Old = BoundStamina.Get())
	{
		Old->OnStaminaChanged.RemoveDynamic(this, &UStaminaBarWidget::HandleStaminaChanged);
	}
	BoundStamina.Reset();
}
