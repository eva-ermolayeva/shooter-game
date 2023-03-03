// Shooter Game. All Rights, If Any, Reserved.

#include "Components/STURespawnComponent.h"
#include "STUGameModeBase.h"

USTURespawnComponent::USTURespawnComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTURespawnComponent::SetRespawnTimer(int32 RespawnTime)
{
    if (RespawnTime == 0) OnRespawnTimeEnded();
    if (!GetWorld()) return;

    GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &USTURespawnComponent::OnRespawnTimeEnded, RespawnTime);
}

float USTURespawnComponent::GetRespawnRemainingSeconds() const
{
    return RespawnTimerHandle.IsValid() && GetWorld() ? GetWorld()->GetTimerManager().GetTimerRemaining(RespawnTimerHandle) : 0.f;
}

bool USTURespawnComponent::IsRespawnInProgress() const
{
    return GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(RespawnTimerHandle);
}

void USTURespawnComponent::OnRespawnTimeEnded()
{
    if (!GetWorld()) return;

    const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
    if (!GameMode) return;

    GameMode->CompleteRespawn(Cast<AController>(GetOwner()));
}
