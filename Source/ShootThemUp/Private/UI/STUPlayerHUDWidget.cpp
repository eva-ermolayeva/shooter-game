// Shooter Game. All Rights, If Any, Reserved.

#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/ProgressBar.h"
#include "Player/STUPlayerState.h"
#include "STUUtils.h"

void USTUPlayerHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (GetOwningPlayer())
    {
        GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &USTUPlayerHUDWidget::OnNewPawn);

        // Initialize will be called after OnPossess, so OnNewPawn won't be called first time
        OnNewPawn(GetOwningPlayerPawn());
    }
}

float USTUPlayerHUDWidget::GetHealthPercent() const
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    if (!HealthComponent) return 0.0f;

    return HealthComponent->GetHealthPercent();
}

bool USTUPlayerHUDWidget::IsPlayerAlive() const
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    return HealthComponent && !HealthComponent->IsDead();
}

bool USTUPlayerHUDWidget::IsPlayerSpectating() const
{
    const auto Controller = GetOwningPlayer();
    return Controller && Controller->GetStateName() == NAME_Spectating;
}

bool USTUPlayerHUDWidget::GetWeaponUIData(FWeaponUIData& WeaponUIData) const
{
    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    if (!WeaponComponent) return false;

    return WeaponComponent->GetCurrentWeaponUIData(WeaponUIData);
}

bool USTUPlayerHUDWidget::GetAmmoData(FAmmoData& AmmoData) const
{
    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(GetOwningPlayerPawn());
    if (!WeaponComponent) return false;

    return WeaponComponent->GetCurrentAmmoData(AmmoData);
}

int32 USTUPlayerHUDWidget::GetKillsNum() const
{
    const auto Controller = GetOwningPlayer();
    if (!Controller) return 0;

    const auto PlayerState = Controller->GetPlayerState<ASTUPlayerState>();
    return PlayerState ? PlayerState->GetKillsNum() : 0;
}

void USTUPlayerHUDWidget::OnTakeAnyDamage(
    AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    if (HealthComponent && !HealthComponent->IsDead())
    {
        OnTakeDamage();

        if (!IsAnimationPlaying(TakingDamageAnimation))
        {
            PlayAnimation(TakingDamageAnimation);
        }
    }
}

void USTUPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
    if (!NewPawn) return;

    NewPawn->OnTakeAnyDamage.AddDynamic(this, &USTUPlayerHUDWidget::OnTakeAnyDamage);

    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    if (HealthComponent)
    {
        HealthComponent->OnHealthChange.AddUObject(this, &USTUPlayerHUDWidget::OnHealthChange);
        UpdateHealthBarColor();
    }
}

void USTUPlayerHUDWidget::OnHealthChange(float NewHealth)
{
    UpdateHealthBarColor();
}

void USTUPlayerHUDWidget::UpdateHealthBarColor()
{
    if (HealthProgressBar)
    {
        HealthProgressBar->SetFillColorAndOpacity(GetHealthPercent() > ColorThresholdPercent ? DefaultHealthColor : LowHealthColor);
    }
}
