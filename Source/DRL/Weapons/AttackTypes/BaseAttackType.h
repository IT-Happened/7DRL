// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DRL/Weapons/Projectiles/BaseProjectile.h"
#include "UObject/Object.h"
#include "BaseAttackType.generated.h"

class UBaseDamageType;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

UENUM(BlueprintType)
enum EAttackTypes
{
	AT_BaseAttack UMETA(DisplayName="Base Attack"),
	AT_SpaceBar UMETA(DisplayName = "SpaceBar"),

	AT_MAX,
};

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
	


protected:
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "UseAttack"))
    void BP_UseAttack();
	
	UFUNCTION(BlueprintCallable)
	void AddEnemyToHit(AActor* Enemy);
	UFUNCTION(BlueprintCallable)
	void MakeDamage();
	UFUNCTION(BlueprintCallable)
	void ClearHitEnemies() {HitEnemies.Empty();}
	
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
	TMap<TSubclassOf<UBaseDamageType>, float> GetDamage() const;
	TArray<FName> GetTagsToIgnore() const;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackDamageMultiple = 1.f;
	
	UPROPERTY()
	TArray<AActor*> HitEnemies;
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
	TArray<FAttack> Attacks = {};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float AttackTimeMultiple = 1.f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TArray<UAnimMontage*> AttackAnimations = {};
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

	virtual void DoWithHitEnemies(TArray<FHitResult> Hits);


private:
	UPROPERTY(EditAnywhere, Category = "Radius")
	float AttackAngle = 120.f;
	UPROPERTY(EditAnywhere, Category = "Radius")
	float RadiusMultiplication = 1.f;

	UPROPERTY(EditAnywhere, Category = "Radius")
	float AngleOffset = 0.f;
};

UCLASS()
class UPushbackAttack: public UCircularAttack
{
	GENERATED_BODY()

protected:
	virtual void DoWithHitEnemies(TArray<FHitResult> Hits) override;

private:
	UPROPERTY(EditAnywhere, Category = "PushBack")
	float PushBackPower = 40000.f;
	
};


UCLASS()
class UStunAttack: public UCircularAttack
{
	GENERATED_BODY()

protected:
	virtual void DoWithHitEnemies(TArray<FHitResult> Hits) override;

private:
	UPROPERTY(EditAnywhere, Category = "PushBack")
	float StunTime = 0.4f;
	
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
	UPROPERTY(EditAnywhere, Category = "Radius")
	float MinRadiusMultiplication = 0.75f;
	UPROPERTY(EditAnywhere, Category = "Radius")
	float MaxRadiusMultiplication = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Radius|Advanced")
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
UCLASS()
class USwordDashAttack : public UDashAttack
{
	GENERATED_BODY()

protected:
	virtual void Dash_Implementation() override;

	virtual void AfterDash_Implementation() override;

private:
	UPROPERTY(EditAnywhere, Category = "Attack")
	float DashAttackRadiusMultiplication = 0.25f;
};

UCLASS()
class USpearDashAttack : public UDashAttack
{
	GENERATED_BODY()

protected:
	virtual void Dash_Implementation() override;
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