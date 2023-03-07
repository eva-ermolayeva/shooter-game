// Shooter Game. All Rights, If Any, Reserved.

#include "Dev/STUDevDamageActor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

ASTUDevDamageActor::ASTUDevDamageActor()
{
    PrimaryActorTick.bCanEverTick = true;

    SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
    SetRootComponent(SceneComponent);

    VFXNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("VFXNiagaraComponent");
    VFXNiagaraComponent->SetupAttachment(GetRootComponent());
}

void ASTUDevDamageActor::BeginPlay()
{
    Super::BeginPlay();
}

void ASTUDevDamageActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (EnableDebugSphere)
    {
        DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 40, DebugSphereColor);
    }
    UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), Radius, DamageType, {}, this, nullptr, DoFullDamage);
}
