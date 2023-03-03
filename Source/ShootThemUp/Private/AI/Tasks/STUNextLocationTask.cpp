// Shooter Game. All Rights, If Any, Reserved.

#include "AI/Tasks/STUNextLocationTask.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

USTUNextLocationTask::USTUNextLocationTask()
{
    NodeName = "Find Next Location";
}

EBTNodeResult::Type USTUNextLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const auto AIController = OwnerComp.GetAIOwner();
    const auto Blackboard = OwnerComp.GetBlackboardComponent();

    if (!AIController || !Blackboard) return EBTNodeResult::Failed;

    const auto Pawn = AIController->GetPawn();
    if (!Pawn) return EBTNodeResult::Failed;

    const auto NavSystem = UNavigationSystemV1::GetCurrent(Pawn);
    if (!NavSystem) return EBTNodeResult::Failed;
    
    FVector SearchCenter = Pawn->GetActorLocation();
    if (!SelfCenter)
    {
        const auto CenterActor = Cast<AActor>(Blackboard->GetValueAsObject(SearchCenterActorKey.SelectedKeyName));
        if (!CenterActor) return EBTNodeResult::Failed;

        SearchCenter = CenterActor->GetActorLocation();
    }

    FNavLocation NavLocation;
    const auto IsNextLocationFound = NavSystem->GetRandomReachablePointInRadius(SearchCenter, SearchRadius, NavLocation);
    if (!IsNextLocationFound) return EBTNodeResult::Failed;

    Blackboard->SetValueAsVector(AimLocatinKey.SelectedKeyName, NavLocation.Location);
    return EBTNodeResult::Succeeded;
}
