// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class CALLOFGOOSY_MW_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Health")
	int health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool isAimingC = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool isShootingC = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool isReloadingC = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	bool hasWeaponC = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Weapon")
	AWeapon* weaponC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	double socketZ_Zoomed_Out = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	double socketZ_Zoomed_In = -15.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	double socketY_Zoomed_Out = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	double socketY_Zoomed_In = 50.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	double cameraboom_Zoomed_Out = 400.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Camera")
	double cameraboom_Zoomed_In = 300.0;

protected:
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
	void DoAim(double alpha);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DoReload();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(int damage);

};
