// Shooter Game. All Rights, If Any, Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/STUBaseWidget.h"
#include "STUMenuWidget.generated.h"

class UButton;
class UHorizontalBox;
class USTULevelItemWidget;
class USTUGameInstance;
class USoundCue;

UCLASS()
class SHOOTTHEMUP_API USTUMenuWidget : public USTUBaseWidget
{
    GENERATED_BODY()
protected:
    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* LevelItemsBox;

    UPROPERTY(meta = (BindWidget))
    UButton* StartGameButton;

    UPROPERTY(meta = (BindWidget))
    UButton* QuitGameButton;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> LevelItemWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> LoadingScreenWidgetClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    USoundCue* GameStartSound;

    virtual void NativeOnInitialized() override;

private:
    UPROPERTY()
    TArray<USTULevelItemWidget*> LevelItemWidgets;

    int32 SelectedLevelID = 0;

    UFUNCTION()
    void OnStartGameClicked();

    UFUNCTION()
    void OnQuitGameClicked();

    void InitLevelItemWidgets();
    void OnLevelSelected(const int32 LevelItemID);
    void OnLevelHovered(const int32 LevelItemID);
    void OnLevelUnhovered(const int32 LevelItemID);
    void CycleIteration(int32 Index, const int32 CycleStep, const int32 CycleEnd);
    USTUGameInstance* GetSTUGameInstance() const;
    void OpenLevel();
};
