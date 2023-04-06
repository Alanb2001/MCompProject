#pragma once

#include "CoreMinimal.h"
#include "MeshDescription.h"
#include <Components/BoxComponent.h>
#include "AbcImporter.h"
#include "GameFramework/Actor.h"
#include "Breakable.generated.h"

UCLASS()
class ABreakable final : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABreakable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(const float DeltaTime) override;

	UPROPERTY()
	UStaticMeshComponent* Renderer;
	UPROPERTY()
	UBoxComponent* Cube;
	UPROPERTY()
	TArray<AActor*> OverlappingActors;

	TArray<FVector2D> Polygon;
	
	float Thickness = 1.0f;
	float MinBreakArea = 0.01f;
	float MinImpactToBreak = 50.0f;
	float AreaMv = -1.0f;
	int Age;
	
	float Area();

	void Reload();

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult);
	
	float NormalizedRandom(const float Mean, const float Stddev) const;
	
	void Break(const FVector2D Position);

	UStaticMesh* MeshFromPolygon(const TArray<FVector2D>& Polygon1, const float Thickness1);
};
