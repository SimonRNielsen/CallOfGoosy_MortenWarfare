// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IInteractable.h"
#include "PowerUp.generated.h"

UENUM(BlueprintType)
enum class EPowerUpType : uint8
{

	DAMAGE, STAMINA, SPEED, INVULNERABLE, CONFUSION,

};

UCLASS()
class CALLOFGOOSY_MW_API APowerUp : public AActor, public IIInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerUp();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerUp|Tooltip")
	FText TooltipText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp|Type")
	EPowerUpType Type;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(APlayerCharacter* interactor) override;

	virtual FText GetTooltip_Implementation() override;

};
