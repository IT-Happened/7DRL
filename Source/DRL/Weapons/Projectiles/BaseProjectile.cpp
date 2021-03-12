// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectile.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "DRL/Weapons/AttackTypes/BaseAttackType.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
	BoxCollision = CreateDefaultSubobject<class UBoxComponent>(TEXT("Collision"));
	if (BoxCollision)
	{
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		BoxCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
		BoxCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	}

	SetRootComponent(BoxCollision);

	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));

	if (ProjectileMeshComponent)
	{
		ProjectileMeshComponent->SetupAttachment(BoxCollision);
	}

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->InitialSpeed = 2000.f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	}
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	check(ProjectileMovementComponent);
	check(BoxCollision);

	BoxCollision->IgnoreActorWhenMoving(GetOwner(), true);
	BoxCollision->IgnoreActorWhenMoving(GetInstigator(), true);

	BoxCollision->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnProjectileHit);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnProjectileBeginOverlap);

	SetLifeSpan(5.f);
}

void ABaseProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FTimerHandle DestroyTimer;
	const float DestroyTime = AttackRef->TimeToDestroyAfterHit;
	if(DestroyTime > 0.f)
		GetWorld()->GetTimerManager().SetTimer(DestroyTimer, this, &ABaseProjectile::DestroyProjectile, DestroyTime, false);
	else
		Destroy();
}

void ABaseProjectile::DestroyProjectile()
{
	Destroy();
}

void ABaseProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                               const FHitResult& SweepResult)
{
	AttackRef->AddEnemyToHit(OtherActor);
	AttackRef->MakeDamage();
}