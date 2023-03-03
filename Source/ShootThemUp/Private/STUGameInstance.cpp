// Shooter Game. All Rights, If Any, Reserved.


#include "STUGameInstance.h"
#include "Sound/STUSoundFuncLib.h"

void USTUGameInstance::ToggleVolume() {
    USTUSoundFuncLib::ToggleSoundClassVolume(MasterSoundClass);
}
