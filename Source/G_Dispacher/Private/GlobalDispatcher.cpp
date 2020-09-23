// Fill out your copyright notice in the Description page of Project Settings.


#include "G_Dispacher/Public/GlobalDispatcher.h"


#include "TimerManager.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"


#define INVALIDINDEX -1
DEFINE_LOG_CATEGORY(GlobalDispach);

int64 FDispacherHandle::s_ListenHandle = 0;
int64 FDispacherHandle::i_ListenHandle = -1;


void UGlobalDispatcher::Init()
{
    //Init DispachEvent
    Dispach_Event = MakeShareable(new MLB_Dispach_Event);
}

FDispacherHandle UGlobalDispatcher::AddListener_Implement_CPP(EventType Type, const Event_Dispacher& Dispacher,
    EDispachOpportunity DispachOpportunity, float Time)
{
    return  Dispach_Event->AddListener_Implement_CPP(this,Type,Dispacher,DispachOpportunity,Time);
}

FDispacherHandle UGlobalDispatcher::AddListener(EventType Type, UObject* Dispacher, FName DispachFuncName,
    EDispachOpportunity DispachOpportunity, float Time)
{
   return Dispach_Event->AddListener(Type,Dispacher,DispachFuncName,DispachOpportunity,Time);
}

void UGlobalDispatcher::TriggerEvent(EventType Type, FString Param)
{
   Dispach_Event->TriggerEvent(Type,Param);
}

void UGlobalDispatcher::ClearEvent(FString Type)
{
    Dispach_Event->ClearEvent(Type);
}

void UGlobalDispatcher::TriggerEvent_CPP(EventType Type, const AnyType& Param)
{
    Dispach_Event->TriggerEvent_CPP(Type,Param);
}

FDispacherHandle MLB_Dispach_Event::AddListener_Implement_CPP(UObject* WorldContextObject,EventType Type,const Event_Dispacher& Dispacher,EDispachOpportunity DispachOpportunity, float Time)
{
    FDispacherHandle handle = FDispacherHandle::Create();
    if (!Dispacher || WorldContextObject == nullptr)
    {
        //print(TEXT("Dispacher  Is Not Valid"));
        UE_LOG(GlobalDispach, Error, TEXT("Dispach Event Failed"));
        return FDispacherHandle::InvalidCreate();
    }
    std::shared_ptr<Event_Dispacher>_s_ptr = std::make_shared<Event_Dispacher>(Dispacher);
    
    if (!AllDispacher_CPP.Contains(Type))
    {
        FDispatcherEvent DispatcherEvent;
        if (_s_ptr != nullptr)
        {
            DispatcherEvent.Dispacher_vector.push_back(_s_ptr);
        }
        else
        {
            UE_LOG(GlobalDispach,Warning,TEXT("DispachFunc Not Valid"));
            return   FDispacherHandle::InvalidCreate(); 
        }
        
        DispatcherEvent.DispachOpportunity=DispachOpportunity;
        DispatcherEvent.WaitExecTime=Time;
        DispatcherEvent.m_World = WorldContextObject->GetWorld();
        AllDispacher_CPP.Add(Type,DispatcherEvent);
        
        /*
         \@ #TODO KEEP SP ALIVE  OTHERWISE SP FREE WEAK_PTR WILL EXPIRED 
        */
        m_sp_data.emplace(handle.GetHandle(),_s_ptr);
    }
    UE_LOG(GlobalDispach, Log, TEXT("Dispach Event CPP Success"));

    
    return handle;
}

FDispacherHandle MLB_Dispach_Event::AddListener(EventType Type,UObject* Dispacher,FName DispachFuncName,EDispachOpportunity DispachOpportunity, float Time )
{
    FDispacherHandle handle = FDispacherHandle::Create();
    //auto elmentIndex =  AllDispacher_CPP.count(Type);
    if (!AllDispacher_CPP.Contains(Type)) 
    {
        FDispatcherEvent DispatcherEvent;
        if (!Dispacher)
        {
            UE_LOG(GlobalDispach,Warning,TEXT("Dispacher Is Not Valid"));
            return   FDispacherHandle::InvalidCreate();
        }
        DispatcherEvent.DispatcherObject = Dispacher;
        if(DispatcherEvent.DispacherEventNames.Contains(DispachFuncName))
        {
           UE_LOG(GlobalDispach,Warning,TEXT("DispachFunc Has Add In GlobalDispach"));
           return   FDispacherHandle::InvalidCreate();
        }
        DispatcherEvent.DispacherEventNames.Add(DispachFuncName);
        DispatcherEvent.DispachOpportunity =DispachOpportunity;
        DispatcherEvent.WaitExecTime = Time;
        DispatcherEvent.m_World = Dispacher->GetWorld();
        AllDispacher_CPP.Add(Type,DispatcherEvent);
    }
    else
    {
        UE_LOG(GlobalDispach,Warning,TEXT("Listener Has Add"));
        return   FDispacherHandle::InvalidCreate();
    }
       
    UE_LOG(GlobalDispach,Log,TEXT("Listener Dispach Event Success"));
    return handle;

}

