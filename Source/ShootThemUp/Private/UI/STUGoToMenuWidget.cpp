// Shooter Game. All Rights, If Any, Reserved.

#include "UI/STUGoToMenuWidget.h"
#include "Components/Button.h"
#include "STUGameInstance.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogSTUGoToMenuWidget, All, All);

void USTUGoToMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ToMenuButton)
    {
        ToMenuButton->OnClicked.AddDynamic(this, &USTUGoToMenuWidget::OnToMenuClicked);
    }
}

void USTUGoToMenuWidget::OnToMenuClicked()
{
    if (!GetWorld()) return;

    const auto GameInstance = GetWorld()->GetGameInstance<USTUGameInstance>();
    if (!GameInstance) return;

    const auto MenuLevelName = GameInstance->GetMenuLevelName();
    if (MenuLevelName.IsNone())
    {
        UE_LOG(LogSTUGoToMenuWidget, Error, TEXT("Menu level name is NONE"));
        return;
    }

    UGameplayStatics::OpenLevel(this, MenuLevelName);
}
