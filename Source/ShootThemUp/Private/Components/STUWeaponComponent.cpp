// Shooter Game. All Rights, If Any, Reserved.

#include "Components/STUWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"
#include "GameFramework/Character.h"
#include "Animations/STUEquipStartedAnimNotify.h"
#include "Animations/STUEquipFinishedAnimNotify.h"
#include "Animations/STUReloadFinishedAnimNotify.h"
#include "Animations/AnimUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All);

constexpr static int32 MaxWeaponNum = 2;

USTUWeaponComponent::USTUWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    checkf(WeaponData.Num() == MaxWeaponNum, TEXT("Character can only hold %i weapon items"), MaxWeaponNum);

    InitAnimations();
    CurrentWeaponIndex = 0;
    SpawnWeapons();
    EquipWeapon();
}

void USTUWeaponComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    CurrentWeapon = nullptr;
    for (auto Weapon : Weapons)
    {
        Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        Weapon->Destroy();
    }
    Weapons.Empty();

    Super::EndPlay(EndPlayReason);
}

void USTUWeaponComponent::SpawnWeapons()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || !GetWorld()) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Character;

    for (auto OneWeaponData : WeaponData)
    {
        auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(OneWeaponData.WeaponClass, SpawnParams);
        if (!Weapon) return;

        Weapon->OnClipEmpty.AddUObject(this, &USTUWeaponComponent::OnEmptyClip);

        Weapons.Add(Weapon);
        AttachWeaponToSocket(Weapon, Character->GetMesh(), WeaponArmorySocketName);
    }
}

void USTUWeaponComponent::AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, FName SocketName)
{
    if (!Weapon || !SceneComponent) return;

    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
    Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

void USTUWeaponComponent::EquipWeapon() //(int32 WeaponIndex)
{
    // if (WeaponIndex < 0 || WeaponIndex >= Weapons.Num()) return;
    if (CurrentWeaponIndex < 0 || CurrentWeaponIndex >= Weapons.Num()) return;

    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    if (CurrentWeapon)
    {
        CurrentWeapon->AimZoom(false);
        CurrentWeapon->StopFire();
    }

    PlayAnimMontage(EquipAnimMontage);
    bEquipAnimInProgress = true;
}

void USTUWeaponComponent::StartFire()
{
    if (!CanFire()) return;

    CurrentWeapon->StartFire();
}

void USTUWeaponComponent::StopFire()
{
    if (!CurrentWeapon) return;
    CurrentWeapon->StopFire();
}

void USTUWeaponComponent::PrevWeapon()
{
    if (!CanEquip()) return;

    CurrentWeaponIndex = (CurrentWeaponIndex - 1) < 0 //
                             ? Weapons.Num() - 1
                             : (CurrentWeaponIndex - 1) % Weapons.Num();
    EquipWeapon();
}

void USTUWeaponComponent::NextWeapon()
{
    if (!CanEquip()) return;

    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    EquipWeapon();
}

void USTUWeaponComponent::Reload()
{
    ChangeClip();
}

bool USTUWeaponComponent::GetCurrentWeaponUIData(FWeaponUIData& WeaponUIData) const
{
    if (!CurrentWeapon) return false;

    WeaponUIData = CurrentWeapon->GetUIData();
    return true;
}

bool USTUWeaponComponent::GetCurrentAmmoData(FAmmoData& AmmoData) const
{
    if (!CurrentWeapon) return false;

    AmmoData = CurrentWeapon->GetAmmoData();
    return true;
}

bool USTUWeaponComponent::TryAddAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType, int32 ClipsAmount)
{
    for (const auto Weapon : Weapons)
    {
        if (Weapon && Weapon->IsA(WeaponType))
        {
            return Weapon->TryAddAmmo(ClipsAmount);
        }
    }
    return false;
}

void USTUWeaponComponent::AimZoom(bool IsEnable)
{
    if (!CurrentWeapon) return;
    CurrentWeapon->AimZoom(IsEnable);
}

