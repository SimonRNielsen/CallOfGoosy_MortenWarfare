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
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlab Begin"));
		HandleEnemySpawn();
	}
}


void ATriggerSpawnPointZone::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlap End"));
	}
}

void ATriggerSpawnPointZone::MoveToInactivePool(class AActor* DeadActor)
{
	DeadActor->GetRootComponent()->Deactivate();
	DeadActor->GetRootComponent()->SetVisibility(false);
	
	InactiveActors.Add(DeadActor);

	ActiveActors.Remove(DeadActor);

	//UE_LOG(LogTemp, Warning, TEXT("Inactive pool: %d"), InactiveActors.Num());
}

void ATriggerSpawnPointZone::MoveToActivePool(FTransform spawnTransform)
{
	if (InactiveActors.Num() == 0)
	{
		SpawnEnemy(spawnTransform);
	}
	else
	{
		int length = InactiveActors.Num();
		AActor* AliveAcctor = InactiveActors[length - 1];

		AliveAcctor->GetRootComponent()->Activate();
		AliveAcctor->GetRootComponent()->SetVisibility(true);

		AliveAcctor->SetActorTransform(spawnTransform);
	}
}

void ATriggerSpawnPointZone::SpawnEnemy(FTransform spawnTransform)
{
	FActorSpawnParameters parameters; //Used for setting additional spawnparameters
	AEnemy* newEnemy = GetWorld()->SpawnActor<AEnemy>(enemyClass, spawnTransform, parameters); //Sets the pointer while spawning the new enemyn at the same time
	newEnemy->parent = this; //Setting this as parent for the newEnemy
}