void MLB_Dispach_Event::TriggerEvent(EventType Type, FString Param)
{
    FDispatcherEvent* DispachEvent =AllDispacher_CPP.Find(Type);
    if (DispachEvent == nullptr)
    {
        TArray<FString>AllEventType;
        AllDispacher_CPP.GetKeys(AllEventType);
        if (AllEventType.Num()==0)
        {
            UE_LOG(GlobalDispach,Error,TEXT("Present Has Not Event Can Dispach"));
        }
        else
        {
            
            for (const FString& a : AllEventType)
            {
                print(a);
            }
            print(TEXT("Now Can Dispach Event:"));
        }
        UE_LOG(GlobalDispach,Error,TEXT("Find DispachEvent Failed"));
        return;
    }
    if (DispachEvent->DispatcherObject == nullptr)
    {
        ensureMsgf(DispachEvent->DispatcherObject,TEXT("DispachObject Find Failed"));
        return;
    }
    
   
    
    auto LambdaFunc = [this](const FDispatcherEvent& DispachEvent,const FString& Param )
    {
        if (!DispachEvent.DispatcherObject)
            return;
        AActor* actor = Cast<AActor>(DispachEvent.DispatcherObject);
       
        if (actor!=nullptr)
        {
            for (const FName& name : DispachEvent.DispacherEventNames)
            {
                UFunction* Func = actor->FindFunction(name);
                if (Func)
                {
                    if (Param != TEXT(""))
                    {
                        actor->ProcessEvent(Func,static_cast<void*>(new FString(Param))); 
                    }
                    else
                    {
                        actor->ProcessEvent(Func,nullptr);
                    }
                 
                }
            }
        }
       
    };
    switch (DispachEvent->DispachOpportunity)
    {
    case EDispachOpportunity::Directly:
        LambdaFunc(*DispachEvent,Param);
        break;
    case EDispachOpportunity::NextTick:
        //std::function<void()>func;
        //func = LambdaFunc;
        DispachEvent->m_World->GetTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateLambda([this,LambdaFunc,DispachEvent ,Param]()
        {
            LambdaFunc(*DispachEvent,Param);
        })
        );
        break;
    case EDispachOpportunity::WaitTime:
        DispachEvent->m_World->GetTimerManager().SetTimer(DispachEvent->TimerHandle,
        FTimerDelegate::CreateLambda([this,LambdaFunc,DispachEvent ,Param]()
                {
                    LambdaFunc(*DispachEvent,Param);
                }),1.f,false,DispachEvent->WaitExecTime);
        break;
    default: ;
    }
    
}

void MLB_Dispach_Event::TriggerEvent_CPP(EventType Type, const AnyType& Param)
{
    
   FDispatcherEvent* DispachEvent = AllDispacher_CPP.Find(Type);
   
   if (DispachEvent == nullptr)
   {
       TArray<FString>AllEventType;
       AllDispacher_CPP.GetKeys(AllEventType);
       if (AllEventType.Num()==0)
       {
           UE_LOG(GlobalDispach,Error,TEXT("Present Has Not Event Can Dispach"));
       }
       else
       {
           print(TEXT("Now Can Dispach Event:"));
           for (const FString& a : AllEventType)
           {
               print(a);
           }
       }
       ensureMsgf(DispachEvent,TEXT("DispachEventType Find Failed"));
       return;
   }

    //clear data than weak_ptr expired
   for (auto it= m_sp_data.cbegin();it == m_sp_data.cend();++it)
   {
       if (it->second.get() == nullptr)
       {
           m_sp_data.erase(it);
       }
   }
   
   auto LambdaFunc = [this](FDispatcherEvent* DispachEvent,EventType Type, const AnyType& Param)
   {

       //print(FString::FromInt(DispachEvent.Dispacher_vector.capacity()));
       //auto DispachEvent =AllDispacher_CPP.Find(Type);
       for (auto& func :DispachEvent->Dispacher_vector)
       {
     
          if (!func.expired())
          {
               auto ptr = func.lock();
               ptr->operator()(Param);
          }
           else
           {
               UE_LOG(GlobalDispach,Error,TEXT("Exec Func Is expired"));
           }
       }
   };

   switch (DispachEvent->DispachOpportunity)
   {
   case EDispachOpportunity::Directly:
        LambdaFunc.operator()(DispachEvent,Type,Param);
        //TriggerEvent_CPP_Internal(DispachEvent, Type, Param);
        break;
   case EDispachOpportunity::NextTick:
       // std::function<void(FDispatcherEvent* DispachEvent,EventType Type, const AnyType& Param)>func;
        //func = LambdaFunc;
        DispachEvent->m_World->GetTimerManager().SetTimerForNextTick([this,LambdaFunc,DispachEvent,Type,Param]()
        {
            LambdaFunc.operator()(DispachEvent,Type,Param);
        });
       break;
   case EDispachOpportunity::WaitTime:
       
       DispachEvent->m_World->GetTimerManager().SetTimer(DispachEvent->TimerHandle,FTimerDelegate::CreateLambda( [this,LambdaFunc,DispachEvent,Type,Param]()
       {
           LambdaFunc.operator()(DispachEvent,Type,Param);
       }
       ),1.f,false,DispachEvent->WaitExecTime);
       break;
   default: ;
   }
}

void MLB_Dispach_Event::TriggerEvent_CPP_Internal(FDispatcherEvent* DispachEvent, EventType Type, const AnyType& Param)
{
    //auto DispachEvent =AllDispacher_CPP.Find(Type);
    for (auto func :DispachEvent->Dispacher_vector)
    {
        if (!func.expired())
        {
            auto ptr = func.lock();
            ptr->operator()(Param);
        }
        else
        {
            UE_LOG(GlobalDispach,Error,TEXT("Exec Func Is expired"));
        }
    }
}


void MLB_Dispach_Event::ClearEvent(EventType Type)
{
	auto event =AllDispacher_CPP.Find(Type);
    if (event!=nullptr)
    {
        if(event->TimerHandle.IsValid())
            event->m_World->GetTimerManager().ClearTimer(event->TimerHandle);
        AllDispacher_CPP.Remove(Type);
    }
}