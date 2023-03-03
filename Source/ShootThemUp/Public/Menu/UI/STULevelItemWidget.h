// Shooter Game. All Rights, If Any, Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STUCoreTypes.h"
#include "STULevelItemWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;

UCLASS()
class SHOOTTHEMUP_API USTULevelItemWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    FOnLevelSelectedSignature OnLevelSelected;
    FOnLevelHoveredSignature OnLevelHovered;
    FOnLevelUnhoveredSignature OnLevelUnhovered;

    int32 GetLevelItemID() { return LevelItemID; }
    void SetLevelItemID(int32 NewLevelItemID) { LevelItemID = NewLevelItemID; }

    void SetLevelData(const FLevelData& Data);
    FLevelData GetLevelData() const { return LevelData; }

    void SetSelectedMarker(bool IsSelected);
    void PlayFrameBlinkAnimation();

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* LevelSelectButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* LevelNameText;

    UPROPERTY(meta = (BindWidget))
    UImage* LevelImage = nullptr;

    UPROPERTY(meta = (BindWidget))
    UImage* FrameImage = nullptr;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* FrameBlinkAnimation;

    virtual void NativeOnInitialized() override;

private:
    int32 LevelItemID;
    FLevelData LevelData;

    UFUNCTION()
    void OnLevelItemClicked();

    UFUNCTION()
    void OnLevelItemHovered();

    UFUNCTION()
    void OnLevelItemUnhovered();
};
