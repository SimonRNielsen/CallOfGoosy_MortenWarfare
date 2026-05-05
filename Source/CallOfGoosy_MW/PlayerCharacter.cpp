// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{

	Super::BeginPlay();

	springArm = FindComponentByClass<USpringArmComponent>(); //Finds the spring arm component in the characters components and assigns it to the springArm variable
	burnEffect = FindComponentByClass<UNiagaraComponent>(); //Finds the niagara component in the characters components and assigns it to the burnEffect variable
	TArray<USkeletalMeshComponent*> meshes; //Array to find all skeletal mesh components on the player (player has multiple -> metahuman)
	GetComponents<USkeletalMeshComponent>(meshes); //Fills array with all skeletal meshes found

	for (USkeletalMeshComponent* mesh : meshes)
	{

		if (mesh->GetName() == "CharacterMesh0")
		{

			playerMesh = mesh; //Assigns the skeletal mesh with the name "CharacterMesh0" -> primary animated skeletal mesh for player
			break; //Early break if the mesh is found (should be first entry -> root)

		}

	}

	DoGetWeapon(); //Calls the function to spawn and attach the weapon to the player

	if (IsValid(HUDClass)) //Only tries to create UI if the class has been set in the editor
	{

		HUD = CreateWidget<UUserWidget>(GetWorld(), HUDClass); //"Spawns" and sets HUD/UI Widget

		if (IsValid(HUD))
		{

			HUD->AddToViewport(); //Sets UI to be displayed on camera

		}

	}

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	TookDamageThisTick = false;
	TimeSinceLastSprint += DeltaTime;

	if (TimeSinceLastSprint >= TimeBeforeRegainingStamina)
	{

		Stamina = UKismetMathLibrary::FClamp(Stamina + StaminaRegenRate, 0.0f, 1.0f);

	}

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacter::DoGetWeapon()
{

	if (weaponClass && IsValid(playerMesh)) //Checks for required variables to spawn and attach weapon to player
	{

		FTransform playerTransform = playerMesh->GetSocketTransform(TEXT("gunSocket"), RTS_World); //Gets the transform for the right hand "gunSocket" that will grab the handle of the gun
		FActorSpawnParameters parameters; //Used for setting additional spawnparameters
		parameters.Owner = this;
		parameters.Instigator = GetInstigator();

		weaponC = GetWorld()->SpawnActor<AWeapon>(weaponClass, playerTransform, parameters); //Sets the pointer while spawning the weapon at the same time

		if (IsValid(weaponC)) //Failsafe in case the spawning fails
		{

			weaponC->Player = this; //Sets the Player reference of the weapon to the creator
			hasWeaponC = weaponC->AttachToComponent(playerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("gunSocket")); //Attaches the weapon to the "gunSocket" on the hand, "welding" them together
			weaponC->GunMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.8f)); //Scales the weapon down to a more fitting size compared to player

		}

	}

}

void APlayerCharacter::DoShoot()
{

	if (GetCharacterMovement()->IsFalling() || IsSprinting || IsInteracting)
	{

		return;

	}

	if (hasWeaponC && isAimingC && IsValid(weaponC) && IsAlive) //Checks if the player has a weapon, is aiming, and that the weapon is set and exists
	{

		if (!isReloadingC && !isShootingC && weaponC->ammo > 0) //Checks if the player is currently reloading or shooting, and that the weapon has ammo, if no ammo or already doing another action with the weapon, it won't allow another shot to be fired
		{

			isShootingC = true; //Sets shooting state to true, which is used for animations and blocking firing until animation is finished (reset by animation notify)

			weaponC->Shoot(); //Calls the weapons Shoot function that handles raycasting and applying hit logic (damage, niagara effects)

		}

	}

}

void APlayerCharacter::DoAim(float alpha)
{

	if (!IsValid(springArm) || !hasWeaponC)
	{

		return;

	}

	if (!IsAlive || IsInteracting)
	{

		alpha = 0.0f;

	}

	springArm->TargetArmLength = FMath::Lerp(cameraboom_Zoomed_Out, cameraboom_Zoomed_In, alpha); //Lerps between the two values for a zoom in/out effect

	/* Deprecated to one-liner below
	double newSocketY = FMath::Lerp(socketY_Zoomed_Out, socketY_Zoomed_In, alpha); //Lerps between the two values for a over-the-shoulder effect
	double newSocketZ = FMath::Lerp(socketZ_Zoomed_Out, socketZ_Zoomed_In, alpha); //Same as above but for Z axis

	springArm->SocketOffset = FVector(0.0f, newSocketY, newSocketZ); //Sets the socket offset to the new values
	*/

	springArm->SocketOffset = FVector(0.0f, FMath::Lerp(socketY_Zoomed_Out, socketY_Zoomed_In, alpha), FMath::Lerp(socketZ_Zoomed_Out, socketZ_Zoomed_In, alpha)); //One-liner of the above calculations

	bUseControllerRotationYaw = isAimingC; //Determines whether the character should rotate with the controller when aiming

}

