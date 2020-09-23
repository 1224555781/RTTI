// Fill out your copyright notice in the Description page of Project Settings.


#include "G_Dispacher/Public/GD_GameInstace.h"

#include "NavigationSystem.h"


UGD_GameInstace::UGD_GameInstace()
{
    
}

void UGD_GameInstace::Init()
{
    Super::Init();
   
    //UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
}

void UGD_GameInstace::Shutdown()
{
    if (GlobalDispatcher)
    {
        GlobalDispatcher->RemoveFromRoot();
    }
    Super::Shutdown();
}

constexpr UGlobalDispatcher* UGD_GameInstace::GetGlobalDispatcher()
{
    if (GlobalDispatcher)
    {
        return GlobalDispatcher;
    }
    else
    {
        GlobalDispatcher = NewObject<UGlobalDispatcher>(this);
        GlobalDispatcher->Init();
        GlobalDispatcher->AddToRoot();
        return GlobalDispatcher;
    }
    return nullptr;
}
