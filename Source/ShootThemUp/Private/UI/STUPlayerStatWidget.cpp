// Shooter Game. All Rights, If Any, Reserved.

#include "UI/STUPlayerStatWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void USTUPlayerStatWidget::SetPlayerName(const FText& Text)
{
    if (!PlayerNameText) return;
    PlayerNameText->SetText(Text);
}

void USTUPlayerStatWidget::SetKillsNum(const FText& Text)
{
    if (!KillsNumText) return;
    KillsNumText->SetText(Text);
}

void USTUPlayerStatWidget::SetDeathsNum(const FText& Text)
{
    if (!DeathsNumText) return;
    DeathsNumText->SetText(Text);
}

void USTUPlayerStatWidget::SetTeamID(const FText& Text)
{
    if (!TeamText) return;
    TeamText->SetText(Text);
}

void USTUPlayerStatWidget::SetTeamColor(const FLinearColor& Color)
{
    if (!TeamColorMarkerImage) return;
    TeamColorMarkerImage->SetColorAndOpacity(Color);
}

void USTUPlayerStatWidget::SetCurrentPlayerMarker(bool Visible)
{
    if (!CurrentPlayerMarkerImage) return;
    CurrentPlayerMarkerImage->SetVisibility(Visible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
