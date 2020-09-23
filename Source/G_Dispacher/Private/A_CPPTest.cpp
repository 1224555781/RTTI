// Fill out your copyright notice in the Description page of Project Settings.


#include "A_CPPTest.h"

#include "GDFunctionLibrary.h"

const EventType event =TEXT("MLB2");

// Sets default values
AA_CPPTest::AA_CPPTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AA_CPPTest::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AA_CPPTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AA_CPPTest::TestPrint(const AnyType& AT)
{
	FString MLB;
	MLB = AT.IsSameType<FString>()? AT.AnyCast<FString>() :TEXT("CAST FAILED");
	print(TEXT("MLB2 Has Called ")+MLB);
}

void AA_CPPTest::addDispachEvent()
{
	UGDFunctionLibrary::GetDispacher(this)
		->AddListener_Implement_CPP(event, std::bind(&AA_CPPTest::TestPrint, this, std::placeholders::_1), EDispachOpportunity::WaitTime,15.f);

}

void AA_CPPTest::TriggerEvent(const FString& Param)
{
	UGDFunctionLibrary::GetDispacher(this)
	->TriggerEvent_CPP(event, Param);
}

