// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAttackType.h"
#include "DrawDebugHelpers.h"
#include "Components/ShapeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"

void UBaseAttackType::BP_UseAttack_Implementation()
{
	if (!GetWorld()) return;

	if(bRotateCharacterOnAttack)
	{
		if(!GetWeaponOwner()) return;
		if(!Cast<APawn>(GetWeaponOwner())) return;
		if(!Cast<APlayerController>(Cast<APawn>(GetWeaponOwner())->GetController())) return;
		RotateCharacterToMouseCursor();
	}
}

void UBaseAttackType::Initialize(AActor* Owner)
{
	Rename(nullptr, Owner);

	for(auto HitAction : HitActions)
		HitAction.HitAction->Initialize(GetOwner());
}

void UBaseAttackType::StartUseAttack(const float DelayTime)
{
	if (DelayTime > 0.f)
	{
		FTimerHandle AttackTimer;
		GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &UBaseAttackType::BP_UseAttack, DelayTime, false);
	}
	else
		BP_UseAttack();
}

void UBaseAttackType::AddActorToHit(AActor* Actor)
{
	for (auto Tag : Actor->Tags)
		if (GetTagsToIgnore().Find(Tag) != INDEX_NONE) return;

	if(HitActors.Contains(Actor)) return;

	const TTuple<AActor*, bool> HitActor(Actor, false);
	
	HitActors.Add(HitActor);
}

void UBaseAttackType::DoHitAction()
{
	
	
	TSet<AActor*> ActorsToHit = {};
	for(auto& HitActor : HitActors)
		if(!HitActor.Value)
		{
			ActorsToHit.Add(HitActor.Key);
			HitActor.Value = true;
		}
	
	for(auto HitAction : HitActions)
	{
		const float RandChance = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		if(RandChance <= HitAction.HitActionChance)
			HitAction.HitAction->DoActionWithActors(ActorsToHit);
	}
}

void UBaseAttackType::EmptyHitActors()
{
	HitActors.Empty();
}

void UBaseAttackType::RotateCharacterToMouseCursor() const
{
	FHitResult MouseHit;
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, MouseHit);

	if (!MouseHit.bBlockingHit) return;

	const FVector ActorLocation = GetWeaponOwner()->GetActorLocation();

	MouseHit.ImpactPoint.Z = ActorLocation.Z;

	const FRotator ThrowRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, MouseHit.ImpactPoint);
	GetWeaponOwner()->SetActorRotation(ThrowRotation);
}

bool UBaseAttackType::IsDrawDebug() const
{
	return Cast<IWeaponInterface>(GetOwner())->Execute_IsDrawDebug(GetOwner());
}

float UBaseAttackType::GetWeaponDistance() const
{
	return Cast<IWeaponInterface>(GetOwner())->Execute_GetWeaponDistance(GetOwner());
}

