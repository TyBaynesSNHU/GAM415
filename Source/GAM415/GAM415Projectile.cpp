// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM415Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AGAM415Projectile::AGAM415Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGAM415Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>("BallMesh");


	// Set as root component
	RootComponent = CollisionComp;

	ballMesh->SetupAttachment(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

//Begin play
void AGAM415Projectile::BeginPlay()
{
	Super::BeginPlay();


	float ranNumX = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
	float ranNumY = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
	float ranNumZ = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);

//Establish variables for colors upon creation
	RanColor = FLinearColor(ranNumX, ranNumY, ranNumZ, 1.0f);

	if (ProjMat && ballMesh)
	{
		ProjDMI = UMaterialInstanceDynamic::Create(ProjMat, this);
		if (ProjDMI)
		{
			ProjDMI->SetVectorParameterValue("Color", RanColor);
			ballMesh->SetMaterial(0, ProjDMI);
		}
	}
}

void AGAM415Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());	

	}

	if ((decalMat) && (OtherActor != nullptr))
	{
		//Assign Niagara system to particleComp. colorP material(set in engine details window). After much debugging when the particles wouldn't spawn, I determined that the component was getting destroyed too early for the attachment to work properly. I changed it to spawn at location and have it effectively set to destroy itself
		UNiagaraComponent* particleComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), colorP, Hit.Location, Hit.Normal.Rotation(), FVector(1.f), true, true);
		particleComp->SetNiagaraVariableLinearColor(FString("RandColor"), RanColor);

		float frameNum = UKismetMathLibrary::RandomFloatInRange(0.f, 3.f);


		//Places decal in world based on these metrics: world location, material, size is random between 20 and 40 units, grabs the hit location, rotates the normals of the decal to face the camera, infinite lifespan
		auto Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), decalMat, FVector(UKismetMathLibrary::RandomFloatInRange(20.f, 40.f)), Hit.Location, Hit.Normal.Rotation(), 0.f);
		auto MatInstance = Decal->CreateDynamicMaterialInstance();

		MatInstance->SetVectorParameterValue("Color", RanColor);
		MatInstance->SetScalarParameterValue("Frames", frameNum);
	}
	Destroy();
	
}