// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinProc.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"




// Sets default values
APerlinProc::APerlinProc()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Procedural Mesh");
	ProcMesh->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void APerlinProc::BeginPlay()
{
	Super::BeginPlay();

	CreateVertices();
	CreateTriangles();
	ProcMesh->CreateMeshSection(SectionID, Vertices, Triangles, Normals, UV0, VertexColors, TArray<FProcMeshTangent>(), true);
	ProcMesh->SetMaterial(0, Mat);
	
}

void APerlinProc::AlterMesh(FVector impactPoint)
{
	//Subtracts the impactPoint from the vertices given the trace from the actors location(procMesh)
	//Sets teh vertices of the procMesh to whatever the Depth value is by the radius value, making a hole
	for (int i = 0; i < Vertices.Num(); i++)
	{
		FVector tempVector = impactPoint - this->GetActorLocation();
		if (FVector(Vertices[i] - tempVector).Size() < radius)
		{
			Vertices[i] = Vertices[i] - Depth;
			ProcMesh->UpdateMeshSection(SectionID, Vertices, Normals, UV0, VertexColors, TArray<FProcMeshTangent>());
		}
	}
}

void APerlinProc::CreateVertices()
{


	//Nested for-loop that will create vertices until the amount created equals XSize and YSize
	for (int x = 0; x <= XSize; x++)
	{
		for (int y = 0; y <= YSize; y++)
		{
			float z = FMath::PerlinNoise2D(FVector2D(x * NoiseScale + 0.1, y * NoiseScale + 0.1))* ZMultiplier;//Uses Perlin noise to generate a random height map on Z
			GEngine->AddOnScreenDebugMessage(-1, 999.0f, FColor::Yellow, FString::Printf(TEXT("Z %f"), z));//Debug message to show Z value
			Vertices.Add(FVector(x * Scale, y * Scale, z));
			UV0.Add(FVector2D(x * UVScale, y * UVScale));
		}
	}
}

void APerlinProc::CreateTriangles()
{
	int Vertex = 0;
	//Increments vertices for drawing the triangles
	for (int x = 0; x < XSize; x++)
	{
		for (int y = 0; y < YSize; y++)
		{
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 1);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 2);
			Triangles.Add(Vertex + YSize + 1);
			Vertex++;
		}
		Vertex++;
	}
}

// Called every frame
void APerlinProc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

