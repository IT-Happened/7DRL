// Fill out your copyright notice in the Description page of Project Settings.


#include "DRL/AI/DRLAIPerceptionComponent.h"


#include "AIController.h"
#include "GameFramework/Character.h"
#include "Perception/AISense_Sight.h"

AActor* UDRLAIPerceptionComponent::GetClosestEnemy() const
{
	TArray<AActor*> PerceiveActors = {};
	GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceiveActors);
	if (PerceiveActors.Num() == 0) return nullptr;

	const auto Controller = Cast<AAIController>(GetOwner());
	if (!Controller) return nullptr;

	const auto Pawn = Controller->GetPawn();
	if (!Pawn) return nullptr;

	float BestDistance = MAX_FLT;
	AActor* BestPawn = nullptr;

	for (const auto PerceiveActor : PerceiveActors)
	{
		const ACharacter* Character = Cast<ACharacter>(PerceiveActor);
		if (Character)
		{
			if (Cast<APlayerController>(Character->GetController()))
			{
				const auto CurrentDistance = (PerceiveActor->GetActorLocation() - Pawn->GetActorLocation()).Size();

				if (CurrentDistance < BestDistance)
				{
					BestDistance = CurrentDistance;
					BestPawn = PerceiveActor;
				}
			}
		}
	}

	return BestPawn;
}
