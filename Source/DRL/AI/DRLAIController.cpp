// Fill out your copyright notice in the Description page of Project Settings.


#include "DRL/AI/DRLAIController.h"

#include "DRLAIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

ADRLAIController::ADRLAIController()
{
	DRLPerceptionComponent = CreateDefaultSubobject<UDRLAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SetPerceptionComponent(*DRLPerceptionComponent);
}

void ADRLAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const auto AimActor = GetFocusOnActor();

	SetFocus(AimActor);
}

AActor* ADRLAIController::GetFocusOnActor() const
{
	if(!GetBlackboardComponent()) return nullptr;
	return Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(FocusOnKeyName));
}
