// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GlobalDispatcher.h"
#include "Engine/EngineTypes.h"
#include "Engine/GameInstance.h"
#include "GD_GameInstace.generated.h"

/**
 * 
 */
UCLASS()
class G_DISPACHER_API UGD_GameInstace : public UGameInstance
{
	GENERATED_BODY()
	UGD_GameInstace();

public:
	virtual void Init() override;
	virtual void Shutdown() override;
	constexpr  UGlobalDispatcher* GetGlobalDispatcher();
private:

/**
 * @brief 
 */
UGlobalDispatcher* GlobalDispatcher;
	
};

