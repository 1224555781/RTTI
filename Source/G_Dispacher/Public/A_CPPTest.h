// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <functional>
#include <map>
#include <vector>



#include "AnyType.h"
#include "GameFramework/Actor.h"
#include "A_CPPTest.generated.h"

UCLASS()
class G_DISPACHER_API AA_CPPTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AA_CPPTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle Handle;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	void TestPrint(const AnyType& AT);

	using a= std::function<void(const AnyType& AT)>;
	using b= std::vector<std::weak_ptr<a>>;
	b _vec_weak_sp;
	std::map<FString, b>c;
	UFUNCTION(BlueprintCallable, Category = CPPTest)
	void addDispachEvent();

	UFUNCTION(BlueprintCallable,Category = CPPTest)
	void TriggerEvent(const FString& Param);
};