void APlayerCharacter::DoReload()
{

	if (IsValid(weaponC) && hasWeaponC && !isReloadingC && !isShootingC && IsAlive && !IsInteracting) //Checks if the player has a valid weapon and not already shooting or reloading
	{

		isReloadingC = true; //Sets reloading state to true, which triggers the reload animation and blocks other usage of the weapon until the reload is finished (reset by animation notify)

	}

}

void APlayerCharacter::BurnDamage(float timeOnFire, bool burning)
{

	FScopeLock Lock(&BurnLock); //Thread lock for entire function (unlocks when completed), prevents edge cases of new burn effect starting before a ending effect has finished (yes - it actually happened)

	if (!IsAlive)
	{

		UE_LOG(LogTemp, Warning, TEXT("Attempted burn damage on a player set as dead!"));
		return; //That which is dead, may never die

	}

	//Burn effect logic toggle
	if (IsValid(burnEffect))
	{

		if (isBurning && !burning)
		{

			burnEffect->Deactivate(); //Burn effect ending

		}
		else if (!isBurning && burning)
		{

			burnEffect->Activate(); //Starts burn effect

		}

		isBurning = burning; //Sets the burning state to the value of the "burning" parameter, which is used to toggle the VFX for burning

	}

	burnTime += timeOnFire; //Adds the time from blueprint timeline to the burnTime variable (used to determine when to take damage)

	bool update = false;
	bool makeAuchSound = false;

	while (burnTime >= tickInterval) //Applies burn damage comparable to the amount of ticks the player is on fire, and reduces health by "fireDamage" and timer 
	{

		burnTime -= tickInterval;
		health -= fireDamage;
		update = true;
		damageTaken += fireDamage;

		if (damageTaken >= damageBeforeComplaint)
		{

			damageTaken -= damageBeforeComplaint;
			makeAuchSound = true;

		}

	}

	if (update)
	{

		UpdateHealth.Broadcast(); //Broadcasts custom event dispatcher (signals HUD to update healthbar)

	}

	if (makeAuchSound)
	{

		AuchSound.Broadcast(); //Tells player to make auch sound because of amount of damage taken

	}

	if (health <= 0 && IsAlive) //Checks if the players health has reached 0 or below, and triggers death logic if so
	{

		IsAlive = false; //Blocks certain actions and checks
		PlayerDeath.Broadcast(); //Broadcasts custom event dispatcher (signals players death)
		burnTime = 0.0f; //Removes excess accumulated time so if pawn gets reset it will not get any "undeserved" damage

		if (IsValid(burnEffect))
		{

			burnEffect->Deactivate(); //Burn effect ending

		}

	}

}

void APlayerCharacter::ResetPlayer() //Reset function to reset all parameters for restarting
{

	if (IsValid(playerMesh))
	{

		playerMesh->SetVisibility(true, true);

	}

	IsAlive = true;
	health = maxHealth;

	if (IsValid(HUD))
	{

		UpdateHealth.Broadcast();

	}

	if (IsValid(burnEffect))
	{

		burnEffect->Deactivate();

	}

	isAimingC = false;
	isShootingC = false;
	isReloadingC = false;
	isBurning = false;
	IsInteracting = false;
	Stamina = 1.0f;

	DoAim(0.0f);

	if (IsValid(weaponC))
	{

		if (IsValid(weaponC->GunMesh))
		{

			hasWeaponC = true;
			weaponC->GunMesh->SetVisibility(true);
			weaponC->ammo = weaponC->maxAmmo;

			if (IsValid(HUD))
			{

				UpdateAmmo.Broadcast();

			}

		}

	}

}


void APlayerCharacter::Sprint(bool trySprint)
{

	if (trySprint)
	{

		if (Stamina > 0.0f && !isAimingC && !isReloadingC)
		{

			Stamina -= StaminaDrainRate;
			IsSprinting = trySprint;
			GetCharacterMovement()->MaxWalkSpeed = MaxMovespeedSprinting;
			TimeSinceLastSprint = 0.0f;

		}
		else
		{

			IsSprinting = false;
			GetCharacterMovement()->MaxWalkSpeed = MaxMovespeedWalking;

		}

	}
	else
	{

		IsSprinting = trySprint;
		GetCharacterMovement()->MaxWalkSpeed = MaxMovespeedWalking;

	}

}


void APlayerCharacter::StartFire_Implementation()
{

	StartFire.Broadcast();

}


void APlayerCharacter::Interact()
{

	if (IsValid(Interactable) && !GetCharacterMovement()->IsFalling())
	{

		if (Interactable->Implements<UIInteractable>())
		{

			IIInteractable::Execute_Interact(Interactable, this);
			IsInteracting = true;

		}

	}

}