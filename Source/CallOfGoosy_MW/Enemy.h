// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IShootable.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Enemy.generated.h"

UCLASS()
class CALLOFGOOSY_MW_API AEnemy : public ACharacter, public IIShootable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Health")
	int health = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Health")
	int maxHealth = 20;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy|Health")
	bool alive = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Feather")
	UNiagaraSystem* FeatherEffect;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(int damage, FVector impactPoint, FRotator impactNormal) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy|Health")
	void HandleDeath();

};
