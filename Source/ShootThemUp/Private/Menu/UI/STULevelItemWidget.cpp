// Shooter Game. All Rights, If Any, Reserved.

#include "Menu/UI/STULevelItemWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void USTULevelItemWidget::SetLevelData(const FLevelData& Data)
{
    LevelData = Data;

    if (LevelNameText)
    {
        LevelNameText->SetText(FText::FromName(LevelData.LevelDisplayName));
    }

    if (LevelImage)
    {
        LevelImage->SetBrushFromTexture(LevelData.LevelThumbnail);
    }
}

void USTULevelItemWidget::SetSelectedMarker(bool IsSelected)
{
    if (FrameImage)
    {
        FrameImage->SetVisibility(IsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void USTULevelItemWidget::PlayFrameBlinkAnimation()
{
    FrameImage->SetVisibility(ESlateVisibility::Visible);
    PlayAnimation(FrameBlinkAnimation);
}

void USTULevelItemWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (LevelSelectButton)
    {
        LevelSelectButton->OnClicked.AddDynamic(this, &USTULevelItemWidget::OnLevelItemClicked);
        LevelSelectButton->OnHovered.AddDynamic(this, &USTULevelItemWidget::OnLevelItemHovered);
        LevelSelectButton->OnUnhovered.AddDynamic(this, &USTULevelItemWidget::OnLevelItemUnhovered);
    }
}

void USTULevelItemWidget::OnLevelItemClicked()
{
    OnLevelSelected.Broadcast(LevelItemID);
}

void USTULevelItemWidget::OnLevelItemHovered()
{
    OnLevelHovered.Broadcast(LevelItemID);
    /*
    if (FrameImage)
    {
        FrameImage->SetVisibility(ESlateVisibility::Visible);
    }
    */
}

void USTULevelItemWidget::OnLevelItemUnhovered()
{
    OnLevelUnhovered.Broadcast(LevelItemID);
    /*
    if (FrameImage)
    {
        FrameImage->SetVisibility(ESlateVisibility::Hidden);
    }
    */
}
