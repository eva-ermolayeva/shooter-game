// Shooter Game. All Rights, If Any, Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "STUPlayerState.generated.h"

UCLASS()
class SHOOTTHEMUP_API ASTUPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    int32 GetTeamID() const { return TeamID; }
    void SetTeamID(int32 NewID) { TeamID = NewID; }

    FLinearColor GetTeamColor() const { return TeamColor; }
    void SetTeamColor(const FLinearColor& NewColor) { TeamColor = NewColor; }

    int32 GetKillsNum() { return KillsNum; }
    void AddKill() { ++KillsNum; }

    int32 GetDeathsNum() { return DeathsNum; }
    void AddDeath() { ++DeathsNum; }

    void LogData() const;

private:
    int32 TeamID;
    FLinearColor TeamColor;

    int32 KillsNum = 0;
    int32 DeathsNum = 0;
};
