// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "TriggerSpawnPointZone.generated.h"

UCLASS()
class CALLOFGOOSY_MW_API ATriggerSpawnPointZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriggerSpawnPointZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Collisionbox
	UPROPERTY(VisibleAnywhere);
	UBoxComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "TriggerSpawnPointZone|ActiveEnmeis")
	TArray<AActor*> ActiveActors;

	UPROPERTY(VisibleAnywhere, Category = "TriggerSpawnPointZone|InactiveEnemies")
	TArray<AActor*> InactiveActors;

	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "TriggerSpawnPointZone|SpawnActor")
	void HandleEnemySpawn();

	UFUNCTION(BlueprintCallable, Category = "TriggerSpawnPointZone|InactiveActor")
	void MoveToInactivePool(class AActor* DeadActor);

	UFUNCTION(BlueprintCallable, Category = "TriggerSpawnPointZone|ActiveActor")
	void MoveToActivePool(FTransform spawnTransform);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TriggerSpawnPointZone|Ennemy")
	TSubclassOf<AEnemy> enemyClass;
};
