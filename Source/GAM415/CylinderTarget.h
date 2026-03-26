#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CylinderTarget.generated.h"

UCLASS()
class GAM415_API ACylinderTarget : public AActor
{
    GENERATED_BODY()

public:
    ACylinderTarget();

protected:
    virtual void BeginPlay() override;

public:
    // The cylinder mesh
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CylinderMesh;

    // Material to apply (needs a "Color" vector parameter, same as your projectile/gun)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
    UMaterialInterface* CylinderMat;

    UPROPERTY(VisibleAnywhere, Category = "Color")
    UMaterialInstanceDynamic* CylinderDMI;

    // The color this cylinder must be hit by to die
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Color")
    FLinearColor TargetColor;

    // The five colors to pick from — populate in Blueprint or here
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
    TArray<FLinearColor> ColorPalette;

    // Decal material for wrong-color hits (reuse your Splat1_MAT)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decal")
    UMaterialInterface* DecalMat;

    // Called by the projectile on hit
    UFUNCTION(BlueprintCallable, Category = "Color")
    void HandleProjectileHit(FLinearColor ProjectileColor, const FHitResult& Hit);

private:
    void PickRandomColor();

    // Tolerance for color comparison (palette colors so this can be tight)
    float ColorTolerance = 0.05f;
};