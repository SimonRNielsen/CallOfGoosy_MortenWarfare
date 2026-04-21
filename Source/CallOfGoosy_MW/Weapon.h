// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.generated.h"

class APlayerCharacter;

UCLASS()
class CALLOFGOOSY_MW_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
	int ammo = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
	int maxAmmo = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
	bool canFire = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
	UDecalComponent* aimDot;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	double maxDistance = 10000.0;
	int damage = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player")
	APlayerCharacter* player;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Gun")
	void Shoot();

	UFUNCTION(BlueprintCallable, Category = "Gun")
	void AimDotMovement();

};
