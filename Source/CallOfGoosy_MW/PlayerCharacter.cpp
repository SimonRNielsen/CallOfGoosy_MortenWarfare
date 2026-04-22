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
	camera = FindComponentByClass<UCameraComponent>(); //Finds the camera component in the characters components and assigns it to the camera variable
	burnEffect = FindComponentByClass<UNiagaraComponent>(); //Finds the niagara component in the characters components and assigns it to the burnEffect variable
	TArray<USkeletalMeshComponent*> meshes; //Array to find all skeletal mesh components on the player (player has multiple -> metahuman)
	GetComponents<USkeletalMeshComponent>(meshes); //Fills array with all skeletal meshes found

	for (USkeletalMeshComponent* mesh : meshes)
	{
		if (mesh->GetName() == "CharacterMesh0")
		{

			playerMesh = mesh; //Assigns the skeletal mesh with the name "CharacterMesh0" -> primary animated skeletal mesh for player
			break;

		}
	}

	DoGetWeapon(); //Calls the function to spawn and attach the weapon to the player

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

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

		FTransform playerTransform = playerMesh->GetSocketTransform(TEXT("gunSocket"), RTS_World);
		FActorSpawnParameters parameters;
		parameters.Owner = this;
		parameters.Instigator = GetInstigator();

		weaponC = GetWorld()->SpawnActor<AWeapon>(weaponClass, playerTransform, parameters);
		weaponC->Player = this;

		if (IsValid(weaponC))
		{

			hasWeaponC = weaponC->AttachToComponent(playerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("gunSocket"));
			weaponC->GunMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.8f));

		}

	}

}

void APlayerCharacter::DoShoot()
{

	if (hasWeaponC && isAimingC && IsValid(weaponC)) //Checks if the player has a weapon, is aiming, and that the weapon is set and exists
	{

		if (!isReloadingC && !isShootingC && weaponC->ammo > 0) //Checks if the player is currently reloading or shooting, and that the weapon has ammo, if no ammo or already doing another action with the weapon, it won't allow another shot to be fired
		{

			isShootingC = true; //Sets shooting state to true, which is used for animations and blocking firing until animation is finished (reset by animation notify)

			weaponC->ammo--; //Reduces ammo-count by 1 when shooting

			weaponC->Shoot(); //Calls the weapons Shoot function that handles raycasting and applying hit logic (damage, niagara effects)

		}

	}

}

void APlayerCharacter::DoAim(float alpha)
{

	if (!IsValid(springArm))
	{

		return;

	}

	springArm->TargetArmLength = FMath::Lerp(cameraboom_Zoomed_Out, cameraboom_Zoomed_In, alpha); //Lerps between the two values for a zoom in/out effect

	/*
	double newSocketY = FMath::Lerp(socketY_Zoomed_Out, socketY_Zoomed_In, alpha); //Lerps between the two values for a over-the-shoulder effect
	double newSocketZ = FMath::Lerp(socketZ_Zoomed_Out, socketZ_Zoomed_In, alpha); //Same as above but for Z axis

	springArm->SocketOffset = FVector(0.0f, newSocketY, newSocketZ); //Sets the socket offset to the new values
	*/

	springArm->SocketOffset = FVector(0.0f, FMath::Lerp(socketY_Zoomed_Out, socketY_Zoomed_In, alpha), FMath::Lerp(socketZ_Zoomed_Out, socketZ_Zoomed_In, alpha)); //One-liner of the above calculations

	bUseControllerRotationYaw = isAimingC; //Determines whether the character should rotate with the controller when aiming

}

void APlayerCharacter::DoReload()
{

	if (IsValid(weaponC) && hasWeaponC && !isReloadingC && !isShootingC) //Checks if the player has a valid weapon and not already shooting or reloading
	{

		isReloadingC = true; //Sets reloading state to true, which triggers the reload animation and blocks other usage of the weapon until the reload is finished (reset by animation notify)

	}

}

void APlayerCharacter::BurnDamage(float timeOnFire, bool burning)
{

	if (!IsValid(burnEffect))
	{

		return;

	}

	//Burn effect logic toggle
	if (isBurning && !burning)
	{

		burnEffect->Deactivate(); //Burn effect ending

	}
	else if (!isBurning && burning)
	{

		burnEffect->Activate(); //Starts burneffect

	}

	isBurning = burning; //Sets the burning state to the value of the "burning" parameter, which is used to toggle the VFX for burning
	burnTime += timeOnFire; //Adds the time from blueprint timeline to the burnTime variable (used to determine when to take damage)

	if (burnTime >= 1.0f) //Applies burn damage every second the player is on fire, and reduces
	{

		burnTime -= 1.0f;
		health -= 5;

	}

	if (health <= 0) //Checks if the players health has reached 0 or below, and triggers death logic if so
	{

		UE_LOG(LogTemp, Warning, TEXT("Player burned to death - death logic needs implementing"));

	}

}