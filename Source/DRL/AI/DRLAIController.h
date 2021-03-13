// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DRLAIController.generated.h"

class UDRLAIPerceptionComponent;
/**
 * 
 */
UCLASS()
class DRL_API ADRLAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ADRLAIController();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDRLAIPerceptionComponent* DRLPerceptionComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	FName FocusOnKeyName = "EnemyActor";

	virtual void Tick(float DeltaSeconds) override;

private:
	AActor* GetFocusOnActor() const;
	
};
