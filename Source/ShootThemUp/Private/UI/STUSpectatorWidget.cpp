// Shooter Game. All Rights, If Any, Reserved.

#include "UI/STUSpectatorWidget.h"
#include "Components/STURespawnComponent.h"
#include "STUUtils.h"

bool USTUSpectatorWidget::GetRespawnRemainingSeconds(float& RemainingSeconds) const
{
    const auto RespawnComponent = STUUtils::GetSTUPlayerComponent<USTURespawnComponent>(GetOwningPlayer());
    if (!RespawnComponent || !RespawnComponent->IsRespawnInProgress()) return false;

    RemainingSeconds = RespawnComponent->GetRespawnRemainingSeconds();
    return true;
}
