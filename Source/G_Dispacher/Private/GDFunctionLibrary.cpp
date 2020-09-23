// Fill out your copyright notice in the Description page of Project Settings.


#include "G_Dispacher/Public/GDFunctionLibrary.h"

#include "GD_GameInstace.h"
#include "Kismet/GameplayStatics.h"

FDispacherHandle UGDFunctionLibrary::AddListener(UObject* WorldContext,EventType Type, UObject* Dispacher, FName DispachFuncName,
                                                 EDispachOpportunity DispachOpportunity, float Time)
{
   return GetDispacher(WorldContext)->AddListener(Type,Dispacher,DispachFuncName,DispachOpportunity,Time);
}

void UGDFunctionLibrary::TriggerGDEvent(UObject* WorldContext,EventType Type, FString Param)
{
   GetDispacher(WorldContext)->TriggerEvent(Type,Param);
}

void UGDFunctionLibrary::ClearEvent(UObject* WorldContext, FString Type)
{
    GetDispacher(WorldContext)->ClearEvent(Type);
}

UGlobalDispatcher* UGDFunctionLibrary::GetDispacher(UObject* WorldContext)
{
    UGD_GameInstace* GameInstace =static_cast<UGD_GameInstace*>( UGameplayStatics::GetGameInstance(WorldContext));
    return GameInstace->GetGlobalDispatcher();
}
