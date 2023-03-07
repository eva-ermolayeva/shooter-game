// Shooter Game. All Rights, If Any, Reserved.

#include "Player/STUPlayerCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/STUWeaponComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

ASTUPlayerCharacter::ASTUPlayerCharacter(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->bUsePawnControlRotation = true;
    // SpringArmComponent->SocketOffset = FVector(0.0f, 0.0f, 80.0f);

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(SpringArmComponent);

    CameraCollisionComponent = CreateDefaultSubobject<USphereComponent>("CameraCollisionComponent");
    CameraCollisionComponent->SetupAttachment(CameraComponent);
    CameraCollisionComponent->SetSphereRadius(10.0f);
    CameraCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

    BehindCollisionBoxComponent = CreateDefaultSubobject<UBoxComponent>("BehindCollisionBoxComponent");
    BehindCollisionBoxComponent->SetupAttachment(CameraComponent);
}

void ASTUPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    check(CameraCollisionComponent);

    CameraCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASTUPlayerCharacter::OnCameraCollisionBeginOverlap);
    CameraCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ASTUPlayerCharacter::OnCameraCollisionEndOverlap);
}

void ASTUPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);
    check(WeaponComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ASTUPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASTUPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis("LookUp", this, &ASTUPlayerCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("TurnAround", this, &ASTUPlayerCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTUPlayerCharacter::Jump);
    PlayerInputComponent->BindAxis("Run", this, &ASTUPlayerCharacter::Run);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &USTUWeaponComponent::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &USTUWeaponComponent::StopFire);
    PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, WeaponComponent, &USTUWeaponComponent::PrevWeapon);
    PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, WeaponComponent, &USTUWeaponComponent::NextWeapon);
    PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &USTUWeaponComponent::Reload);

    DECLARE_DELEGATE_OneParam(FAimZoomInputSignature, bool);
    PlayerInputComponent->BindAction<FAimZoomInputSignature>("AimZoom", IE_Pressed, WeaponComponent, &USTUWeaponComponent::AimZoom, true);
    PlayerInputComponent->BindAction<FAimZoomInputSignature>("AimZoom", IE_Released, WeaponComponent, &USTUWeaponComponent::AimZoom, false);
}

void ASTUPlayerCharacter::OnDeath()
{
    WeaponComponent->AimZoom(false);

    Super::OnDeath();

    if (Controller)
    {
        Controller->ChangeState(NAME_Spectating);
    }
}

bool ASTUPlayerCharacter::IsObstacleOnRight() const
{
    if (!GetWorld()) return false;

    const auto WeaponSocketLocation = GetMesh()->GetSocketLocation(WeaponSocket);

    FCollisionShape CollisionBox = FCollisionShape::MakeBox(RightCollisionBoxSize);
    FVector CollisionBoxCenter =
        WeaponSocketLocation + 0.5f * RightCollisionBoxSize.Y * GetActorRightVector() + 20.0f * GetActorForwardVector();

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    return GetWorld()->SweepSingleByChannel(HitResult, CollisionBoxCenter, CollisionBoxCenter, FQuat(GetViewRotation()),
        ECollisionChannel::ECC_Visibility, CollisionBox, CollisionParams);
}

TArray<AActor*> ASTUPlayerCharacter::GetOverlappingActorsBehind() const
{
    TArray<AActor*> OverlappingActors;
    BehindCollisionBoxComponent->GetOverlappingActors(OverlappingActors);

    return OverlappingActors;
}

void ASTUPlayerCharacter::MoveForward(float Amount)
{
    if (Amount == 0) return;
    AddMovementInput(GetActorForwardVector(), Amount);
}

void ASTUPlayerCharacter::MoveRight(float Amount)
{
    if (Amount == 0) return;
    AddMovementInput(GetActorRightVector(), Amount);
}

void ASTUPlayerCharacter::Run(float Amount)
{
    bWantToRun = Amount != 0;
}

void ASTUPlayerCharacter::CheckCameraOverlap()
{
    const auto HideMesh = CameraCollisionComponent->IsOverlappingComponent(GetCapsuleComponent());
    GetMesh()->SetOwnerNoSee(HideMesh);

    TArray<USceneComponent*> MeshChildren;
    GetMesh()->GetChildrenComponents(true, MeshChildren);

    for (auto MeshChild : MeshChildren)
    {
        if (const auto MeshChildGeometry = Cast<UPrimitiveComponent>(MeshChild))
        {
            MeshChildGeometry->SetOwnerNoSee(HideMesh);
        }
    }
}

void ASTUPlayerCharacter::OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    CheckCameraOverlap();
}

void ASTUPlayerCharacter::OnCameraCollisionEndOverlap(
    UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    CheckCameraOverlap();
}
