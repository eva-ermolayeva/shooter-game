// Shooter Game. All Rights, If Any, Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STUPlayerStatWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class SHOOTTHEMUP_API USTUPlayerStatWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    void SetPlayerName(const FText& Text);
    void SetKillsNum(const FText& Text);
    void SetDeathsNum(const FText& Text);
    void SetTeamID(const FText& Text);
    void SetTeamColor(const FLinearColor& Color);
    void SetCurrentPlayerMarker(bool Visible);

    protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* PlayerNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* KillsNumText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DeathsNumText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TeamText;

    UPROPERTY(meta = (BindWidget))
    UImage* CurrentPlayerMarkerImage;

    UPROPERTY(meta = (BindWidget))
    UImage* TeamColorMarkerImage;
};
