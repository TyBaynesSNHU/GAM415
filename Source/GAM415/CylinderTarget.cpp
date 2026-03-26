#include "CylinderTarget.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ACylinderTarget::ACylinderTarget()
{
    PrimaryActorTick.bCanEverTick = false;

    CylinderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh"));
    RootComponent = CylinderMesh;

    // Load the engine's default cylinder mesh automatically
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(
        TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
    if (MeshFinder.Succeeded())
    {
        CylinderMesh->SetStaticMesh(MeshFinder.Object);
    }

    // Default palette — same five as the gun, can be overridden in Blueprint
    ColorPalette.Add(FLinearColor(1.f, 0.f, 0.f, 1.f)); // Red
    ColorPalette.Add(FLinearColor(0.f, 1.f, 0.f, 1.f)); // Green
    ColorPalette.Add(FLinearColor(0.f, 0.f, 1.f, 1.f)); // Blue
    ColorPalette.Add(FLinearColor(1.f, 1.f, 0.f, 1.f)); // Yellow
    ColorPalette.Add(FLinearColor(1.f, 0.f, 1.f, 1.f)); // Magenta
}

void ACylinderTarget::BeginPlay()
{
    Super::BeginPlay();
    PickRandomColor();
}

void ACylinderTarget::PickRandomColor()
{
    if (ColorPalette.Num() == 0) return;

    int32 RandIndex = FMath::RandRange(0, ColorPalette.Num() - 1);
    TargetColor = ColorPalette[RandIndex];

    if (CylinderMat)
    {
        CylinderDMI = UMaterialInstanceDynamic::Create(CylinderMat, this);
        CylinderMesh->SetMaterial(0, CylinderDMI);
        CylinderDMI->SetVectorParameterValue("Color", TargetColor);
    }
}

void ACylinderTarget::HandleProjectileHit(FLinearColor ProjectileColor, const FHitResult& Hit)
{
    // Compare RGB channels within tolerance
    bool bRMatch = FMath::IsNearlyEqual(ProjectileColor.R, TargetColor.R, ColorTolerance);
    bool bGMatch = FMath::IsNearlyEqual(ProjectileColor.G, TargetColor.G, ColorTolerance);
    bool bBMatch = FMath::IsNearlyEqual(ProjectileColor.B, TargetColor.B, ColorTolerance);

    if (bRMatch && bGMatch && bBMatch)
    {
        // Correct color — destroy immediately
        Destroy();
    }
    // Wrong color — decal sticks, handled by the projectile's existing OnHit logic
    // No reaction needed here, the projectile's OnHit already spawns the decal
}