// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BaseHitAction.generated.h"

class UBaseDamageType;

UCLASS(Abstract, EditInlineNew, Blueprintable)
class DRL_API UBaseHitAction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Initialize(AActor* Owner);

	UFUNCTION()
	virtual void DoActionWithActors(TSet<AActor*> Actors);
	
protected:

	UFUNCTION(BlueprintCallable)
	AActor* GetOwner() const {return Cast<AActor>(GetOuter());}
	UFUNCTION(BlueprintCallable)
    AActor* GetWeaponOwner() const {return GetOwner()->GetOwner();}
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FHitAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Hit")
	float HitActionChance = 1.f;

	UPROPERTY(EditAnywhere, Instanced, Category = "Hit")
	UBaseHitAction* HitAction = nullptr;
};


//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

UCLASS()
class DRL_API UDamageAction : public UBaseHitAction
{
	GENERATED_BODY()

protected:

	virtual void DoActionWithActors(TSet<AActor*> Actors) override;

private:
	UPROPERTY(EditAnywhere, Category = "Damage")
	float DamageMultiplication = 1.f;

	TMap<TSubclassOf<UBaseDamageType>, float> GetDamage() const;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

UENUM()
enum EPushDirection
{
	PD_From UMETA(DisplayName = "From"),
	PD_To UMETA(DisplayName = "To"),

	PD_MAX,
};

UCLASS()
class DRL_API UPushAction : public UBaseHitAction
{
	GENERATED_BODY()

protected:
	virtual void DoActionWithActors(TSet<AActor*> Actors) override;

private:
	UPROPERTY(EditAnywhere, Category = "Push")
	float PushBackPower = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Push")
	TEnumAsByte<EPushDirection> PushDirection = PD_From;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

UCLASS()
class DRL_API UStunAction : public UBaseHitAction
{
	GENERATED_BODY()

protected:
	virtual void DoActionWithActors(TSet<AActor*> Actors) override;

private:
	UPROPERTY(EditAnywhere, Category = "Stun")
	float StunTimer = 0.5f;
};
