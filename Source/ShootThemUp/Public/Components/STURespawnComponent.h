// Shooter Game. All Rights, If Any, Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STURespawnComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTTHEMUP_API USTURespawnComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USTURespawnComponent();

    void SetRespawnTimer(int32 RespawnTime);
    float GetRespawnRemainingSeconds() const;
    bool IsRespawnInProgress() const;

private:
    FTimerHandle RespawnTimerHandle;

    void OnRespawnTimeEnded();
};