TArray<FName> UBaseAttackType::GetTagsToIgnore() const
{
	return Cast<IWeaponInterface>(GetOwner())->Execute_GetTagsToIgnore(GetOwner());
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void UCircularAttack::BP_UseAttack_Implementation()
{
	Super::BP_UseAttack_Implementation();

	const float StartAngle = GetWeaponOwner()->GetActorRotation().Yaw + AngleOffset - Angle / 2;

	TArray<FHitResult> AllHits;

	for (int i = 0; i < Angle; i++)
	{
		TArray<FHitResult> OutHits;
		FRotator Rotation(0.f, StartAngle + i, 0.f);

		const FVector StartLocation = GetWeaponOwner()->GetActorLocation();
		const FVector EndLocation = StartLocation + Rotation.Vector() * GetWeaponDistance() * RadiusMultiplication;

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActors(TArray<AActor*>({GetOwner(), GetWeaponOwner()}));

		UKismetSystemLibrary::LineTraceMulti(GetWorld(), StartLocation, EndLocation,
                                              TraceTypeQuery1, true, TArray<AActor*>{GetOwner(), GetWeaponOwner()},
                                              IsDrawDebug() ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
                                              OutHits, true);

		for(auto Hit : OutHits)
			if(Hit.bBlockingHit)
				if(Cast<APawn>(Hit.Actor))
					AddActorToHit(Hit.GetActor());
	}

	DoHitAction();
	EmptyHitActors();
}
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void UPierceAttack::BP_UseAttack_Implementation()
{
	if (!GetWorld()) return;

	if(Cast<APlayerController>(Cast<APawn>(GetWeaponOwner())->GetController()))
	{
		FHitResult MouseHit;
		GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, MouseHit);

		if (!MouseHit.bBlockingHit) return;

		FVector OwnerLocation = GetWeaponOwner()->GetActorLocation();

		MouseHit.ImpactPoint.Z = OwnerLocation.Z;

		FVector HitLocation = MouseHit.ImpactPoint - OwnerLocation;

		float MinAttackRadius = GetWeaponDistance() * MinRadiusMultiplication;
		float MaxAttackRadius = GetWeaponDistance() * MaxRadiusMultiplication;

		if (HitLocation.Size() > MaxAttackRadius)
			HitLocation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, MouseHit.ImpactPoint)
                .Vector() * MaxAttackRadius + OwnerLocation;
		else if (HitLocation.Size() < MinAttackRadius)
			HitLocation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, MouseHit.ImpactPoint)
                .Vector() * MinAttackRadius + OwnerLocation;
		else
			HitLocation = MouseHit.ImpactPoint;

		HitLocation.Z = GetOwner()->GetActorLocation().Z;

		GetWeaponOwner()->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(OwnerLocation, HitLocation));

		TArray<FHitResult> Hits;
		auto DrawDebug = IsDrawDebug();
		UKismetSystemLibrary::BoxTraceMulti(GetWorld(), HitLocation, HitLocation, PointAttackRange,
                                            FRotator::ZeroRotator, TraceTypeQuery1, false,
                                            TArray<AActor*>{GetOwner(), GetWeaponOwner()},
                                            DrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
                                            Hits, true, FColor::Blue, FLinearColor::Green, 3.f);


		if (Hits.Num() > 0)
			for (auto Hit : Hits)
			{
				FHitResult LineHit;
				UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetWeaponOwner()->GetActorLocation(), Hit.ImpactPoint,
                                                      TraceTypeQuery1, false, TArray<AActor*>{GetOwner(), GetWeaponOwner()},
                                                      DrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
                                                      LineHit, true);

				if (LineHit.bBlockingHit)
					if (Cast<APawn>(LineHit.Actor))
					{
						AddActorToHit(LineHit.GetActor());
						break;
					}
			}
		else
		{
			FHitResult LineHit;
			UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetWeaponOwner()->GetActorLocation(), HitLocation,
                                                  TraceTypeQuery1, false, TArray<AActor*>{GetOwner(), GetWeaponOwner()},
                                                  DrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
                                                  LineHit, true);
			if (LineHit.bBlockingHit)
				if (Cast<APawn>(LineHit.Actor))
					AddActorToHit(LineHit.GetActor());
		}
	}

	
	DoHitAction();
	EmptyHitActors();
		
}


//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------


