// Shooter Game. All Rights, If Any, Reserved.

#include "Player/STUBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/STUCharacterMovementComponent.h"
#include "Components/STUHealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/STUAIWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "STUBaseCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(BaseCharacterLog, All, All)

ASTUBaseCharacter::ASTUBaseCharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit.SetDefaultSubobjectClass<USTUCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    HealthComponent = CreateDefaultSubobject<USTUHealthComponent>("HealthComponent");
    WeaponComponent = CreateDefaultSubobject<USTUWeaponComponent>("WeaponComponent");

    // HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>("TextRenderComponent");
    // HealthTextComponent->SetupAttachment(GetRootComponent());
    // HealthTextComponent->SetOwnerNoSee(true);
}

void ASTUBaseCharacter::TurnOff()
{
    if (WeaponComponent)
    {
        WeaponComponent->AimZoom(false);
        WeaponComponent->StopFire();
    }

    Super::TurnOff();
}

void ASTUBaseCharacter::Reset()
{
    if (WeaponComponent)
    {
        WeaponComponent->AimZoom(false);
        WeaponComponent->StopFire();
    }

    Super::Reset();
}

void ASTUBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    // check works only in debug and dev builds
    check(GetMesh());
    check(HealthComponent);
    // check(HealthTextComponent);
    check(GetCharacterMovement());

    OnHealthChange(HealthComponent->GetHealth());
    HealthComponent->OnDeath.AddUObject(this, &ASTUBaseCharacter::OnDeath);
    HealthComponent->OnHealthChange.AddUObject(this, &ASTUBaseCharacter::OnHealthChange);

    LandedDelegate.AddDynamic(this, &ASTUBaseCharacter::OnGroundLanded);
}

void ASTUBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float ASTUBaseCharacter::GetMovementDirection() const
{
    const auto VelocityVectorNorm = GetVelocity().GetSafeNormal();

    if (VelocityVectorNorm.IsZero()) return 0.0f;

    const auto ForwardVector = GetActorForwardVector();
    const auto AngleBetween = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVector, VelocityVectorNorm)));
    const auto CrossProduct = FVector::CrossProduct(ForwardVector, VelocityVectorNorm);
    return CrossProduct.IsZero() ? AngleBetween : AngleBetween * FMath::Sign(CrossProduct.Z);
}

void ASTUBaseCharacter::OnGroundLanded(const FHitResult& Hit)
{
    const auto FallVelocityZ = -GetVelocity().Z;

    if (FallVelocityZ < LandedDamageVelocity.X) return;

    FPointDamageEvent PointDamageEvent;
    const auto FinalDamage = FMath::GetMappedRangeValueClamped(LandedDamageVelocity, LandedDamage, FallVelocityZ);
    TakeDamage(FinalDamage, PointDamageEvent, nullptr, nullptr);
}

void ASTUBaseCharacter::OnHealthChange(float Health)
{
    // HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Health)));
}

void ASTUBaseCharacter::OnDeath()
{
    UE_LOG(BaseCharacterLog, Display, TEXT("Character %s is dead"), *GetName());

    // PlayAnimMontage(DeathAnimMontage);
    GetCharacterMovement()->DisableMovement();

    SetLifeSpan(LifeSpanOnDeath);

    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponComponent->StopFire();

    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());

    // ragdoll
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetSimulatePhysics(true);
}

void ASTUBaseCharacter::SetTeamMarkerColor(const FLinearColor& Color)
{
    const auto MaterialInstance = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
    if (!MaterialInstance) return;

    MaterialInstance->SetVectorParameterValue(TeamMarkerMatColorName, Color);
}
