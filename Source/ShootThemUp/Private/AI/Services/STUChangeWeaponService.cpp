// Shooter Game. All Rights, If Any, Reserved.

#include "AI/Services/STUChangeWeaponService.h"
#include "AIController.h"
#include "Components/STUAIWeaponComponent.h"
#include "STUUtils.h"

USTUChangeWeaponService::USTUChangeWeaponService()
{
    NodeName = "Change Weapon";
}

void USTUChangeWeaponService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    if (FMath::IsNearlyZero(Probability)) return;

    const auto AIController = OwnerComp.GetAIOwner();
    if (FMath::FRand() > Probability || !AIController) return;

    const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUAIWeaponComponent>(AIController->GetPawn());
    if (!WeaponComponent) return;

    WeaponComponent->NextWeapon();
}
