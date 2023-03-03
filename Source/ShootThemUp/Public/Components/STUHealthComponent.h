// Shooter Game. All Rights, If Any, Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUCoreTypes.h"
#include "STUHealthComponent.generated.h"

class UCameraShakeBase;
class UPhysicalMaterial;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent)) class SHOOTTHEMUP_API USTUHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USTUHealthComponent();

    FOnHealthChangeSignature OnHealthChange;
    FOnDeathSignature OnDeath;

    UFUNCTION(BlueprintCallable, Category = "Health")
    bool IsDead() const { return FMath::IsNearlyZero(Health); }

    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealthPercent() const { return Health / MaxHealth; }

    float GetHealth() const { return Health; }

    bool TryAddHealth(float HealthAmount);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal")
    bool IsAutoHealOn = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal",
        meta = (EditCondition = "IsAutoHealOn", ClampMin = "0.01", ClampMax = "3600.0"))
    float HealUpdateTime = 0.5f;

    UPROPERTY(
        EditAnywhere, BlueprintReadWrite, Category = "Heal", meta = (EditCondition = "IsAutoHealOn", ClampMin = "0.0", ClampMax = "900.0"))
    float HealDelayTime = 3.0f;

    UPROPERTY(
        EditAnywhere, BlueprintReadWrite, Category = "Heal", meta = (EditCondition = "IsAutoHealOn", ClampMin = "0.0", ClampMax = "100.0"))
    float HealModifier = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health",
        meta = (ToolTip = "Damage modifiers by Physical Materials. Use 0.0 value to get lethal damage."))
    TMap<UPhysicalMaterial*, float> DamageModifiers;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    TSubclassOf<UCameraShakeBase> CameraShake;

    virtual void BeginPlay() override;

private:
    float Health = 0.0f;

    FTimerHandle HealTimerHandle;

    UFUNCTION()
    void OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation,
        class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType,
        AActor* DamageCauser);

    UFUNCTION()
    void OnTakeRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo,
        class AController* InstigatedBy, AActor* DamageCauser);

    void ApplyDamage(float Damage, AController* InstigatedBy);
    float GetModifiedDamage(AActor* DamagedActor, float Damage, const FName& BoneName);
    void ReportDamageEvent(float Damage, AController* InstigatedBy);
    void HealUpdate();
    void SetHealth(float NewHealth); 

    void PlayCameraShake();

    void CountDeath(AController* KillerController) const;
};
