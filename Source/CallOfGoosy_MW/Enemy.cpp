// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(int damage, FVector impactPoint, FRotator impactNormal)
{
	//Taking damage which will be reduced by damage
	health -= damage;

	//Spawn hit particals
	if (IsValid(FeatherEffect))
	{

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			FeatherEffect,
			impactPoint,
			impactNormal,
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	}

	//If health is 0 or less call the HandleDeath event in Blueprint
	if (health <= 0)
	{
		HandleDeath();
		alive = false;
	}
}

