// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "PlayerCharacter.h"
#define ECC_WeaponTrace ECollisionChannel::ECC_GameTraceChannel1

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{

	Super::BeginPlay();

	AimDot = FindComponentByClass<UDecalComponent>();
	GunMesh = FindComponentByClass<USkeletalMeshComponent>();
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(Player)) //Check if player is valid (not null/nullptr and not about to be destroyed)
	{

		return;

	}

	if (Player->isShootingC) //If the player is shooting, do a early return since shooting logic is handled in the "Shoot" function, and the rest of the code isn't beneficial for player or performance if the player is shooting
	{

		return;

	}

	if (IsValid(AimDot)) //Checks if "AimDot" is valid in same manner as "player"  
	{

		AimDot->SetVisibility(Player->isAimingC); //Sets the visibility of the dot to be the same as the players aiming state, so it's only visible when the player is actively aiming

	}
	else 
	{

		return;

	}

	if (!Player->isAimingC) //Early return if the player isn't aiming, since the dot most likely wouldn't be helpful, and only take up performance needlessly then
	{

		return;

	}
	
	AimDotMovement(); //Calls AimDotMovement that moves the aiming dot to a updated position every frame

}

void AWeapon::Shoot()
{

	ammo--; //Reduces ammo-count by 1 when shooting

	Player->UpdateAmmo.Broadcast(); //Broadcasts custom event dispatcher (signals HUD to update ammo-count)

	FHitResult hitResult; //Hit result struct that is a "out" parameter of the line trace, provides information about hit results

	if (!DoLineTrace(hitResult)) //Calls the helper function "DoLineTrace" that does a line trace and returns whether it hit anything, and "fills" the hitResult with information about the first blocking hit object - if any
	{

		return;

	}

	if (hitResult.bBlockingHit) //Proceeds if a blocking hit was detected
	{

		AActor* hitActor = hitResult.GetActor(); //Gets the actor that was hit by the line trace, used for checking if it implements the "IShootable" interface and applying damage if it does

		if (hitActor->Implements<UIShootable>()) //Checks if the hit actor implements the "IShootable" interface, which is used for actors that can be damaged by shooting them
		{
		
			int multiplier;
			if (hitResult.BoneName == FName("upper_neck"))
			{

				UE_LOG(LogTemp, Warning, TEXT("Enemy got hit in upper neck/head!"));
				multiplier = HeadShotMultiplier;

			}
			else if (hitResult.BoneName == FName("lower_neck") || hitResult.BoneName == FName("root"))
			{

				UE_LOG(LogTemp, Warning, TEXT("Enemy got hit in lower neck/body!"));
				multiplier = BodyShotMultiplier;

			}
			else
			{

				UE_LOG(LogTemp, Warning, TEXT("Enemy got hit in %s"), *hitResult.BoneName.ToString());
				multiplier = ShotMultiplier;

			}

			IIShootable::Execute_GetHit(hitActor, damage * multiplier, hitResult.ImpactPoint, hitResult.ImpactNormal.Rotation()); //Calls the "GetHit" function from the "IShootable" interface on the hit actor, and passes in the damage multiplied by the appropriate multiplier based on where the enemy was hit (head, body, or other)

			//Apply VFX logic here or in GetHit function of the hit actor (requires method to take a FVector & FRotator or FTransform or FHitResult as parameter to get the location and rotation for the VFX)

		}
		else 
		{

			//Hit surface logic -> spawn niagara effect at the hit location, with a rotation based on the normal of the hit surface, so it looks like it's hitting the surface instead of being sideways
			if (hitResult.bBlockingHit && HitSpark)
			{

				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					HitSpark,
					hitResult.ImpactPoint,
					hitResult.ImpactNormal.Rotation(),
					FVector(1.0f),
					true,
					true,
					ENCPoolMethod::AutoRelease,
					true
				);

			}
		
		}

	}

}

void AWeapon::AimDotMovement()
{
	
	FHitResult hitResult; //Hit result struct that is a "out" parameter of the line trace, provides information about hit results

	if (!DoLineTrace(hitResult)) //Calls the helper function "DoLineTrace" that does a line trace and returns whether it hit anything, and "fills" the hitResult with information about the first blocking hit object - if any
	{

		return;

	}

	if (hitResult.bBlockingHit) //Proceeds if a blocking hit was detected
	{

		FVector impactPoint = hitResult.ImpactPoint; //Gets the point where the line trace hit the blocking object, used for setting the location of the aiming dot (where the barrel is pointed at)
		FRotator impactNormalRotation = UKismetMathLibrary::MakeRotFromZ(hitResult.ImpactNormal); //Gets the normal of the hit surface, and converts it to a rotator that can be used for setting the rotation of the aiming dot (so it faces the surface it's hitting) combined with a rotation offset to make it face the player instead of being sideways
		FRotator rotationOffset = FRotator(0.0f, 0.0f, 90.0f); //Rotation offset
		FRotator finalRotation = UKismetMathLibrary::ComposeRotators(impactNormalRotation, rotationOffset); //Combines rotation to get the final rotation for the aimdot

		AimDot->SetWorldLocationAndRotation(impactPoint, finalRotation); //Sets location and rotation from the above calculations

		float newScale = UKismetMathLibrary::FClamp(hitResult.Distance * 0.006f, 1.0f, 10.0f); //Clamps the distance from the player to the hit point between 1 and 10 and multiplies it to set the scale of the aimdot, so it gets bigger the further away it is, to make it more visible at range
		AimDot->SetWorldScale3D(FVector(newScale, newScale, newScale)); //Sets the scale of the aimdot to the above calculation

	}

}

bool AWeapon::DoLineTrace(FHitResult& result)
{

	if (!IsValid(GunMesh))
	{

		return false;

	}

	//Line trace point calculations
	FTransform socketTransform = GunMesh->GetSocketTransform(TEXT("barrel"), RTS_World); //Gets the transform of the "barrel" socket on the weapons skeletal mesh, which is then used as the starting point for a line trace that simulates a bullet being fired from the gun
	FVector socketLocation = socketTransform.GetLocation(); //Vector3 location of the socket = starting point of the line trace
	FVector endpoint = socketLocation + (socketTransform.GetRotation().GetForwardVector() * maxDistance); //Forward direction of the socket, multiiplied by maxDistance and added to the socket location, to get the endpoint of the line trace

	//Line trace variables and parameters
	FCollisionQueryParams parameters; //Sets parameters for the line trace -> to ignore the weapon and player to avoid mistakenly hitting the player self or the weapon when shooting
	parameters.AddIgnoredActor(this);
	if (GetOwner())
	{
		parameters.AddIgnoredActor(GetOwner());
	}

	return GetWorld()->LineTraceSingleByChannel(result, socketLocation, endpoint, ECC_WeaponTrace, parameters); //Line trace (raycast) that simulates a bullet being fired, and detects object that block the bullet (Channel called "WeaponTrace" that is set up in project settings)

}