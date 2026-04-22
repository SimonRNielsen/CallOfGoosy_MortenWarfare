// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "IShootable.h"
#include "Weapon.generated.h"

class APlayerCharacter;

UCLASS()
class CALLOFGOOSY_MW_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player")
	APlayerCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Mesh")
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Gun")
	UDecalComponent* AimDot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
	int ammo = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Gun")
	int maxAmmo = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
	int HeadShotMultiplier = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
	int BodyShotMultiplier = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
	int ShotMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Gun")
	int damage = 5;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float maxDistance = 10000.0;

	bool DoLineTrace(FHitResult& result);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Gun")
	void Shoot();

	UFUNCTION(BlueprintCallable, Category = "Gun")
	void AimDotMovement();

};
