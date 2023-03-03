// Shooter Game. All Rights, If Any, Reserved.

#include "Weapon/STUBaseWeapon.h"
#include "Player/STUBaseCharacter.h"
#include "Player/STUPlayerCharacter.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All)

ASTUBaseWeapon::ASTUBaseWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    SetRootComponent(WeaponMesh);
}

void ASTUBaseWeapon::BeginPlay()
{
    Super::BeginPlay();

    check(WeaponMesh);
    checkf(DefaultAmmo.Bullets > 0, TEXT("Bullets count couldn't be less than 1"));
    checkf(DefaultAmmo.Clips > 0, TEXT("Clips count couldn't be less than 1"));

    CurrentAmmo = DefaultAmmo;
    ChangeClip();
}

void ASTUBaseWeapon::StartFire() {}

void ASTUBaseWeapon::StopFire() {}

void ASTUBaseWeapon::ChangeClip()
{
    if (!CurrentAmmo.Infinite)
    {
        if (CurrentAmmo.Clips == 0)
        {
            UE_LOG(LogBaseWeapon, Display, TEXT("No more clips"));
            return;
        }
        CurrentAmmo.Clips--;
    }
    CurrentAmmo.Bullets = DefaultAmmo.Bullets;
    UE_LOG(LogBaseWeapon, Display, TEXT("------ Clip Changed ------"));
}

bool ASTUBaseWeapon::CanReload() const
{
    return CurrentAmmo.Bullets < DefaultAmmo.Bullets && CurrentAmmo.Clips > 0;
}

bool ASTUBaseWeapon::TryAddAmmo(int32 ClipsAmount)
{
    if (ClipsAmount <= 0 || IsAmmoFull()) return false;

    if (IsAmmoEmpty())
    {
        CurrentAmmo.Clips = FMath::Clamp(ClipsAmount, 0, DefaultAmmo.Clips);
        OnClipEmpty.Broadcast(this);
    }
    else if (!CurrentAmmo.Infinite && CurrentAmmo.Clips < DefaultAmmo.Clips)
    {
        const auto NextClipsAmount = CurrentAmmo.Clips + ClipsAmount;
        if (DefaultAmmo.Clips - NextClipsAmount >= 0)
        {
            CurrentAmmo.Clips = NextClipsAmount;
        }
        else
        {
            CurrentAmmo.Clips = DefaultAmmo.Clips - 1;
            CurrentAmmo.Bullets = DefaultAmmo.Bullets;
        }
    }
    else
    {
        CurrentAmmo.Bullets = DefaultAmmo.Bullets;
        return true;
    }

    return true;
}

void ASTUBaseWeapon::MakeShot() {}

AController* ASTUBaseWeapon::GetController() const
{
    const auto PawnOwner = Cast<APawn>(GetOwner());
    return PawnOwner ? PawnOwner->GetController() : nullptr;
}

bool ASTUBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const
{
    const auto Character = Cast<ACharacter>(GetOwner());
    if (!Character) return false;

    if (Character->IsPlayerControlled())
    {
        const auto Controller = Character->GetController<APlayerController>();
        if (!Controller) return false;

        Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    }
    else
    {
        ViewLocation = GetMuzzleWorldLocation();
        ViewRotation = GetMuzzleWorldRotation();
    }
    return true;
}

FVector ASTUBaseWeapon::GetMuzzleWorldLocation() const
{
    return WeaponMesh->GetSocketLocation(MuzzleSocketName);
}

FRotator ASTUBaseWeapon::GetMuzzleWorldRotation() const
{
    return WeaponMesh->GetSocketRotation(MuzzleSocketName);
}

bool ASTUBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;

    if (!GetPlayerViewPoint(ViewLocation, ViewRotation)) return false;

    TraceStart = ViewLocation;
    TraceEnd = ViewLocation + ViewRotation.Vector() * TraceMaxDistance;
    return true;
}

void ASTUBaseWeapon::CheckShotTrace(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd) const
{
    if (!GetWorld()) return;

    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner());
    CollisionParams.bReturnPhysicalMaterial = true;

    const auto Player = Cast<ASTUPlayerCharacter>(GetOwner());
    if (Player)
    {
        TArray<AActor*> CloserToCameraActors = Player->GetOverlappingActorsBehind();
        CollisionParams.AddIgnoredActors(CloserToCameraActors);
    }

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
}

bool ASTUBaseWeapon::IsNotBackwardShot(FHitResult& HitResult) const
{
    FVector TraceToSocketDitection = GetMuzzleWorldRotation().Vector() * TraceMaxDistance - GetMuzzleWorldLocation();
    FVector TraceToViewDitection = HitResult.ImpactPoint - GetMuzzleWorldLocation();

    const auto ShootAngle = FMath::RadiansToDegrees(
        FMath::Acos(FVector::DotProduct(TraceToSocketDitection.GetSafeNormal(), TraceToViewDitection.GetSafeNormal())));

    return ShootAngle <= 90;
}

bool ASTUBaseWeapon::IsClipEmpty() const
{
    return CurrentAmmo.Bullets == 0;
}
bool ASTUBaseWeapon::IsAmmoEmpty() const
{
    return !CurrentAmmo.Infinite && CurrentAmmo.Clips == 0 && IsClipEmpty();
}
bool ASTUBaseWeapon::IsAmmoFull() const
{
    return CurrentAmmo.Clips == DefaultAmmo.Clips - 1 && CurrentAmmo.Bullets == DefaultAmmo.Bullets;
}
void ASTUBaseWeapon::DecreaseAmmo()
{
    if (CurrentAmmo.Bullets == 0)
    {
        UE_LOG(LogBaseWeapon, Display, TEXT("Clip is empty"));
        return;
    }

    CurrentAmmo.Bullets--;
    // LogAmmo();

    if (IsClipEmpty() && !IsAmmoEmpty())
    {
        OnClipEmpty.Broadcast(this);
    }
}

void ASTUBaseWeapon::LogAmmo()
{
    FString AmmoInfo = "Ammo" + FString::FromInt(CurrentAmmo.Bullets) + " / ";
    AmmoInfo += CurrentAmmo.Infinite ? "Infinite" : FString::FromInt(CurrentAmmo.Clips);
    UE_LOG(LogBaseWeapon, Display, TEXT("%s"), *AmmoInfo);
}

UNiagaraComponent* ASTUBaseWeapon::SpawnMuzzleFX()
{
    return UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFX, //
        WeaponMesh,                                               //
        MuzzleSocketName,                                         //
        FVector::ZeroVector,                                      //
        FRotator::ZeroRotator,                                    //
        EAttachLocation::SnapToTarget,                            //
        true);
}
