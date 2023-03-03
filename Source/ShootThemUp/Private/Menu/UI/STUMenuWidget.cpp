// Shooter Game. All Rights, If Any, Reserved.

#include "Menu/UI/STUMenuWidget.h"
#include "Menu/UI/STULevelItemWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "STUGameInstance.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUMenuWidget, All, All);

void USTUMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (StartGameButton)
    {
        StartGameButton->OnClicked.AddDynamic(this, &USTUMenuWidget::OnStartGameClicked);
    }

    if (QuitGameButton)
    {
        QuitGameButton->OnClicked.AddDynamic(this, &USTUMenuWidget::OnQuitGameClicked);
    }

    InitLevelItemWidgets();
}

void USTUMenuWidget::OnStartGameClicked()
{
    if (!GetWorld()) return;

    UGameplayStatics::PlaySound2D(this, GameStartSound);

    if (LoadingScreenWidgetClass)
    {
        const auto LoadingScreenWidget = CreateWidget(GetWorld(), LoadingScreenWidgetClass);
        LoadingScreenWidget->AddToViewport();
    }

    FTimerHandle OpenLevelTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(OpenLevelTimerHandle, this, &USTUMenuWidget::OpenLevel, 0.3, false);
}

void USTUMenuWidget::OpenLevel()
{
    const auto STUGameInstance = GetSTUGameInstance();
    if (!STUGameInstance) return;

    const auto StartupLevelName = STUGameInstance->GetSelectedLevelData().LevelName;
    if (StartupLevelName.IsNone())
    {
        UE_LOG(LogSTUMenuWidget, Error, TEXT("Startup level name is NONE"));
        return;
    }

    UGameplayStatics::OpenLevel(this, StartupLevelName);
}

void USTUMenuWidget::OnQuitGameClicked()
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

void USTUMenuWidget::OnLevelHovered(const int32 LevelItemID)
{
    if (LevelItemID == SelectedLevelID) return;

    LevelItemWidgets[SelectedLevelID]->SetSelectedMarker(false);
    LevelItemWidgets[LevelItemID]->SetSelectedMarker(true);
    /*
    if (LevelItemID == SelectedLevelID) return;

    LevelItemWidgets[SelectedLevelID]->SetSelectedMarker(false);

    int32 CycleStep = LevelItemID > SelectedLevelID ? 1 : -1;
    int32 CycleEnd = LevelItemID;
    int32 CurrLevelIndex = SelectedLevelID + CycleStep;

    CycleIteration(CurrLevelIndex, CycleStep, CycleEnd);
    */
}

void USTUMenuWidget::OnLevelUnhovered(const int32 LevelItemID)
{
    if (LevelItemID == SelectedLevelID) return;

    LevelItemWidgets[LevelItemID]->SetSelectedMarker(false);
    LevelItemWidgets[SelectedLevelID]->SetSelectedMarker(true);
    /*
    if (LevelItemID == SelectedLevelID) return;

    LevelItemWidgets[LevelItemID]->SetSelectedMarker(false);

    int32 CycleStep = SelectedLevelID > LevelItemID ? 1 : -1;
    int32 CycleEnd = SelectedLevelID;
    int32 CurrLevelIndex = LevelItemID + CycleStep;

    CycleIteration(CurrLevelIndex, CycleStep, CycleEnd);
    */
}

void USTUMenuWidget::CycleIteration(int32 Index, const int32 CycleStep, const int32 CycleEnd) {
    
    if (Index == CycleEnd)
    {
        LevelItemWidgets[CycleEnd]->SetSelectedMarker(true);
        return;
    }

    LevelItemWidgets[Index]->PlayFrameBlinkAnimation();
    Index += CycleStep;

    FTimerDelegate TimerDelegate;
    TimerDelegate.BindLambda([=]() { CycleIteration(Index, CycleStep, CycleEnd); });

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
}

void USTUMenuWidget::InitLevelItemWidgets()
{
    const auto STUGameInstance = GetSTUGameInstance();
    if (!STUGameInstance) return;

    checkf(STUGameInstance->GetLevelsData().Num() != 0, TEXT("Levels data array must not be empty!"));

    if (!LevelItemsBox) return;
    LevelItemsBox->ClearChildren();

    int32 CurrentLevelItemIndex = 0;

    for (const FLevelData LevelData : STUGameInstance->GetLevelsData())
    {
        const auto LevelItemWidget = CreateWidget<USTULevelItemWidget>(GetWorld(), LevelItemWidgetClass);
        if (!LevelItemWidget) continue;

        LevelItemWidget->SetLevelData(LevelData);
        LevelItemWidget->OnLevelSelected.AddUObject(this, &USTUMenuWidget::OnLevelSelected);

        // WIP
        LevelItemWidget->SetLevelItemID(CurrentLevelItemIndex);
        CurrentLevelItemIndex++;

        LevelItemWidget->OnLevelHovered.AddUObject(this, &USTUMenuWidget::OnLevelHovered);
        LevelItemWidget->OnLevelUnhovered.AddUObject(this, &USTUMenuWidget::OnLevelUnhovered);
        //  WIP

        LevelItemsBox->AddChild(LevelItemWidget);
        LevelItemWidgets.Add(LevelItemWidget);
    }

    UE_LOG(LogSTUMenuWidget, Warning, TEXT("InitItems currLevelID = %i"), SelectedLevelID);

    if (STUGameInstance->GetSelectedLevelData().LevelName.IsNone())
    {
        OnLevelSelected(SelectedLevelID);
    }
    else
    {
        const auto SelectedLevelName = STUGameInstance->GetSelectedLevelData().LevelName;
        for (auto LevelItemWidget : LevelItemWidgets)
        {
            if (LevelItemWidget->GetLevelData().LevelName != SelectedLevelName) continue;
            OnLevelSelected(LevelItemWidget->GetLevelItemID());
        }
    }
}

void USTUMenuWidget::OnLevelSelected(const int32 LevelItemID)
{
    const auto STUGameInstance = GetSTUGameInstance();
    if (!STUGameInstance) return;

    STUGameInstance->SetSelectedLevelData(LevelItemWidgets[LevelItemID]->GetLevelData());
    SelectedLevelID = LevelItemID;

    LevelItemWidgets[LevelItemID]->SetSelectedMarker(true);
}

USTUGameInstance* USTUMenuWidget::GetSTUGameInstance() const
{
    if (!GetWorld()) return nullptr;
    return GetWorld()->GetGameInstance<USTUGameInstance>();
}
