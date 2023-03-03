// Shooter Game. All Rights, If Any, Reserved.

#include "AI/Decorators/STUHealthPercentDecorator.h"
#include "Components/STUHealthComponent.h"
#include "AIController.h"
#include "STUUtils.h"

USTUHealthPercentDecorator::USTUHealthPercentDecorator()
{
    NodeName = "Check Health Percent";
}

bool USTUHealthPercentDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    const auto AIController = OwnerComp.GetAIOwner();

    if (!AIController) return false;
    const auto HealthComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(AIController->GetPawn());
    if (!HealthComponent || HealthComponent->IsDead()) return false;

    return HealthComponent->GetHealthPercent() <= ThresholdHealthPercent;
}
