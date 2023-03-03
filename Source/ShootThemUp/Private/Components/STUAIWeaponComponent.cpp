// Shooter Game. All Rights, If Any, Reserved.

#include "Components/STUAIWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"

void USTUAIWeaponComponent::StartFire()
{
    if (!CanFire()) return;

    if (CurrentWeapon->IsAmmoEmpty())
    {
        NextWeapon();
    }
    else
    {
        CurrentWeapon->StartFire();
    }
}

void USTUAIWeaponComponent::NextWeapon()
{
    if (!CanEquip()) return;
    int32 NextWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    while (NextWeaponIndex != CurrentWeaponIndex)
    {
        if (!Weapons[NextWeaponIndex]->IsAmmoEmpty()) break;
        NextWeaponIndex = (NextWeaponIndex + 1) % Weapons.Num();
    }

    if (NextWeaponIndex != CurrentWeaponIndex)
    {
        CurrentWeaponIndex = NextWeaponIndex;
        EquipWeapon();
    }
}

bool USTUAIWeaponComponent::NeedAmmoFor(TSubclassOf<ASTUBaseWeapon> WeaponType)
{
    for (const auto Weapon : Weapons)
    {
        if (Weapon && Weapon->IsA(WeaponType))
        {
            return !Weapon->IsAmmoFull();
        }
    }
    return false;
}
