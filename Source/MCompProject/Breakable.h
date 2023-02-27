// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Geom.h"
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

	UStaticMeshComponent* CubeMesh;
	UStaticMeshComponent* filter;
	UStaticMeshComponent* renderer;
	UStaticMeshComponent* collider;
	//URigidBodyComponent* rigidbody;
	TArray<FVector2D> polygon;
	float thickness = 1.0f;
	float minBreakArea = 0.01f;
	float minImpactToBreak = 50.0f;
	float area = -1.0f;
	int age;

	float Area();

	void Reload();

	UStaticMesh* MeshFromPolygon(const TArray<FVector2D>& Polygon, const float Thickness);
};
