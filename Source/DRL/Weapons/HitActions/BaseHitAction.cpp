// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseHitAction.h"

#include "DrawDebugHelpers.h"
#include "DRL/Weapons/AttackTypes/BaseAttackType.h"
#include "DRL/Weapons/DamageType/BaseDamageType.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UBaseHitAction::Initialize(AActor* Owner)
{
	Rename(nullptr, Owner);
}

void UBaseHitAction::DoActionWithActors(TSet<AActor*> Actors) {}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void UDamageAction::DoActionWithActors(TSet<AActor*> Actors)
{
	auto Damages = GetDamage();
	
	for(auto Actor : Actors)
		for(const auto Damage : Damages)
			UGameplayStatics::ApplyDamage(Actor, Damage.Value * DamageMultiplication, Cast<APawn>(GetWeaponOwner())->GetController(), GetOwner(), Damage.Key);
}

TMap<TSubclassOf<UBaseDamageType>, float> UDamageAction::GetDamage() const
{
	return Cast<IWeaponInterface>(GetOwner())->Execute_GetWeaponDamage(GetOwner());
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void UPushAction::DoActionWithActors(TSet<AActor*> Actors)
{
	for(auto Actor : Actors)
	{
		ACharacter* Character = Cast<ACharacter>(Actor);
		if(Character)
		{
			FVector OwnerLocation = GetWeaponOwner()->GetActorLocation();
			OwnerLocation.Z = 0;
			FVector ActorLocation = Actor->GetActorLocation();
			ActorLocation.Z = 0;

			const FRotator ThrowRotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, ActorLocation);
			FVector DirectionVector;
			
			switch(PushDirection)
			{
			case PD_From:
				DirectionVector = ThrowRotation.Vector();
				break;
			case PD_To:
				DirectionVector = -ThrowRotation.Vector();
				break;
			default:
				DirectionVector = ThrowRotation.Vector();
				break;
			}

			if(Cast<IWeaponInterface>(GetOwner())->IsDrawDebug())
				DrawDebugLine(GetWorld(), Actor->GetActorLocation(), DirectionVector* PushBackPower, FColor::Red, false, 2.f, -1, 3.f);

			Character->GetCharacterMovement()->AddImpulse(DirectionVector * PushBackPower, true);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void UStunAction::DoActionWithActors(TSet<AActor*> Actors)
{
	for(auto Actor : Actors)
	{
		if(Actor->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
			ICharacterInterface::Execute_StunCharacter(Actor, StunTimer);
	}
}
