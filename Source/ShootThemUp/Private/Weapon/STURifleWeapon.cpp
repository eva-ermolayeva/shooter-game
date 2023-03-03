// Shooter Game. All Rights, If Any, Reserved.

#include "Weapon/STURifleWeapon.h"
#include "DrawDebugHelpers.h"
#include "Player/STUBaseCharacter.h"
#include "Player/STUPlayerCharacter.h"
#include "Weapon/Components/STUWeaponFXComponent.h"
#include "Components/AudioComponent.h"
#include "Components/TimelineComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "STUUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogRiffleWeapon, All, All)

ASTURifleWeapon::ASTURifleWeapon()
{
    WeaponFXComponent = CreateDefaultSubobject<USTUWeaponFXComponent>("WeaponFXComponent");
    AimZoomTimeline = CreateDefaultSubobject<UTimelineComponent>("AimZoomTimeline");
}

void ASTURifleWeapon::BeginPlay()
{
    Super::BeginPlay();

    check(WeaponFXComponent);

    if (AimZoomCurve)
    {
        FOnTimelineFloat AimZoomTimelineInterpFunc;
        AimZoomTimelineInterpFunc.BindUFunction(this, FName{TEXT("AimZoomTimelineProgress")});
        AimZoomTimeline->AddInterpFloat(AimZoomCurve, AimZoomTimelineInterpFunc);
    }

    const auto Controller = Cast<APlayerController>(GetController());
    if (Controller && Controller->PlayerCameraManager)
    {
        DefaultFOVAngle = Controller->PlayerCameraManager->GetFOVAngle();
    }
}

void ASTURifleWeapon::StartFire()
{
    InitializeFX();
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTURifleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
}

void ASTURifleWeapon::StopFire()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
    SetFXActive(false);
}

void ASTURifleWeapon::AimZoom(bool IsEnable)
{
    if (AimZoomTimeline)
    {
        if (IsEnable)
        {
            AimZoomTimeline->Play();
        }
        else
        {
            AimZoomTimeline->Reverse();
        }
    }
    else
    {
        AimZoomTimelineProgress(IsEnable ? 1.0f : 0.0f);
    }
}

void ASTURifleWeapon::AimZoomTimelineProgress(float ZoomAlphaValue)
{
    const auto Controller = Cast<APlayerController>(GetController());
    if (!Controller || !Controller->PlayerCameraManager) return;

    float NewFOV = FMath::Lerp(DefaultFOVAngle, AimZoomFOVAngle, ZoomAlphaValue);
    Controller->PlayerCameraManager->SetFOV(NewFOV);
}

void ASTURifleWeapon::MakeShot()
{
    if (!GetWorld())
    {
        StopFire();
        return;
    }

    if (IsAmmoEmpty())
    {
        StopFire();
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), NoAmmoSound, GetActorLocation());
        return;
    }

    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd))
    {
        StopFire();
        return;
    }

    FHitResult HitResult;
    CheckShotTrace(HitResult, TraceStart, TraceEnd);

    FVector TraceFXEnd = TraceEnd;
    if (HitResult.bBlockingHit)
    {
        TraceFXEnd = HitResult.ImpactPoint;
        MakeDamage(HitResult);
    }

    SpawnTraceFX(GetMuzzleWorldLocation(), TraceFXEnd);

    DecreaseAmmo();
}

bool ASTURifleWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;

    if (!GetPlayerViewPoint(ViewLocation, ViewRotation)) return false;

    const auto BulletSpreadHalfRad = FMath::DegreesToRadians(BulletSpread);
    TraceEnd = ViewLocation + FMath::VRandCone(ViewRotation.Vector(), BulletSpreadHalfRad) * TraceMaxDistance;
    TraceStart = ViewLocation;

    const auto Player = Cast<ASTUPlayerCharacter>(GetOwner());
    if (Player && Player->IsObstacleOnRight())
    {
        TraceStart = GetMuzzleWorldLocation();
    }

    return true;
}

void ASTURifleWeapon::MakeDamage(FHitResult& HitResult)
{
    const auto DamagedActor = HitResult.GetActor();
    if (!DamagedActor || !DamagedActor->IsA<ASTUBaseCharacter>()) return;

    const auto IsEnemy = STUUtils::AreEnemies(GetController(), Cast<APawn>(HitResult.GetActor())->GetController());
    if (!IsEnemy) return;

    FPointDamageEvent PointDamageEvent;
    PointDamageEvent.HitInfo = HitResult;
    HitResult.GetActor()->TakeDamage(DamageAmount, PointDamageEvent, GetController(), this);

    WeaponFXComponent->PlayImpactFX(HitResult);
}

void ASTURifleWeapon::InitializeFX()
{
    if (!MuzzleFXComponent)
    {
        MuzzleFXComponent = SpawnMuzzleFX();
    }

    if (!FireAudioComponent)
    {
        FireAudioComponent = UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, MuzzleSocketName);
    }
    SetFXActive(true);
}

void ASTURifleWeapon::SetFXActive(bool IsActive)
{
    if (MuzzleFXComponent)
    {
        MuzzleFXComponent->SetPaused(!IsActive);
        MuzzleFXComponent->SetVisibility(IsActive);
    }

    if (FireAudioComponent)
    {
        IsActive ? FireAudioComponent->Play() : FireAudioComponent->Stop();
    }
}

void ASTURifleWeapon::SpawnTraceFX(FVector TraceStart, FVector TraceEnd)
{
    auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, TraceStart);
    if (TraceFXComponent)
    {
        TraceFXComponent->SetNiagaraVariableVec3(TraceFXTargetParameterName, TraceEnd);
    }
}
