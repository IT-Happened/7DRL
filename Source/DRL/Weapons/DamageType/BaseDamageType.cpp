// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDamageType.h"

void UBaseDamageType::TakeDamage(float& VariableToTakeDamage, const float Damage)
{
	VariableToTakeDamage -= Damage;
}
