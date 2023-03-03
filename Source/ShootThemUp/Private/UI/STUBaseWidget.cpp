// Shooter Game. All Rights, If Any, Reserved.

#include "UI/STUBaseWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void USTUBaseWidget::Show() {
    UGameplayStatics::PlaySound2D(this, ShowSound);
    PlayAnimation(ShowAnimation);
}
