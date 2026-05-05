// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon.h"
#include "IBurnable.h"
#include "IInteractable.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDeath);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateAmmo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateKills);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayDamagedSound);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOpenMenu);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartGame);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartFire);

UCLASS()
class CALLOFGOOSY_MW_API APlayerCharacter : public ACharacter, public IIBurnable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Health")
	int health = 100;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player|Health")
	int maxHealth = 100;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player|Health")
	int damageBeforeComplaint = 10;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player|Health")
	int fireDamage = 5;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player|Health")
	float tickInterval = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Health")
	UNiagaraComponent* burnEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Health")
	bool IsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Health")
	bool TookDamageThisTick = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Movement")
	bool IsSprinting = false;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player|Movement")
	float MaxMovespeedAiming = 100.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player|Movement")
	float MaxMovespeedWalking = 375.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player|Movement")
	float MaxMovespeedSprinting = 600.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Movement")
	float Stamina = 1.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player|Movement")
	float StaminaDrainRate = 0.005f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player|Movement")
	float StaminaRegenRate = 0.01f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Player|Movement")
	float TimeBeforeRegainingStamina = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool isAimingC = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool isShootingC = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool isReloadingC = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool hasWeaponC = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Weapon")
	TSubclassOf<AWeapon> weaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Weapon")
	AWeapon* weaponC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Weapon")
	USkeletalMeshComponent* playerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	bool MenuOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	float socketZ_Zoomed_Out = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	float socketZ_Zoomed_In = -15.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	float socketY_Zoomed_Out = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	float socketY_Zoomed_In = 50.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	float cameraboom_Zoomed_Out = 400.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	float cameraboom_Zoomed_In = 300.0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Camera")
	USpringArmComponent* springArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Camera")
	TSubclassOf<UUserWidget> HUDClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Camera")
	UUserWidget* HUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Interaction")
	AActor* Interactable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Interaction")
	bool IsInteracting = false;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FUpdateHealth UpdateHealth;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FUpdateHealth UpdateAmmo;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FUpdateKills UpdateKills;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FPlayerDeath PlayerDeath;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FPlayDamagedSound AuchSound;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FOpenMenu OpenMenu;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FStartGame StartGame;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FStartFire StartFire;

protected:

	float burnTime = 0.0f;
	float TimeSinceLastSprint = 0.0f;
	bool isBurning = false;
	int damageTaken = 0;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	FCriticalSection BurnLock; //Thread lock

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DoGetWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DoShoot();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DoAim(float alpha);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DoReload();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void BurnDamage(float timeOnFire, bool burning);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ResetPlayer();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Sprint(bool trySprint);

	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void StartFire_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();

};
