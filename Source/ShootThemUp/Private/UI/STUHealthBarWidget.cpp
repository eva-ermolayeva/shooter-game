// Shooter Game. All Rights, If Any, Reserved.

#include "UI/STUHealthBarWidget.h"
#include "Components/ProgressBar.h"

void USTUHealthBarWidget::SetHealthPercent(float Percent)
{
    if (!HealthProgressBar) return;

    HealthProgressBar->SetPercent(Percent);
    HealthProgressBar->SetFillColorAndOpacity(Percent > ColorThresholdPercent ? DefaultHealthColor : LowHealthColor);
    HealthProgressBar->SetVisibility((Percent > VisibilityThresholdPercent || FMath::IsNearlyZero(Percent)) //
                                         ? ESlateVisibility::Hidden
                                         : ESlateVisibility::Visible);
}
