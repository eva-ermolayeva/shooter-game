// Shooter Game. All Rights, If Any, Reserved.

#include "Components/STUHealthComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "STUUtils.h"
#include "STUGameModeBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Perception/AISense_Damage.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All)

USTUHealthComponent::USTUHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    checkf(MaxHealth > 0, TEXT("Maximum Health number couldn't be less than 1"));
    SetHealth(MaxHealth);

    const auto ComponentOwner = GetOwner();
    if (ComponentOwner)
    {
        ComponentOwner->OnTakePointDamage.AddDynamic(this, &USTUHealthComponent::OnTakePointDamage);
        ComponentOwner->OnTakeRadialDamage.AddDynamic(this, &USTUHealthComponent::OnTakeRadialDamage);
    }
}

bool USTUHealthComponent::TryAddHealth(float HealthAmount)
{
    if (IsDead() || FMath::IsNearlyEqual(Health, MaxHealth)) return false;

    SetHealth(Health + HealthAmount);
    return true;
}

void USTUHealthComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation,
    UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
    ApplyDamage(GetModifiedDamage(DamagedActor, Damage, BoneName), InstigatedBy);
}

void USTUHealthComponent::OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin,
    FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
    ApplyDamage(Damage, InstigatedBy);
}

void USTUHealthComponent::ApplyDamage(float Damage, AController* InstigatedBy)
{
    if (Damage <= 0 || IsDead() || !GetWorld()) return;

    SetHealth(Health - Damage);
    GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);

    if (IsDead())
    {
        CountDeath(InstigatedBy);
        OnDeath.Broadcast();
        return;
    }

    if (IsAutoHealOn)
    {
        GetWorld()->GetTimerManager().SetTimer(
            HealTimerHandle, this, &USTUHealthComponent::HealUpdate, HealUpdateTime, true, HealDelayTime);
    }

    PlayCameraShake();
    ReportDamageEvent(Damage, InstigatedBy);
}

float USTUHealthComponent::GetModifiedDamage(AActor* DamagedActor, float Damage, const FName& BoneName)
{
    const auto Character = Cast<ACharacter>(DamagedActor);
    if (!Character || !Character->GetMesh() || !Character->GetMesh()->GetBodyInstance(BoneName)) return Damage;

    const auto DamagedBonePhysMat = Character->GetMesh()->GetBodyInstance(BoneName)->GetSimplePhysicalMaterial();
    if (!DamagedBonePhysMat || !DamageModifiers.Contains(DamagedBonePhysMat)) return Damage;

    return DamageModifiers[DamagedBonePhysMat] ? Damage * DamageModifiers[DamagedBonePhysMat] : MaxHealth;
}

void USTUHealthComponent::ReportDamageEvent(float Damage, AController* InstigatedBy)
{
    if (!GetWorld() || !GetOwner() || !InstigatedBy || !InstigatedBy->GetPawn()) return;
    UAISense_Damage::ReportDamageEvent(GetWorld(),   //
        GetOwner(),                                  //
        InstigatedBy->GetPawn(),                     //
        Damage,                                      //
        InstigatedBy->GetPawn()->GetActorLocation(), //
        GetOwner()->GetActorLocation());
}

void USTUHealthComponent::HealUpdate()
{
    if (FMath::IsNearlyEqual(Health, MaxHealth) || IsDead()) return;

    SetHealth(Health + HealModifier);

    if (IsAutoHealOn && Health == MaxHealth && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    }
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
    Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    OnHealthChange.Broadcast(Health);
}

void USTUHealthComponent::PlayCameraShake()
{
    if (IsDead()) return;
    const auto Player = Cast<APawn>(GetOwner());
    if (!Player) return;

    const auto Controller = Player->GetController<APlayerController>();
    if (!Controller || !Controller->PlayerCameraManager) return;

    Controller->PlayerCameraManager->StartCameraShake(CameraShake);
}

void USTUHealthComponent::CountDeath(AController* KillerController) const
{
    if (!GetWorld()) return;

    const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
    if (!GameMode) return;

    const auto Player = Cast<APawn>(GetOwner());

    GameMode->CountKill(KillerController, Player ? Player->Controller : nullptr);
}
