// Shooter Game. All Rights, If Any, Reserved.

#include "Player/STUPlayerController.h"
#include "Components/STURespawnComponent.h"
#include "STUGameModeBase.h"
#include "STUGameInstance.h"

ASTUPlayerController::ASTUPlayerController()
{
    RespawnComponent = CreateDefaultSubobject<USTURespawnComponent>("RespawnComponent");
}

void ASTUPlayerController::BeginPlay()
{
    if (GetWorld())
    {
        const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->OnMatchStateChanged.AddUObject(this, &ASTUPlayerController::OnMatchStateChanged);
        }
    }
}

void ASTUPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if (!InputComponent) return;

    InputComponent->BindAction("PauseGame", IE_Pressed, this, &ASTUPlayerController::OnPauseGame);
    InputComponent->BindAction("MuteSound", IE_Pressed, this, &ASTUPlayerController::OnSoundMute);
}

void ASTUPlayerController::OnPauseGame()
{
    if (!GetWorld()) return;

    const auto GameMode = GetWorld()->GetAuthGameMode();
    if (!GameMode) return;

    if (GameMode->IsPaused())
    {
        GameMode->ClearPause();
    }
    else
    {
        GameMode->SetPause(this);
    }
}

void ASTUPlayerController::OnMatchStateChanged(ESTUMatchState NewState) {
    if (NewState == ESTUMatchState::InProgress)
    {
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
    }
    else
    {
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;
    }
}

void ASTUPlayerController::OnSoundMute()
{
    const auto GameInstance = GetGameInstance<USTUGameInstance>();
    if (!GameInstance) return;

    GameInstance->ToggleVolume();
}
