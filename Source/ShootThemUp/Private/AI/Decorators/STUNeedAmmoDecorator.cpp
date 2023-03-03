// Shooter Game. All Rights, If Any, Reserved.

#include "AI/Decorators/STUNeedAmmoDecorator.h"
#include "AIController.h"
#include "Components/STUAIWeaponComponent.h"
#include "STUUtils.h"

USTUNeedAmmoDecorator::USTUNeedAmmoDecorator()
{
    NodeName = "Need Ammo";
}

bool USTUNeedAmmoDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    const auto AIController = OwnerComp.GetAIOwner();
    if (!AIController) return false;

    const auto AIWeaponComponent = STUUtils::GetSTUPlayerComponent<USTUAIWeaponComponent>(AIController->GetPawn());
    if (!AIWeaponComponent) return false;

    return AIWeaponComponent->NeedAmmoFor(WeaponType);
}
