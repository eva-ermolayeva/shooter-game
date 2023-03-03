// Shooter Game. All Rights, If Any, Reserved.

#include "Components/STUAIPerceptionComponent.h"
#include "AIController.h"
#include "STUUtils.h"
#include "Components/STUHealthComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"

AActor* USTUAIPerceptionComponent::GetClosestEnemy() const
{
    TArray<AActor*> PerceivedActors;
    GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
    if (PerceivedActors.Num() == 0)
    {
        GetCurrentlyPerceivedActors(UAISense_Damage::StaticClass(), PerceivedActors);
        if (PerceivedActors.Num() == 0) return nullptr;
    }

    const auto Controller = Cast<AAIController>(GetOwner());
    if (!Controller) return nullptr;

    const auto Pawn = Controller->GetPawn();
    if (!Pawn) return nullptr;

    float ShortestDistance = MAX_FLT;
    AActor* ClosestActor = nullptr;

    for (const auto PerceivedActor : PerceivedActors)
    {
        const auto PerceivedActorHeathComponent = STUUtils::GetSTUPlayerComponent<USTUHealthComponent>(PerceivedActor);

        const auto PerceivedPawn = Cast<APawn>(PerceivedActor);
        const auto IsPerceivedActorEnemy = PerceivedPawn && STUUtils::AreEnemies(Controller, PerceivedPawn->GetController());

        if (IsPerceivedActorEnemy && PerceivedActorHeathComponent && !PerceivedActorHeathComponent->IsDead())
        {
            float CurrentDistance = (Pawn->GetActorLocation() - PerceivedActor->GetActorLocation()).Size();
            if (CurrentDistance < ShortestDistance)
            {
                ShortestDistance = CurrentDistance;
                ClosestActor = PerceivedActor;
            }
        }
    }
    return ClosestActor;
}
