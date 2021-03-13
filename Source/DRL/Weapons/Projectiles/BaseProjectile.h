// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class UProjectileAttack;
class UBoxComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UBaseDamageType;

UCLASS(Abstract)
class DRL_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABaseProjectile();

	void SetAttackRef(UProjectileAttack* Attack) {AttackRef = Attack;}

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     FVector NormalImpulse, const FHitResult& Hit);
	
	void DestroyProjectile();

	virtual void Destroyed() override;

	UFUNCTION()                 
	void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                              const FHitResult& SweepResult);

	
private:

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* ProjectileMeshComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UBoxComponent* BoxCollision = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY()
	UProjectileAttack* AttackRef = nullptr;
};
