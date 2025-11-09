// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "GAM415Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	boxComp = CreateDefaultSubobject<UBoxComponent>("Box comp");
	sceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture");
	rootArrow = CreateDefaultSubobject<UArrowComponent>("Root Arrow");

	//Attachments
	RootComponent = boxComp;//Root comp is the box comp

	mesh->SetupAttachment(boxComp);//mesh is attach to the box comp
	sceneCapture->SetupAttachment(mesh);//sceneCapture is connected to the mesh bone
	rootArrow->SetupAttachment(RootComponent);

	//Disable collision as player walks through the mesh.
	mesh->SetCollisionResponseToAllChannels(ECR_Ignore);



}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();

	//Overlap event
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);
	mesh->SetHiddenInSceneCapture(true);
	

	if (mat)
	{
		mesh->SetMaterial(0, mat);
	}
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePortals();

}

void APortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGAM415Character* playerChar = Cast<AGAM415Character>(OtherActor);
	//If cast to playerChar is valid->if otherPortal is valid->if player is not actively teleporting(to stop loops/do once)
	if (playerChar)
	{
		if (OtherPortal)
		{
			if (!playerChar->isTeleporting)
			{
				//Casts to playerChar and sets the bool to true/Grabs the location of the OtherPortal and sets the playerChar's location to that vector
				playerChar->isTeleporting = true;
				FVector loc = OtherPortal->rootArrow->GetComponentLocation();
				playerChar->SetActorLocation(loc);

				FTimerHandle TimerHandle; //initialize TimerHandle variable as Timerhandle function
				FTimerDelegate TimerDelegate; //Initialize TimerDelegate variable as TimerDelegate fucntion
				TimerDelegate.BindUFunction(this, "SetBool", playerChar);//functions arguments: this= this class, set bool function, cast to playerchar. --Sets the playerChar isTeleporting bool to false.
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1, false);//Function arguments: timerhandle/delegate, how many seconds before it gets called, does not loop

			}
		}
	}
}

void APortal::SetBool(AGAM415Character* playerChar)
{//Sets bool so the player can teleport again
	if (playerChar)
	{
		playerChar->isTeleporting = false;
	}
}

void APortal::UpdatePortals()
{//Grabbing location/rotation of the camera component of the portal to apply to the playerChar
	FVector Location = this->GetActorLocation() - OtherPortal->GetActorLocation();
	FVector camLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentLocation();
	FRotator camRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentRotation();
	FVector CombinedLocation = camLocation + Location;//Combined location is the two location vectors combined

	sceneCapture->SetWorldLocationAndRotation(CombinedLocation, camRotation);//Moves the sceneCapture component to these values
}

