// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "GAM415Character.h"
#include "GAM415Projectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			// Spawn the projectile at the muzzle
			AGAM415Projectile* Projectile = World->SpawnActorDeferred<AGAM415Projectile>(ProjectileClass, FTransform(SpawnRotation, SpawnLocation), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding);

			if (Projectile)
			{
				Projectile->bUseOverrideColor = true;
				Projectile->OverrideColor = CurrentColor;
				UGameplayStatics::FinishSpawningActor(Projectile, FTransform(SpawnRotation, SpawnLocation));
			}
			PickNewColor();
		}
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::AttachWeapon(AGAM415Character* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	//Init pallette and pick color
	if (ColorPalette.Num() == 0)
	{
		ColorPalette.Add(FLinearColor(1.f, 0.f, 0.f, 1.f)); // Red
		ColorPalette.Add(FLinearColor(0.f, 1.f, 0.f, 1.f)); //Green
		ColorPalette.Add(FLinearColor(0.f, 0.f, 1.f, 1.f)); //Blue
		ColorPalette.Add(FLinearColor(1.f, 1.f, 0.f, 1.f)); //Yellow
		ColorPalette.Add(FLinearColor(1.f, 0.f, 1.f, 1.f)); //Magenta
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			if (WeaponMat)
			{
				WeaponDMI = UMaterialInstanceDynamic::Create(WeaponMat, this);
				//APply to all material slots (potential fix to a glitch)
				for (int32 i = 0; i < GetNumMaterials(); i++)
				{
					SetMaterial(i, WeaponDMI);
				}
			}

			PickNewColor();
		});

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}
}

void UTP_WeaponComponent::PickNewColor()
{
	if (ColorPalette.Num() == 0) return;

	int32 RandIndex = FMath::RandRange(0, ColorPalette.Num() - 1);
	CurrentColor = ColorPalette[RandIndex];

	//Apply
	if (WeaponDMI)
	{
		WeaponDMI->SetVectorParameterValue("Color", CurrentColor);

		//Potential glitch fix
		for (int32 i = 0; i < GetNumMaterials(); i++)
		{
			SetMaterial(i, WeaponDMI);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}