#pragma once

#include "CoreMinimal.h"
#include "MeshDescription.h"
#include <Components/BoxComponent.h>
#include "AbcImporter.h"
#include "ProceduralMeshComponent.h"
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
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Renderer;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* Cube;
	
	UPROPERTY(EditAnywhere)
	TArray<FVector2D> Polygon;
	
	UPROPERTY(EditAnywhere)
	float Thickness = 1.0f;
	
	UPROPERTY(EditAnywhere)
	float MinBreakArea = 0.01f;
	
	UPROPERTY(EditAnywhere)
	float MinImpactToBreak = 50.0f;
	
	UPROPERTY(EditAnywhere)
	float AreaMv = -1.0f;
	
	UPROPERTY(EditAnywhere)
	int Age;
	
	UFUNCTION()
	float Area();
	
	UFUNCTION()
	void Reload();
	
	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult);

	UFUNCTION()
	float NormalizedRandom(const float Mean, const float Stddev) const;

	UFUNCTION()
	void Break(const FVector2D Position);

	UFUNCTION()
	UStaticMesh* MeshFromPolygon(const TArray<FVector2D>& Polygon1, const float Thickness1);
};
