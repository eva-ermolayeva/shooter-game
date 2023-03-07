// Shooter Game. All Rights, If Any, Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUDevDamageActor.generated.h"

class UNiagaraComponent;

UCLASS()
class SHOOTTHEMUP_API ASTUDevDamageActor : public AActor
{
    GENERATED_BODY()

public:
    ASTUDevDamageActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USceneComponent* SceneComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool DoFullDamage = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool EnableDebugSphere = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!EnableDebugSphere"))
    FColor DebugSphereColor = FColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VFX")
    UNiagaraComponent* VFXNiagaraComponent;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};
