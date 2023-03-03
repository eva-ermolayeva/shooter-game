// Shooter Game. All Rights, If Any, Reserved.

#include "Player/STUPlayerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUPlayerState, All, All);

void ASTUPlayerState::LogData() const
{
    UE_LOG(LogSTUPlayerState, Display, TEXT("Team ID: %i, Kills num: %i, Deaths num: %i"), TeamID, KillsNum, DeathsNum);
}