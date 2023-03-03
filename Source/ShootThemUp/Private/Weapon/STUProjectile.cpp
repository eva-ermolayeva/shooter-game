// Shooter Game. All Rights, If Any, Reserved.

#include "Weapon/STUProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/Components/STUWeaponFXComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

ASTUProjectile::ASTUProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    ColissionComponent = CreateDefaultSubobject<USphereComponent>("ColissionComponent");
    ColissionComponent->InitSphereRadius(5.0f);
    ColissionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ColissionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    ColissionComponent->bReturnMaterialOnMove = true;
    SetRootComponent(ColissionComponent);

    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
    MovementComponent->InitialSpeed = 2000.0f;
    MovementComponent->ProjectileGravityScale = 0.0f;

    TraceFXNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("TraceFXNiagaraComponent");
    TraceFXNiagaraComponent->SetupAttachment(GetRootComponent());

    WeaponFXComponent = CreateDefaultSubobject<USTUWeaponFXComponent>("WeaponFXComponent");
}

void ASTUProjectile::BeginPlay()
{
    Super::BeginPlay();

    check(MovementComponent);
    check(ColissionComponent);
    check(WeaponFXComponent);

    MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed;
    ColissionComponent->IgnoreActorWhenMoving(GetOwner(), true);
    ColissionComponent->OnComponentHit.AddDynamic(this, &ASTUProjectile::OnProjectileHit);
    SetLifeSpan(LifeSeconds);
}

void ASTUProjectile::OnProjectileHit(
    UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!GetWorld()) return;

    MovementComponent->StopMovementImmediately();

    UGameplayStatics::ApplyRadialDamage(GetWorld(), //
        DamageAmount,                               //
        GetActorLocation(),                         //
        DamageRadius,                               //
        UDamageType::StaticClass(),                 //
        {GetOwner()},                               //
        this,                                       //
        GetController(),                            //
        bDoFullDamage);

    // DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 32, FColor::Red, false, 5.0f);
    WeaponFXComponent->PlayImpactFX(Hit);

    // Delay for smoother trace FX
    SetLifeSpan(1.0f);
}

AController* ASTUProjectile::GetController() const
{
    const auto PawnOwner = Cast<APawn>(GetOwner());
    return PawnOwner ? PawnOwner->GetController() : nullptr;
}
