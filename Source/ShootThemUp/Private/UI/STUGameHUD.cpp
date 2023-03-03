// Shooter Game. All Rights, If Any, Reserved.

#include "UI/STUGameHUD.h"
#include "Engine/Canvas.h"
#include "UI/STUBaseWidget.h"
#include "STUGameModeBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogGameHUD, All, All)

void ASTUGameHUD::DrawHUD()
{
    Super::DrawHUD();

    // DrawCrossHair();
}

void ASTUGameHUD::BeginPlay()
{
    Super::BeginPlay();

    GameWidgets.Add(ESTUMatchState::InProgress, CreateWidget<USTUBaseWidget>(GetWorld(), PlayerHUDWidgetClass));
    GameWidgets.Add(ESTUMatchState::Pause, CreateWidget<USTUBaseWidget>(GetWorld(), PauseWidgetClass));
    GameWidgets.Add(ESTUMatchState::GameOver, CreateWidget<USTUBaseWidget>(GetWorld(), GameOverWidgetClass));

    for (TTuple<ESTUMatchState, UUserWidget*> Widget : GameWidgets)
    {
        if (!Widget.Value) continue;

        Widget.Value->AddToViewport();
        Widget.Value->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (GetWorld())
    {
        const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->OnMatchStateChanged.AddUObject(this, &ASTUGameHUD::OnMatchStateChanged);
        }
    }
}

void ASTUGameHUD::DrawCrossHair()
{
    int32 CenterX = Canvas->SizeX * 0.5f;
    int32 CenterY = Canvas->SizeY * 0.5f;

    const float HalfLineSize = 10.0f;
    const float LineThickness = 2.0f;
    const FLinearColor LineColor = FLinearColor::Blue;

    DrawLine(CenterX - HalfLineSize, CenterY, CenterX + HalfLineSize, CenterY, LineColor, LineThickness);
    DrawLine(CenterX, CenterY - HalfLineSize, CenterX, CenterY + HalfLineSize, LineColor, LineThickness);
}

void ASTUGameHUD::OnMatchStateChanged(ESTUMatchState NewState)
{
    UE_LOG(LogGameHUD, Display, TEXT("Match state changed, new state is %s"), *UEnum::GetValueAsString(NewState));

    if (!GameWidgets.Contains(NewState)) return;

    if (CurrentDisplayedWidget)
    {
        CurrentDisplayedWidget->SetVisibility(ESlateVisibility::Collapsed);
    }

    CurrentDisplayedWidget = GameWidgets[NewState];

    if (CurrentDisplayedWidget)
    {
        CurrentDisplayedWidget->SetVisibility(ESlateVisibility::Visible);
        CurrentDisplayedWidget->Show();
    }
}
