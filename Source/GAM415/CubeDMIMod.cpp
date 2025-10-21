// Fill out your copyright notice in the Description page of Project Settings.

#include "GAM415Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "CubeDMIMod.h"

// Sets default values
ACubeDMIMod::ACubeDMIMod()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Creating subobjects
	boxComp = CreateDefaultSubobject<UBoxComponent>("Box Component");
	cubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("Cube Mesh");
	//Defining boxComp as Root
	RootComponent = boxComp;
	cubeMesh->SetupAttachment(boxComp);

}

// Called when the game starts or when spawned
void ACubeDMIMod::BeginPlay()
{
	Super::BeginPlay();

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ACubeDMIMod::OnOverlapBegin);

	//If baseMat is valid, create DMI
	if (baseMat)
	{
		dmiMat = UMaterialInstanceDynamic::Create(baseMat, this);
	}
	//If  cubeMesh is valid, set the DMI
	if (cubeMesh)
	{
		cubeMesh->SetMaterial(0, dmiMat);
	}
}

// Called every frame
void ACubeDMIMod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACubeDMIMod::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Cast to player character. 
	AGAM415Character* overlappedActor = Cast<AGAM415Character>(OtherActor);
	//When overlapped(if overlappedActor is valid)
	if (overlappedActor)
	{
		//Create random numbers between 0.f and 1.f for each axis of color
		float ranNumX = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		float ranNumY = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		float ranNumZ = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		float ranOpacity = UKismetMathLibrary::RandomFloatInRange(0.25f, 1.f);

		FVector4 randColor = FVector4(ranNumX, ranNumY, ranNumZ, 1.f);
		if (dmiMat)
		{
			//Reusing ranNumX and Z for Darkness and Opacity because I can
			dmiMat->SetVectorParameterValue("Color", randColor);
			dmiMat->SetScalarParameterValue("Darkness", ranNumX);
			dmiMat->SetScalarParameterValue("Opacity", ranOpacity);
		}
	}
}