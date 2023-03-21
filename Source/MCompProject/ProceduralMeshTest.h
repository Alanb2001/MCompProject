#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "MeshDescription.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshTest.generated.h"

UCLASS()
class MCOMPPROJECT_API AProceduralMeshTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralMeshTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

		TArray<FVector> Vertices;
    	TArray<int32> Triangles;
    	TArray<FVector2D> UVs;
    
    	UPROPERTY()
    	UProceduralMeshComponent* ProcMesh;
		UPROPERTY(EditAnywhere)
		UMaterialInterface* Material;
	
    	void CreateMesh();
};
