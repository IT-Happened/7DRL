// Fill out your copyright notice in the Description page of Project Settings.


#include "LoggerService.h"
#include <iostream>
#include <fstream>
// Sets default values for this component's properties
ULoggerService::ULoggerService()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULoggerService::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void ULoggerService::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool ULoggerService::SaveText(FString SaveDirectory, FString FinalString)
{
	//where i'm saving the file UE_LOG(LogTemp, Warning, TEXT("TestoSalvato: %s"), *FinalString);
	//what i'm saving return FFileHelper::SaveStringToFile(FinalString, *SaveDirectory);
	//Saving
	std::ofstream log_file;
	log_file.open(SaveDirectory + "\\example_log.txt");
	log_file << TCHAR_TO_UTF8(*FinalString);
	UE_LOG(LogTemp, Warning, TEXT("Salvato in: %s"), *SaveDirectory);
}