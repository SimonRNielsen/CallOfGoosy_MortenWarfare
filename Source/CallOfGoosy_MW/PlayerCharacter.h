// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class CALLOFGOOSY_MW_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Health")
	int health = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Health")
	UNiagaraComponent* burnEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool isAimingC = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool isShootingC = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool isReloadingC = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool hasWeaponC = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	TSubclassOf<AWeapon> weaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	AWeapon* weaponC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	USkeletalMeshComponent* playerMesh;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	UCameraComponent* camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	USpringArmComponent* springArm;

protected:

	float burnTime = 0.0;
	bool isBurning = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


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

};
