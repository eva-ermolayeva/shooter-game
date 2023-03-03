// Shooter Game. All Rights, If Any, Reserved.

#include "Pickups/STUBasePickup.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

DEFINE_LOG_CATEGORY_STATIC(LogBasePickup, All, All);

ASTUBasePickup::ASTUBasePickup()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
    CollisionComponent->SetSphereRadius(50.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    SetRootComponent(CollisionComponent);
}

void ASTUBasePickup::BeginPlay()
{
    Super::BeginPlay();

    check(CollisionComponent);

    InitialLocation = GetActorLocation();

    AddActorLocalRotation(FRotator(0.0f, FMath::RandRange(0.0f, 180.0f), 0.0f));
}

void ASTUBasePickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (TryGivePickupTo(Cast<APawn>(OtherActor)))
    {
        Despawn();
    }
}

void ASTUBasePickup::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    HandleMovement();
}

bool ASTUBasePickup::CouldBeTaken() const
{
    return !GetWorldTimerManager().IsTimerActive(RespawnTimerHandle);
}

bool ASTUBasePickup::TryGivePickupTo(APawn* PlayerPawn)
{
    return false;
}

void ASTUBasePickup::Despawn()
{
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupTakenSound, GetActorLocation());
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    GetRootComponent()->SetVisibility(false, true);

    GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ASTUBasePickup::Respawn, RespawnTime);
}

void ASTUBasePickup::Respawn()
{
    AddActorLocalRotation(FRotator(0.0f, FMath::RandRange(0.0f, 180.0f), 0.0f));
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    GetRootComponent()->SetVisibility(true, true);
}

void ASTUBasePickup::HandleMovement()
{
    if (GetWorld())
    {
        FVector CurrentLocation = GetActorLocation();
        float Time = GetWorld()->GetTimeSeconds();
        CurrentLocation.Z = InitialLocation.Z + MovementAmplitude * FMath::Sin(MovementFrequency * Time);

        SetActorLocation(CurrentLocation);
    }

    AddActorLocalRotation(FRotator(0.0f, RotationYaw, 0.0f));
}
