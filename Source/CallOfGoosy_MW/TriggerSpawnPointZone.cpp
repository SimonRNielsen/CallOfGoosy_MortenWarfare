// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerSpawnPointZone.h"

// Sets default values
ATriggerSpawnPointZone::ATriggerSpawnPointZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Collisionbox
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));

	RootComponent = CollisionComponent;
}

// Called when the game starts or when spawned
void ATriggerSpawnPointZone::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATriggerSpawnPointZone::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ATriggerSpawnPointZone::OnOverlapEnd);
}

// Called every frame
void ATriggerSpawnPointZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATriggerSpawnPointZone::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlab Begin"));
		HandleEnemySpawn();
	}
}


void ATriggerSpawnPointZone::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlap End"));
	}
}

