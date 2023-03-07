// Shooter Game. All Rights, If Any, Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "STUBaseCharacter.generated.h"

class USTUHealthComponent;
class USTUWeaponComponent;
class USoundCue;

UCLASS()
class SHOOTTHEMUP_API ASTUBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASTUBaseCharacter(const FObjectInitializer& ObjInit);

    UFUNCTION(BlueprintCallable)
    float GetMovementDirection() const;

    virtual void Tick(float DeltaTime) override;
    virtual void TurnOff() override;
    virtual void Reset() override;
    virtual bool IsWantToRun() const { return false; }
    void SetTeamMarkerColor(const FLinearColor& Color);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USTUHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USTUWeaponComponent* WeaponComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Animations")
    UAnimMontage* DeathAnimMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
    FName WeaponSocket = "WeaponSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (ClampMin = "0.0", ClampMax = "360.0"))
    float LifeSpanOnDeath = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FVector2D LandedDamage = FVector2D(5.0f, 100.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FVector2D LandedDamageVelocity = FVector2D(700.0f, 2500.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Material")
    FName TeamMarkerMatColorName = "Paint Color";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    USoundCue* DeathSound;

    virtual void BeginPlay() override;
    virtual void OnDeath();
    virtual void OnHealthChange(float Health) {}

private:
    UFUNCTION()
    void OnGroundLanded(const FHitResult& Hit);
};
