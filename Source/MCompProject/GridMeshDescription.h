#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"
#include "GameFramework/Actor.h"
#include "GridMeshDescription.generated.h"

UCLASS()
class MCOMPPROJECT_API AGridMeshDescription : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridMeshDescription();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FMeshDescription build_grid_mesh_description(int x_count, int y_count, float x_scale, float y_scale);
};
