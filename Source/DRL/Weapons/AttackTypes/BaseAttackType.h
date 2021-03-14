// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DRL/Weapons/HitActions/BaseHitAction.h"
#include "DRL/Weapons/Projectiles/BaseProjectile.h"
#include "UObject/Object.h"
#include "BaseAttackType.generated.h"

class UBaseDamageType;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

UINTERFACE()
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};
class IWeaponInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	TMap<TSubclassOf<UBaseDamageType>, float> GetWeaponDamage();
	
	UFUNCTION(BlueprintImplementableEvent)
	TArray<FName> GetTagsToIgnore();

	UFUNCTION(BlueprintImplementableEvent)
	float GetWeaponDistance();

	UFUNCTION(BlueprintImplementableEvent)
	bool IsDrawDebug();
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

UINTERFACE()
class UCharacterInterface : public UInterface
{
	GENERATED_BODY()
};
class ICharacterInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StunCharacter(float StunTime);
	
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

UCLASS(Abstract, Blueprintable, EditInlineNew)
class DRL_API UBaseAttackType : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void StartUseAttack(float DelayTime = 0.f);

	UFUNCTION(BlueprintCallable)
	void Initialize(AActor* Owner);

protected:
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "UseAttack"))
    void BP_UseAttack();
	
	UFUNCTION(BlueprintCallable)
	void AddActorToHit(AActor* Actor);
	UFUNCTION(BlueprintCallable)
	void DoHitAction();
	UFUNCTION(BlueprintCallable)
    void EmptyHitActors();
	
	UFUNCTION(BlueprintCallable)
	void RotateCharacterToMouseCursor() const;
	
	UFUNCTION(BlueprintCallable)
    AActor* GetOwner() const {return Cast<AActor>(GetOuter());}
	UFUNCTION(BlueprintCallable)
    AActor* GetWeaponOwner() const {return GetOwner()->GetOwner();}
	
	UFUNCTION(BlueprintCallable)
	bool IsDrawDebug() const;
	UFUNCTION(BlueprintCallable)
	float GetWeaponDistance() const;
	
private:
	TArray<FName> GetTagsToIgnore() const;	

	UPROPERTY(EditAnywhere, Category = "Hit")
	TArray<FHitAction> HitActions = {};

	UPROPERTY(EditAnywhere, Category = "Advanced")
	bool bRotateCharacterOnAttack = true;

	UPROPERTY()
	TMap<AActor*, bool> HitActors = {};
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

USTRUCT(BlueprintType)
struct FAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (DisplayName = "Delay before attack (percentage)"))
	float DelayTime = 0.f;
	
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Attack")
	UBaseAttackType* Attacks = {};
};
USTRUCT(BlueprintType)
struct FComboAttack
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	FName AttackName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TArray<FAttack> Attacks = {};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float AttackTimeMultiple = 1.f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TArray<UAnimMontage*> AttackAnimations = {};
};

UENUM(BlueprintType)
enum EAttackActions
{
	AA_NONE UMETA(DisplayName = "None"),
	AA_Light UMETA(DisplayName = "Light"),
	AA_Dash UMETA(DisplayName = "Dash") ,

	AA_MAX,
};

UCLASS(BlueprintType)
class UCombinationsObject : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void AddNextActions(TMap<TEnumAsByte<EAttackActions>, UCombinationsObject*> NextAttack);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo", meta = (ExposeOnSpawn = "true"))
	FName AttackName = NAME_None;
	
	UPROPERTY(VisibleAnywhere, Instanced, BlueprintReadOnly, Category = "Combo")
	TMap<TEnumAsByte<EAttackActions>, UCombinationsObject*> NextCombination = {};
};

USTRUCT(BlueprintType)
struct FCombinations
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Instanced, BlueprintReadWrite, Category = "Combo")
	UCombinationsObject* RootCombo = nullptr;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

UCLASS()
class UCircularAttack : public UBaseAttackType
{
	GENERATED_BODY()

protected:
	virtual void BP_UseAttack_Implementation() override;
	

private:
	UPROPERTY(EditAnywhere, Category = "Hit")
	float Angle = 120.f;
	UPROPERTY(EditAnywhere, Category = "Hit")
	float RadiusMultiplication = 1.f;

	UPROPERTY(EditAnywhere, Category = "Hit")
	float AngleOffset = 0.f;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

UCLASS()
class UPierceAttack : public UBaseAttackType
{
	GENERATED_BODY()
protected:
	virtual void BP_UseAttack_Implementation() override;

private:
	UPROPERTY(EditAnywhere, Category = "Hit")
	float MinRadiusMultiplication = 0.75f;
	UPROPERTY(EditAnywhere, Category = "Hit")
	float MaxRadiusMultiplication = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Hit|Advanced")
	FVector PointAttackRange = FVector(60.f, 60.f, 40.f);
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

UENUM()
enum EDashDirection
{
	DD_Forward UMETA(DisplayName = "Forward"),
	DD_Right UMETA(DisplayName = "Right"),
	DD_Left UMETA(DisplayName = "Left"),
	DD_Back UMETA(DisplayName = "Back"),
	DD_InVelocity UMETA(DisplayName = "InVelocity"),

	DD_MAX,
};
UCLASS()
class UDashAttack : public UBaseAttackType
{
	GENERATED_BODY()

protected:
	virtual void BP_UseAttack_Implementation() override;
	
	UFUNCTION(BlueprintNativeEvent)
    void Dash();
    
	UFUNCTION(BlueprintNativeEvent)
	void AfterDash();

private:
	UPROPERTY(EditAnywhere, Category = "Dash|Movement")
	float DashDistanceMultiplication = 4.f;
	UPROPERTY(EditAnywhere, Category = "Dash|Movement")
	float DashTime = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Hit")
	bool bHitEnemiesOnDash = false;
	
	UPROPERTY(EditAnywhere, Category = "Hit")
	float DashHitRadiusMultiplication = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Dash|Movement")
	TEnumAsByte<EDashDirection> DashDirection = DD_Forward;

	UPROPERTY(EditAnywhere, Category = "Dash")
	bool HiddenActorWhileDash = false;
	UPROPERTY(EditAnywhere, Category = "Dash")
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannelsToIgnore;
	
	UPROPERTY(EditAnywhere, Category = "Dash|Movement")
	UCurveFloat* DashCurve = nullptr;
	UPROPERTY(EditAnywhere, Category = "Dash|Advanced")
	float TimerRate = 0.001f;

	UPROPERTY()
	UShapeComponent* OwnerCollision = nullptr;
	TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> ActorResponses;
	
	FVector StartActorLocation;
	FVector EndActorLocation;
	float TimePassed = 0.f;
	FTimerHandle DashTimer;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

UCLASS()
class UProjectileAttack : public UBaseAttackType
{
	GENERATED_BODY()

	friend class ABaseProjectile;

protected:
	virtual void BP_UseAttack_Implementation() override;

private:
	void Shoot();
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<ABaseProjectile> ProjectileClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "Projectile|Advanced")
	float TimeToDestroyAfterHit = 0.f;
	UPROPERTY(EditAnywhere, Category = "Projectile|Advanced")
	float SpawnForwardOffset = 0.f;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------