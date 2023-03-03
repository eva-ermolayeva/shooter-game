// Shooter Game. All Rights, If Any, Reserved.

#include "Weapon/Components/STUWeaponFXComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"

USTUWeaponFXComponent::USTUWeaponFXComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUWeaponFXComponent::PlayImpactFX(const FHitResult& Hit)
{
    auto ImpactEffectsData = DefaultImpactEffectsData;

    if (Hit.PhysMaterial.IsValid())
    {
        const auto PhysMat = Hit.PhysMaterial.Get();
        if (ImpactEffectsDataMap.Contains(PhysMat))
        {
            ImpactEffectsData = ImpactEffectsDataMap[PhysMat];
        }
    }

    UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactEffectsData.Sound, Hit.ImpactPoint);

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), //
        ImpactEffectsData.NiagaraSystem,                       //
        Hit.ImpactPoint,                                       //
        Hit.ImpactNormal.Rotation());

    auto DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), //
        ImpactEffectsData.DecalData.Material,                                //
        ImpactEffectsData.DecalData.Size,                                    //
        Hit.ImpactPoint,                                                     //
        Hit.ImpactNormal.Rotation());

    if (DecalComponent)
    {
        DecalComponent->SetFadeOut(ImpactEffectsData.DecalData.LifeTime, ImpactEffectsData.DecalData.FadeOutTime);
    }
}
