// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUp.h"

// Sets default values
APowerUp::APowerUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APowerUp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APowerUp::Interact_Implementation(APlayerCharacter* interactor)
{

	//Override in Blueprint
	UE_LOG(LogTemp, Warning, TEXT("WARNING! No Blueprint-override was found for Interact!"));

}

FText APowerUp::GetTooltip_Implementation()
{

	return TooltipText;

}