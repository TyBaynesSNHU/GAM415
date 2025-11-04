// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcPlane.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AProcPlane::AProcPlane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	procMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Proc Mesh");

	if (PlaneMat)
	{
		procMesh->SetMaterial(0, PlaneMat);
	}


//	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PlaneMatFinder(TEXT("/Script/Engine.Material'/Game/Procedural/Plane_MAT.Plane_MAT'"));
//	if (PlaneMatFinder.Succeeded())
//	{
//		PlaneMat = PlaneMatFinder.Object;
//	}
}

// Called when the game starts or when spawned
void AProcPlane::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProcPlane::PostActorCreated()
{
	Super::PostActorCreated();
	CreateMesh();

}

void AProcPlane::PostLoad()
{
	Super::PostLoad();
	CreateMesh();
}

// Called every frame
void AProcPlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProcPlane::CreateMesh()
{
	//Creates parameters to edit within the engine that creates the procedural mesh
	procMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