void USTUWeaponComponent::PlayAnimMontage(UAnimMontage* Animation)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    Character->PlayAnimMontage(Animation);
}

void USTUWeaponComponent::InitAnimations()
{
    if (!EquipAnimMontage) return;

    auto EquipStartedAnimNotify = AnimUtils::FindFirstNotifyByClass<USTUEquipStartedAnimNotify>(EquipAnimMontage);
    if (EquipStartedAnimNotify)
    {
        EquipStartedAnimNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnEquipStarted);
    }
    else
    {
        UE_LOG(LogWeaponComponent, Error, TEXT("Equip start anim notify is forgotten to set"));
        checkNoEntry();
    }

    auto EquipFinishedAnimNotify = AnimUtils::FindFirstNotifyByClass<USTUEquipFinishedAnimNotify>(EquipAnimMontage);
    if (EquipFinishedAnimNotify)
    {
        EquipFinishedAnimNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnEquipFinished);
    }
    else
    {
        UE_LOG(LogWeaponComponent, Error, TEXT("Equip finish anim notify is forgotten to set"));
        checkNoEntry();
    }

    for (auto OneWeaponData : WeaponData)
    {
        auto ReloadFinishedAnimNotify = AnimUtils::FindFirstNotifyByClass<USTUReloadFinishedAnimNotify>(OneWeaponData.ReloadAnimMontage);
        if (!ReloadFinishedAnimNotify)
        {
            UE_LOG(LogWeaponComponent, Error, TEXT("Reload anim notify is forgotten to set"));
            checkNoEntry();
        }
        ReloadFinishedAnimNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnReloadFinished);
    }
}

void USTUWeaponComponent::OnEquipStarted(USkeletalMeshComponent* MeshComponent)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || !MeshComponent || MeshComponent != Character->GetMesh()) return;

    // if (CurrentWeaponIndex < 0 || CurrentWeaponIndex >= Weapons.Num()) return;

    if (CurrentWeapon)
    {
        AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
    }

    CurrentWeapon = Weapons[CurrentWeaponIndex];
    const auto CurrentWeaponData =
        WeaponData.FindByPredicate([&](const FWeaponData& Data) { return Data.WeaponClass == CurrentWeapon->GetClass(); });
    CurrentReloadAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;
    AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);
}

void USTUWeaponComponent::OnEquipFinished(USkeletalMeshComponent* MeshComponent)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());

    if (!Character || !MeshComponent || MeshComponent != Character->GetMesh()) return;

    bEquipAnimInProgress = false;
}

void USTUWeaponComponent::OnReloadFinished(USkeletalMeshComponent* MeshComponent)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());

    if (!Character || !MeshComponent || MeshComponent != Character->GetMesh()) return;

    bReloadAnimInProgress = false;
}

bool USTUWeaponComponent::CanEquip() const
{
    return !bEquipAnimInProgress && !bReloadAnimInProgress;
}

bool USTUWeaponComponent::CanFire() const
{
    return CurrentWeapon && !bEquipAnimInProgress && !bReloadAnimInProgress;
}

bool USTUWeaponComponent::CanReload() const
{
    return CurrentWeapon             //
           && !bEquipAnimInProgress  //
           && !bReloadAnimInProgress //
           && CurrentWeapon->CanReload();
}

void USTUWeaponComponent::OnEmptyClip(ASTUBaseWeapon* EmptyAmmoWeapon)
{
    if (!EmptyAmmoWeapon) return;

    if (CurrentWeapon == EmptyAmmoWeapon)
    {
        ChangeClip();
    }
    else
    {
        for (const auto Weapon : Weapons)
        {
            if (Weapon == EmptyAmmoWeapon)
            {
                Weapon->ChangeClip();
            }
        }
    }
}

void USTUWeaponComponent::ChangeClip()
{
    if (!CanReload()) return;

    CurrentWeapon->StopFire();
    CurrentWeapon->ChangeClip();
    PlayAnimMontage(CurrentReloadAnimMontage);
    bReloadAnimInProgress = true;
}
