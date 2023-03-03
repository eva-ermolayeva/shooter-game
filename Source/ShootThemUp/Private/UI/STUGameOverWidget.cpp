// Shooter Game. All Rights, If Any, Reserved.

#include "UI/STUGameOverWidget.h"
#include "STUGameModeBase.h"
#include "Player/STUPlayerState.h"
#include "UI/STUPlayerStatWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "STUUtils.h"

void USTUGameOverWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ResetLevelButton)
    {
        ResetLevelButton->OnClicked.AddDynamic(this, &USTUGameOverWidget::OnResetLevelClicked);
    }

    if (GetWorld())
    {
        const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->OnMatchStateChanged.AddUObject(this, &USTUGameOverWidget::OnMatchStateChanged);
        }
    }
}

void USTUGameOverWidget::OnResetLevelClicked()
{
    const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);
    UGameplayStatics::OpenLevel(this, FName(CurrentLevelName));
}

void USTUGameOverWidget::OnMatchStateChanged(ESTUMatchState NewState)
{
    if (NewState == ESTUMatchState::GameOver)
    {
        UpdatePlayersStat();
    }
}

void USTUGameOverWidget::UpdatePlayersStat()
{
    if (!GetWorld() || !AllPlayersStatBox || !PlayerStatRowWidgetClass) return;

    AllPlayersStatBox->ClearChildren();

    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        const auto Controller = It->Get();
        if (!Controller) continue;

        const auto PlayerState = Cast<ASTUPlayerState>(Controller->PlayerState);
        if (!PlayerState) continue;

        auto NewPlayerStatRow = Cast<USTUPlayerStatWidget>(CreateWidget<UUserWidget>(GetWorld(), PlayerStatRowWidgetClass));
        if (!NewPlayerStatRow) continue;

        NewPlayerStatRow->SetPlayerName(FText::FromString(PlayerState->GetPlayerName()));
        NewPlayerStatRow->SetKillsNum(STUUtils::TextFromInt(PlayerState->GetKillsNum()));
        NewPlayerStatRow->SetDeathsNum(STUUtils::TextFromInt(PlayerState->GetDeathsNum()));
        NewPlayerStatRow->SetTeamID(STUUtils::TextFromInt(PlayerState->GetTeamID()));
        NewPlayerStatRow->SetTeamColor(PlayerState->GetTeamColor());
        NewPlayerStatRow->SetCurrentPlayerMarker(Controller->IsPlayerController());

        AllPlayersStatBox->AddChild(NewPlayerStatRow);
    }
}
