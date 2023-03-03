// Shooter Game. All Rights, If Any, Reserved.

#include "STUGameModeBase.h"
#include "Player/STUBaseCharacter.h"
#include "Player/STUPlayerController.h"
#include "Player/STUPlayerState.h"
#include "AIController.h"
#include "UI/STUGameHUD.h"
#include "Components/STURespawnComponent.h"
#include "Components/STUWeaponComponent.h"
#include "STUUtils.h"
#include "EngineUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUGameMode, All, All);

ASTUGameModeBase::ASTUGameModeBase()
{
    DefaultPawnClass = ASTUBaseCharacter::StaticClass();
    PlayerControllerClass = ASTUPlayerController::StaticClass();
    HUDClass = ASTUGameHUD::StaticClass();
    PlayerStateClass = ASTUPlayerState::StaticClass();
}

void ASTUGameModeBase::StartPlay()
{
    Super::StartPlay();

    SpawnBots();
    CreateTeamsData();
    CurrentRoundIndex = 1;
    StartRound();
    SetMatchState(ESTUMatchState::InProgress);
}

UClass* ASTUGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    if (InController && InController->IsA<AAIController>())
    {
        return AIPawnClass;
    }
    return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void ASTUGameModeBase::CountKill(AController* KillerController, AController* VictimController)
{
    if (KillerController)
    {
        Cast<ASTUPlayerState>(KillerController->PlayerState)->AddKill();
    }
    if (VictimController)
    {
        Cast<ASTUPlayerState>(VictimController->PlayerState)->AddDeath();
        InitiateRespawn(VictimController);
    }
}

float ASTUGameModeBase::GetRoundRemainingSeconds() const
{
    return RoundTimerHandle.IsValid() && GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(RoundTimerHandle)
               ? GetWorld()->GetTimerManager().GetTimerRemaining(RoundTimerHandle)
               : 0.0f;
}

void ASTUGameModeBase::CompleteRespawn(AController* Controller)
{
    ResetOnePlayer(Controller);
}

bool ASTUGameModeBase::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
    const auto WasPaused = Super::SetPause(PC, CanUnpauseDelegate);
    if (WasPaused)
    {
        StopAllFire();
        SetMatchState(ESTUMatchState::Pause);
    }

    return WasPaused;
}

bool ASTUGameModeBase::ClearPause()
{
    const auto WasPauseCleared = Super::ClearPause();
    if (WasPauseCleared)
    {
        SetMatchState(ESTUMatchState::InProgress);
    }

    return WasPauseCleared;
}

void ASTUGameModeBase::SpawnBots()
{
    if (!GetWorld()) return;

    for (int32 i = 0; i < GameData.PlayersNum - 1; i++)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        const auto AIController = GetWorld()->SpawnActor<AAIController>(AIControllerClass, SpawnParams);
        RestartPlayer(AIController);
    }
}

void ASTUGameModeBase::StartRound()
{
    GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &ASTUGameModeBase::OnRoundEnded, GameData.RoundTimeSeconds);
}

void ASTUGameModeBase::OnRoundEnded()
{
    if (CurrentRoundIndex + 1 <= GameData.RoundsNum)
    {
        //UE_LOG(LogSTUGameMode, Display, TEXT("========= round %i/%i ended ========="), CurrentRoundIndex, GameData.RoundsNum);
        CurrentRoundIndex++;
        ResetAllPlayers();
        StartRound();
    }
    else
    {
        GameOver();
    }
}

void ASTUGameModeBase::ResetAllPlayers()
{
    if (!GetWorld()) return;
    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        ResetOnePlayer(It->Get());
    }
}

void ASTUGameModeBase::ResetOnePlayer(AController* Controller)
{
    if (Controller && Controller->GetPawn())
    {
        Controller->GetPawn()->Reset();
    }
    RestartPlayer(Controller);
    SetColorToPlayer(Controller);
}

void ASTUGameModeBase::CreateTeamsData()
{
    if (!GetWorld()) return;

    int32 CurrentTeamID = 1;
    int32 CurrentBotId = 1;
    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        const auto Controller = It->Get();
        if (!Controller) continue;

        const auto CurrentPlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        if (!CurrentPlayerState) continue;

        CurrentPlayerState->SetTeamID(CurrentTeamID);
        CurrentPlayerState->SetTeamColor(DetermineColorByTeamID(CurrentTeamID));

        FString CurrentPlayerName("You");
        if (!Controller->IsPlayerController())
        {
            CurrentPlayerName = "Bot " + FString::FromInt(CurrentBotId++);
        }
        CurrentPlayerState->SetPlayerName(CurrentPlayerName);

        SetColorToPlayer(Controller);

        if (GameData.TeamColors.Num() <= 1) continue;
        CurrentTeamID = CurrentTeamID % GameData.TeamColors.Num() + 1;
    }
}

FLinearColor ASTUGameModeBase::DetermineColorByTeamID(int32 TeamID)
{
    if (TeamID - 1 < GameData.TeamColors.Num())
    {
        return GameData.TeamColors[TeamID - 1];
    }
    else
    {
        UE_LOG(LogSTUGameMode, Warning, TEXT("No color assigned for command ID %i, default color set (%s)"), TeamID,
            *GameData.DefaultTeamColor.ToString())
        return GameData.DefaultTeamColor;
    }
}

void ASTUGameModeBase::SetColorToPlayer(AController* Controller)
{
    if (!Controller) return;

    const auto Character = Cast<ASTUBaseCharacter>(Controller->GetPawn());
    if (!Character) return;

    const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
    if (!PlayerState) return;

    Character->SetTeamMarkerColor(PlayerState->GetTeamColor());
}

void ASTUGameModeBase::LogPlayersStatistic() const
{
    if (!GetWorld()) return;

    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        const auto Controller = It->Get();
        if (!Controller) continue;

        const auto CurrentPlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        if (!CurrentPlayerState) continue;

        CurrentPlayerState->LogData();
    }
}

void ASTUGameModeBase::InitiateRespawn(AController* Controller)
{
    if (GetRoundRemainingSeconds() < MinRemainingTimeForRespawn + GameData.RespawnTimeSeconds) return;

    const auto RespawnComponent = STUUtils::GetSTUPlayerComponent<USTURespawnComponent>(Controller);
    if (!RespawnComponent) return;

    RespawnComponent->SetRespawnTimer(GameData.RespawnTimeSeconds);
}

void ASTUGameModeBase::StopAllFire() {
    for (auto Pawn : TActorRange<APawn>(GetWorld()))
    {
        const auto WeaponComponent = STUUtils::GetSTUPlayerComponent<USTUWeaponComponent>(Pawn);
        if (!WeaponComponent) continue;

        WeaponComponent->AimZoom(false);
        WeaponComponent->StopFire(); 
    }
}

void ASTUGameModeBase::GameOver()
{
    UE_LOG(LogSTUGameMode, Display, TEXT("========= GAME OVER ========="));
    LogPlayersStatistic();

    for (auto Pawn : TActorRange<APawn>(GetWorld()))
    {
        if (Pawn)
        {
            Pawn->TurnOff();
            Pawn->DisableInput(nullptr);
        }
    }

    SetMatchState(ESTUMatchState::GameOver);
}

void ASTUGameModeBase::SetMatchState(ESTUMatchState NewState)
{
    if (MatchState == NewState) return;
    MatchState = NewState;
    OnMatchStateChanged.Broadcast(NewState);
}
