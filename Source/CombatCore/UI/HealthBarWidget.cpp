

#include "UI/HealthBarWidget.h"
#include "Combat/HealthComponent.h"

void UHealthBarWidget::BindToHealthComponent(UHealthComponent* InHealth)
{
	UnbindCurrent();
	
	if (!IsValid(InHealth)) return;
	
	BoundHealth = InHealth;
	InHealth->OnHealthChanged.AddDynamic(this, &UHealthBarWidget::HandleHealthChanged);
	InHealth->OnDeath.AddDynamic(this, &UHealthBarWidget::HandleDeath);
	
	OnHealthUpdated(InHealth->GetHealthPercent(),
					InHealth->GetCurrentHealth(),
					InHealth->GetMaxHealth());
}

void UHealthBarWidget::NativeDestruct()
{	
	UnbindCurrent();
	Super::NativeDestruct();
}

void UHealthBarWidget::HandleHealthChanged(float Current, float Max, float Delta)
{
	const float Percent = (Max > 0.f) ? (Current / Max) : 0.f;
	OnHealthUpdated(Percent, Current, Max);
}

void UHealthBarWidget::HandleDeath(const FDamageInfo& Info)
{
	OnOwnerDied();
}

void UHealthBarWidget::UnbindCurrent()
{
	if (UHealthComponent* Old =  BoundHealth.Get())
	{
		Old->OnHealthChanged.RemoveDynamic(this, &UHealthBarWidget::HandleHealthChanged);
		Old->OnDeath.RemoveDynamic(this, &UHealthBarWidget::HandleDeath);
	}
	BoundHealth.Reset();
}

