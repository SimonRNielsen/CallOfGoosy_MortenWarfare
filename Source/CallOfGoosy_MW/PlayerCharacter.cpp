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

	if (!IsInvulnerable)
	{

		TookDamageThisTick = false; //Enables player to take damage again after having taken damage once last tick (gamedesign choice to not "melt" the player with multiple damage sources trying to apply damage. IsInvulnerable is a buff that is triggered from a powerup, and blocks this bool from being reset until the buff expires

	}

	TimeSinceLastSprint += DeltaTime; //Timer to track when the player last sprinted, which will then trigger stamina regeneration after a predetermined time (can also be set in the editor)

	if (TimeSinceLastSprint >= TimeBeforeRegainingStamina)
	{

		Stamina = UKismetMathLibrary::FClamp(Stamina + StaminaRegenRate, 0.0f, 1.0f); //Slowly regenerates the stamina, clamped between 0 (in edge cases where might've dropped way below 0, although that shouldn't be possible) and 1 (full stamina/100%)

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

	if (GetCharacterMovement()->IsFalling() || IsSprinting || IsInteracting || !IsAlive)
	{

		isShootingC = false; //Blocks shooting if player is dead or other conditions that might block the animation notification, and resets shooting state in case of edge cases where it might not have been reset properly (like getting killed while shooting and the animation notify not being able to reset it) - yes, that actually happened... causing the player to appear as if shooting himself in the face repeatedly until death animation finished
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

//Function that handles lerp effect for moving camera when aiming, alpha is a float given by a timeline in the blueprint that smoothly transitions between 0-1, and is reversed in the same manner when releasing the aim button
void APlayerCharacter::DoAim(float alpha)
{

	if (!IsValid(springArm) || !hasWeaponC)
	{

		return;

	}

	if (!IsAlive || IsInteracting)
	{

		bUseControllerRotationYaw = false;
		alpha = 0.0f; //If the player is dead or interacting, it forces the camera to be in the default position (not zoomed in, no over-the-shoulder) by setting alpha to 0, which is used for lerping between the two states of the camera

	}

	/* Deprecated to one-liner below - kept for readability
	double newSocketY = FMath::Lerp(socketY_Zoomed_Out, socketY_Zoomed_In, alpha); //Lerps between the two values for a over-the-shoulder effect
	double newSocketZ = FMath::Lerp(socketZ_Zoomed_Out, socketZ_Zoomed_In, alpha); //Same as above but for Z axis

	springArm->SocketOffset = FVector(0.0f, newSocketY, newSocketZ); //Sets the socket offset to the new values
	*/

	//
	springArm->SocketOffset = FVector(0.0f, FMath::Lerp(socketY_Zoomed_Out, socketY_Zoomed_In, alpha), FMath::Lerp(socketZ_Zoomed_Out, socketZ_Zoomed_In, alpha)); //One-liner of the above calculations
	springArm->TargetArmLength = FMath::Lerp(cameraboom_Zoomed_Out, cameraboom_Zoomed_In, alpha); //Lerps between the two values for a zoom in/out effect

	//Blocks player from changing character rotation while interacting by doing a early return before triggering the bool that toggles character rotation with controller (mouse)
	if (IsInteracting)
	{

		return;

	}

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

		//UE_LOG(LogTemp, Warning, TEXT("Attempted burn damage on a player set as dead!"));
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
		damageTaken += fireDamage; //Used to measure how much damage the player has taken so the damagesound doesn't "spam" every time the player takes damage

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

		IsAlive = false; //Blocks several actions and checks
		//PlayerDeath.Broadcast(); //Broadcasts custom event dispatcher (signals players death) - deprecated since implementation of death animation, it is now triggered by the animation notify instead
		Die(); //Calls the Die function that signals the blueprint event for death
		burnTime = 0.0f; //Removes excess accumulated time so if pawn gets reset it will not get any "undeserved" damage

		if (IsValid(burnEffect))
		{

			burnEffect->Deactivate(); //Burn effect ending

		}

	}

}

void APlayerCharacter::ResetPlayer() //Reset function to reset all parameters for restarting
{

	//Resets visibility of player mesh (in case it was hidden after death animation)
	if (IsValid(playerMesh))
	{

		playerMesh->SetVisibility(true, true);

	}

	//Resets health and alive state
	IsAlive = true;
	health = maxHealth;

	//Sets the players health correctly on the HUD after reset
	if (IsValid(HUD))
	{

		UpdateHealth.Broadcast();

	}

	//Stops visual burn effect
	if (IsValid(burnEffect))
	{

		burnEffect->Deactivate();

	}

	//Resetting all states and stamina to default
	isAimingC = false;
	isShootingC = false;
	isReloadingC = false;
	isBurning = false;
	IsInteracting = false;
	IsSprinting = false;
	Stamina = 1.0f;

	//Resetting movement mode and speed to default
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking); 
	GetCharacterMovement()->MaxWalkSpeed = MaxMovespeedWalking;

	//Resets camera to default position
	DoAim(0.0f); 

	//Resets weapon, visibility, ammo and animationstate
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

//Triggered by enhanced input action, sends a true when pressed and false when released
void APlayerCharacter::Sprint(bool trySprint)
{

	if (!IsAlive)
	{

		return;

	}

	if (trySprint)
	{

		if (Stamina > 0.0f && !isAimingC && !isReloadingC)
		{

			//Increases speed while sprinting (as long as the player has stamina and not doing incompatible actions like aiming or reloading), and starts draining stamina. Also resets the timer for stamina regeneration
			Stamina -= StaminaDrainRate;
			IsSprinting = trySprint;
			GetCharacterMovement()->MaxWalkSpeed = MaxMovespeedSprinting;
			TimeSinceLastSprint = 0.0f;

		}
		else
		{

			//Sets speed to walking if the player is out of stamina, or tries to sprint while aiming or reloading
			IsSprinting = false;
			GetCharacterMovement()->MaxWalkSpeed = MaxMovespeedWalking;

		}

	}
	else
	{

		//Untoggling sprint, setting speed back to normal
		IsSprinting = trySprint;
		GetCharacterMovement()->MaxWalkSpeed = MaxMovespeedWalking;

	}

}

//Signals event to start timeline for burn damage
void APlayerCharacter::StartFire_Implementation()
{

	StartFire.Broadcast();

}


void APlayerCharacter::Interact()
{

	if (!IsAlive)
	{

		return;

	}

	if (IsValid(Interactable) && !GetCharacterMovement()->IsFalling() && !isReloadingC)
	{

		if (Interactable->Implements<UIInteractable>())
		{

			//Triggers the interaction function on the interactable object, and sets states to block other actions, movement and animations while interacting - is reenabled from animation notify
			IIInteractable::Execute_Interact(Interactable, this);
			IsInteracting = true;
			isAimingC = false;
			isShootingC = false;
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();

		}

	}

}


void APlayerCharacter::Die_Implementation()
{

	//Death logic is handled in blueprint, this function is just used to signal the blueprint event for death

}