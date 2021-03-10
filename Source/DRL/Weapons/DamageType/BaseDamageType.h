// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDamageType.generated.h"


UCLASS(Abstract)
class DRL_API UBaseDamageType : public UDamageType
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void TakeDamage(float& VariableToTakeDamage, float Damage);
};
