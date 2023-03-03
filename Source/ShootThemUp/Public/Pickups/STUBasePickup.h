// Shooter Game. All Rights, If Any, Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUBasePickup.generated.h"

class USphereComponent;
class USoundCue;

UCLASS()
class SHOOTTHEMUP_API ASTUBasePickup : public AActor
{
    GENERATED_BODY()

public:
    ASTUBasePickup();

protected:
    UPROPERTY(VisibleAnywhere, Category = "Pickup")
    USphereComponent* CollisionComponent;

    UPROPERTY(EditAnywhere, Category = "Pickup")
    float RespawnTime = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Pickup")
    float RotationYaw = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Pickup")
    float MovementAmplitude = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Pickup")
    float MovementFrequency = 5.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    USoundCue* PickupTakenSound;

    virtual void BeginPlay() override;
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
    virtual void Tick(float DeltaTime) override;

    bool CouldBeTaken() const;

private:
    FVector InitialLocation;
    FTimerHandle RespawnTimerHandle;
    FTimerHandle CheckOverlapTimerHandle;
    mutable FCriticalSection RespawnGuard;

    virtual bool TryGivePickupTo(APawn* PlayerPawn);
    void Despawn();
    void Respawn();
    void HandleMovement();
    void CheckOverlappingActors();
};
