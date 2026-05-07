 #include "UI/PlayerHUDWidget.h"
  #include "Character/PlayerCharacter.h"
  #include "Combat/HealthComponent.h"
  #include "Combat/StaminaComponent.h"

  void UPlayerHUDWidget::BindToPlayer(APlayerCharacter* Player)
  {
        UnbindAll();

        if (!IsValid(Player)) return;

        if (UHealthComponent* Health = Player->GetHealthComponent())
        {
                BoundHealth = Health;
                Health->OnHealthChanged.AddDynamic(this, &UPlayerHUDWidget::HandleHealthChanged);
                Health->OnDeath.AddDynamic(this, &UPlayerHUDWidget::HandleDeath);
                OnHealthUpdated(Health->GetHealthPercent(), Health->GetCurrentHealth(), Health->GetMaxHealth());
        }

        if (UStaminaComponent* Stamina = Player->GetStaminaComponent())
        {
                BoundStamina = Stamina;
                Stamina->OnStaminaChanged.AddDynamic(this, &UPlayerHUDWidget::HandleStaminaChanged);
                OnStaminaUpdated(Stamina->GetStaminaPercent(), Stamina->GetCurrentStamina(), Stamina->GetMaxStamina());
        }
  }

  void UPlayerHUDWidget::NativeDestruct()
  {
        UnbindAll();
        Super::NativeDestruct();
  }

  void UPlayerHUDWidget::HandleHealthChanged(float Current, float Max, float Delta)
  {
        const float Percent = (Max > 0.f) ? (Current / Max) : 0.f;
        OnHealthUpdated(Percent, Current, Max);
  }

  void UPlayerHUDWidget::HandleDeath(const FDamageInfo& Info)
  {
        OnOwnerDied();
  }

  void UPlayerHUDWidget::HandleStaminaChanged(float Current, float Max, float Delta)
  {
        const float Percent = (Max > 0.f) ? (Current / Max) : 0.f;
        OnStaminaUpdated(Percent, Current, Max);
  }

  void UPlayerHUDWidget::UnbindAll()
  {
        if (UHealthComponent* Old = BoundHealth.Get())
        {
                Old->OnHealthChanged.RemoveDynamic(this, &UPlayerHUDWidget::HandleHealthChanged);
                Old->OnDeath.RemoveDynamic(this, &UPlayerHUDWidget::HandleDeath);
        }
        BoundHealth.Reset();

        if (UStaminaComponent* Old = BoundStamina.Get())
        {
                Old->OnStaminaChanged.RemoveDynamic(this, &UPlayerHUDWidget::HandleStaminaChanged);
        }
        BoundStamina.Reset();
  }
