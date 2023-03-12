#pragma once

#include "CoreMinimal.h"
#include "Geom.h"
#include "VoronoiCalculator.h"
#include "VoronoiClipper.h"
#include <vector>
#include <cassert>
#include <random>
#include <cmath>
#include "ProceduralMeshComponent.h"
#include "MeshDescription.h"
#include <Components/BoxComponent.h>
#include "StaticMeshAttributes.h"
#include "GameFramework/Actor.h"
#include "Breakable.generated.h"

UCLASS()
class ABreakable : public AActor
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
	virtual void Tick(float DeltaTime) override;

	UStaticMeshComponent* renderer;
	UBoxComponent* cube;
	//URigidBodyComponent* Rigidbody;
	TArray<FVector2D> polygon;
	TArray<AActor*> overlappingActors;
	
	float thickness = 1.0f;
	float minBreakArea = 0.01f;
	float minImpactToBreak = 50.0f;
	float area = -1.0f;
	int age;
	
	float Area();

	void Reload();

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult);
	
	float NormalizedRandom(float mean, float stddev);
	
	void Break(FVector2D position);

	UStaticMesh* MeshFromPolygon(const TArray<FVector2D>& Polygon, const float Thickness);
};
