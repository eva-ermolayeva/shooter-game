// Shooter Game. All Rights, If Any, Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "STUCoreTypes.h"
#include "STUGameModeBase.generated.h"

class AAIController;

UCLASS()
class SHOOTTHEMUP_API ASTUGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASTUGameModeBase();

    FOnMatchStateChangedSignature OnMatchStateChanged;

    virtual void StartPlay() override;
    virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

    void CountKill(AController* KillerController, AController* VictimController);

    float GetRoundRemainingSeconds() const;
    int32 GetCurrentRoundNum() const { return CurrentRoundIndex; }
    int32 GetTotalRoundsNum() const { return GameData.RoundsNum; }

    void CompleteRespawn(AController* Controller);
    virtual bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;
    virtual bool ClearPause() override;


protected:
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    TSubclassOf<AAIController> AIControllerClass;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    TSubclassOf<APawn> AIPawnClass;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    FGameData GameData;

private:
    ESTUMatchState MatchState = ESTUMatchState::WaitingToStart;
    int32 CurrentRoundIndex = 1;
    FTimerHandle RoundTimerHandle;
    int32 MinRemainingTimeForRespawn = 5; //in seconds

    void SpawnBots();
    void StartRound();
    void OnRoundEnded();

    void ResetAllPlayers();
    void ResetOnePlayer(AController* Controller);

    void CreateTeamsData();
    FLinearColor DetermineColorByTeamID(int32 TeamID);
    void SetColorToPlayer(AController* Controller);

    void LogPlayersStatistic() const;

    void InitiateRespawn(AController* Controller);

    void StopAllFire();
    void GameOver();

    void SetMatchState(ESTUMatchState NewState);
};
