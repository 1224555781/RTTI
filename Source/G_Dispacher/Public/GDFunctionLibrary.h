// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GlobalDispatcher.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GDFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class G_DISPACHER_API UGDFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,meta = (HidePin="WorldContextObject",DefaultToSelf,WorldContext = "WorldContextObject" , AdvanceDiplay = "Type,Dispacher,DispachFuncName"),Category=GDFunctionLibrary)
	static FDispacherHandle AddListener(UObject* WorldContextObject,FString Type, UObject* Dispacher, FName DispachFuncName,
    EDispachOpportunity DispachOpportunity, float Time);
	UFUNCTION(BlueprintCallable,meta = (HidePin="WorldContextObject",DefaultToSelf,WorldContext = "WorldContextObject"),Category=GDFunctionLibrary)
	static void TriggerGDEvent(UObject* WorldContextObject,FString Type, FString Param);
	UFUNCTION(BlueprintCallable,meta = (HidePin="WorldContextObject",DefaultToSelf,WorldContext = "WorldContextObject" , AdvanceDiplay = "Type,Dispacher,DispachFuncName"),Category=GDFunctionLibrary)
	static void ClearEvent(UObject* WorldContextObject,FString Type);
	//UFUNCTION(BlueprintCallable,meta =(DefaultToSelf, WorldContext = "WorldContextObject"),Category=GDFunctionLibrary)
	static UGlobalDispatcher* GetDispacher(UObject* WorldContextObject);
};


