// Shooter Game. All Rights, If Any, Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/STUBaseWeapon.h"
//#include "Components/TimelineComponent.h"
#include "STURifleWeapon.generated.h"

class USTUWeaponFXComponent;
class UNiagaraComponent;
class UAudioComponent;
class UTimelineComponent;
class UCurveFloat;
//class FOnTimelineFloat;

UCLASS()
class SHOOTTHEMUP_API ASTURifleWeapon : public ASTUBaseWeapon
{
    GENERATED_BODY()

public:
    ASTURifleWeapon();

    virtual void StartFire() override;
    virtual void StopFire() override;
    virtual void AimZoom(bool IsEnable) override;

    UFUNCTION()
    void AimZoomTimelineProgress(float ZoomAlphaValue);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float BasicDamageAmount = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float HeadshotDamageAmount = 40.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
    FName HeadBoneName = "b_head";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage", meta = (ClampMin = "0.0", ClampMax = "10.0"))
    float TimeBetweenShots = 0.1f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
    float BulletSpread = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* TraceFX = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    FString TraceFXTargetParameterName = "TraceTarget";

    UPROPERTY(VisibleAnywhere, Category = "VFX")
    USTUWeaponFXComponent* WeaponFXComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom", meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float AimZoomFOVAngle = 50.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Zoom")
    UCurveFloat* AimZoomCurve = nullptr;

    UPROPERTY()
    UTimelineComponent* AimZoomTimeline;

    virtual void BeginPlay() override;
    virtual void MakeShot() override;
    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const override;

private:
    float DefaultFOVAngle = 90.0f;
    FTimerHandle ShotTimerHandle;

    UPROPERTY()
    UNiagaraComponent* MuzzleFXComponent = nullptr;

    UPROPERTY()
    UAudioComponent* FireAudioComponent = nullptr;

    void MakeDamage(FHitResult& HitResult);
    void InitializeFX();
    void SetFXActive(bool IsActive);
    void SrawnTraceFX(FVector TraceStart, FVector TraceEnd);
};
