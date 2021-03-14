// Fill out your copyright notice in the Description page of Project Settings.


#include "DRL/AI/Services/BTService_FindEnemy.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DRL/AI/DRLAIPerceptionComponent.h"

UBTService_FindEnemy::UBTService_FindEnemy()
{
	NodeName = "Find Enemy";
}

void UBTService_FindEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	if(Blackboard)
	{
		const auto Controller = OwnerComp.GetAIOwner();
		const auto PerceptionComponent = Controller->FindComponentByClass<UDRLAIPerceptionComponent>();

		if(PerceptionComponent)
		{
			Blackboard->SetValueAsObject(EnemyActorKey.SelectedKeyName, PerceptionComponent->GetClosestEnemy());
		}
	}
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}