void UDashAttack::BP_UseAttack_Implementation()
{
	Super::BP_UseAttack_Implementation();

	TimePassed = 0.f;

	StartActorLocation = GetWeaponOwner()->GetActorLocation();

	switch (DashDirection)
	{
	case DD_Forward:
		EndActorLocation = StartActorLocation + GetWeaponOwner()->GetActorForwardVector() * GetWeaponDistance() *
			DashDistanceMultiplication;
		break;
	case DD_Back:
		EndActorLocation = StartActorLocation + GetWeaponOwner()->GetActorForwardVector() * -1.f * GetWeaponDistance() *
			DashDistanceMultiplication;
		break;
	case DD_Right:
		EndActorLocation = StartActorLocation + GetWeaponOwner()->GetActorRightVector() * GetWeaponDistance() *
			DashDistanceMultiplication;
		break;
	case DD_Left:
		EndActorLocation = StartActorLocation + GetWeaponOwner()->GetActorRightVector() * -1.f * GetWeaponDistance() *
			DashDistanceMultiplication;
		break;
	case DD_InVelocity:
		{
			const FVector Velocity = GetWeaponOwner()->GetVelocity();
			
			if (Velocity.Size() > 0.f)
				EndActorLocation = StartActorLocation + Velocity / 500.f * GetWeaponDistance() *
					DashDistanceMultiplication;
			else
				EndActorLocation = StartActorLocation + GetWeaponOwner()->GetActorForwardVector() * GetWeaponDistance()
					* DashDistanceMultiplication;
			break;
		}
	default:
		EndActorLocation = StartActorLocation + GetWeaponOwner()->GetActorForwardVector() * GetWeaponDistance() *
			DashDistanceMultiplication;
		break;
	}


	GetWorld()->GetTimerManager().SetTimer(DashTimer, this, &UDashAttack::Dash, TimerRate, true);

	if (HiddenActorWhileDash)
	{
		GetOwner()->SetActorHiddenInGame(true);
		GetWeaponOwner()->SetActorHiddenInGame(true);
	}

	OwnerCollision = Cast<UShapeComponent>(GetWeaponOwner()->GetComponentByClass(UShapeComponent::StaticClass()));
	for (auto Channel : CollisionChannelsToIgnore)
	{
		ActorResponses.Add(Channel, OwnerCollision->GetCollisionResponseToChannel(Channel));
		OwnerCollision->SetCollisionResponseToChannel(Channel, ECR_Ignore);
	}
}

void UDashAttack::Dash_Implementation()
{
	TimePassed += TimerRate;

	GetWeaponOwner()->SetActorLocation(
		FMath::Lerp(StartActorLocation, EndActorLocation, DashCurve->GetFloatValue(TimePassed)), true);
		
	if(bHitEnemiesOnDash)
	{
		TArray<FHitResult> TraceResults;

		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetWeaponOwner()->GetActorLocation(),
                                               GetWeaponOwner()->GetActorLocation(),
                                               GetWeaponDistance() * DashHitRadiusMultiplication, TraceTypeQuery1,
                                               false, TArray<AActor*>{GetOwner(), GetWeaponOwner()},
                                               IsDrawDebug() ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
                                               TraceResults, true);

		for (auto Hit : TraceResults)
		{
			if (Hit.bBlockingHit)
				if(Cast<APawn>(Hit.Actor))
					AddActorToHit(Hit.GetActor());
		}

		DoHitAction();
	}

	if (TimePassed >= DashTime)
	{
		GetWorld()->GetTimerManager().ClearTimer(DashTimer);	
		AfterDash();
	}
}

void UDashAttack::AfterDash_Implementation()
{
	EmptyHitActors();
	
	if (HiddenActorWhileDash)
	{
		GetOwner()->SetActorHiddenInGame(false);
		GetWeaponOwner()->SetActorHiddenInGame(false);
	}

	for (auto Response : ActorResponses)
	{
		OwnerCollision->SetCollisionResponseToChannel(Response.Key, Response.Value);
	}
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void UProjectileAttack::BP_UseAttack_Implementation()
{
	Super::BP_UseAttack_Implementation();
	Shoot();
}

void UProjectileAttack::Shoot()
{
	const FTransform ProjectileTransform = {
		GetWeaponOwner()->GetActorRotation(),
		GetOwner()->GetActorLocation() + GetWeaponOwner()->GetActorForwardVector() * SpawnForwardOffset
	};

	ABaseProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABaseProjectile>(
		ProjectileClass, ProjectileTransform, GetOwner(),
		Cast<APawn>(GetWeaponOwner()),
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (Projectile)
	{
		Projectile->SetAttackRef(this);
		Projectile->FinishSpawning(ProjectileTransform);
	}
}
