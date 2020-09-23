// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include <functional>
#include <map>
#include <memory>
#include <vector>



#include "AnyType.h"
#include "Engine/EngineTypes.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ObjectMacros.h"

#include "GlobalDispatcher.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(GlobalDispach,Log,All);





UENUM(BlueprintType)
enum class EDispachOpportunity : uint8
{
	Directly	,//
    NextTick	,//
    WaitTime	//
};


USTRUCT(BlueprintType)
struct FDispatcherEvent
{
	GENERATED_BODY()

	FDispatcherEvent()
	{
		DispatcherObject = nullptr;
		DispachOpportunity =EDispachOpportunity::Directly;
	};
	/*FDispatcherEvent(FDispatcherEvent& other)
	{
		*this = other;
	};*/
	/*FDispatcherEvent(FDispatcherEvent && other)
	{
		*this = std::move(other);
	};*/
	/*FDispatcherEvent& operator=(FDispatcherEvent& other)
	{
		return other;
	};
	FDispatcherEvent& operator=(FDispatcherEvent&& other)
	{
		*this = std::move(other);
		return *this;
	};*/
	/*
	\ CPP
	*/
	using Event_Dispacher = std::function<void(const AnyType& arg)>;
	using Event_Dispacher_vector = std::vector<std::weak_ptr<Event_Dispacher>>;
	UWorld* m_World;
	Event_Dispacher_vector Dispacher_vector; 
	/*
	\ BP
	*/
	// TODO  CPP Don't  Use This Object
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category = ConfigDispacher)
	UObject* DispatcherObject;
	UPROPERTY(BlueprintReadWrite , Category = ConfigDispacher)
	TArray<FName>DispacherEventNames;
	
	//**************************************
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = ConfigDispacher)
	EDispachOpportunity DispachOpportunity;

	//This Wait Time Just use  for DispachOpportunity == DispachOpportunity::WaitTime
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = ConfigDispacher)
	float WaitExecTime;
	UPROPERTY(BlueprintReadOnly, Category = ConfigDispacher)
	FTimerHandle TimerHandle;
	
	
	
};

USTRUCT(BlueprintType)
struct FDispacherHandle
{

	GENERATED_BODY()

	FDispacherHandle() = default;

	
	bool isValidhandle()const{return ListenHandle > 0;};

	int64 GetHandle()const {return ListenHandle;};
	private:
	FDispacherHandle(int64 InHandle)
        :ListenHandle(InHandle)
	{
		bIsValidHandle = ListenHandle>0 ? true : false;
	};

	mutable int64 ListenHandle;

	static FDispacherHandle Create()
	{
		return FDispacherHandle(++s_ListenHandle);
	}
	static FDispacherHandle InvalidCreate()
	{
		return FDispacherHandle(i_ListenHandle);
	}

public:
	UPROPERTY(BlueprintReadOnly,Category = DispacherHandle)
	bool bIsValidHandle;
private:
	static int64 s_ListenHandle;
	static int64 i_ListenHandle;
	//Get Authority Creat Handle
	friend class MLB_Dispach_Event;
};


/**
 * 
 */

using EventType =FString;
using Event_Dispacher = std::function<void(const AnyType& arg)>;
class MLB_Dispach_Event;
UCLASS()
class G_DISPACHER_API UGlobalDispatcher : public UObject
{
	GENERATED_BODY()

		UGlobalDispatcher(){};

public:
	void Init();

	FDispacherHandle AddListener_Implement_CPP(FString Type,const Event_Dispacher& Dispacher,EDispachOpportunity DispachOpportunity = EDispachOpportunity::Directly , float Time = -1);
	UFUNCTION(BlueprintCallable,Category = Dispach)
    FDispacherHandle AddListener(FString Type,UObject* Dispacher,FName DispachFuncName = NAME_None,EDispachOpportunity DispachOpportunity = EDispachOpportunity::Directly , float Time = -1);
	UFUNCTION(BlueprintCallable,Category = Dispach)
    void TriggerEvent(FString Type,FString Param);
	UFUNCTION(BlueprintCallable,Category = Dispach)
	void ClearEvent(FString Type);
	void TriggerEvent_CPP(FString Type,const AnyType& Param);
	
	MLB_Dispach_Event* GetDispacher()const
	{
		if (Dispach_Event.IsValid())
		{
			return Dispach_Event.Get();
		}
		return nullptr;
	};



	/*
	\TEST
	*/
	
	TFunction<void()>a;
	using Event_Dispachers = std::function<void()>;
	using Event_Dispacher_vector = std::vector<std::weak_ptr<Event_Dispachers>>;
	UWorld* m_World;
	Event_Dispacher_vector Dispacher_vector;
	
private:
	TSharedPtr<MLB_Dispach_Event,ESPMode::ThreadSafe>Dispach_Event;
};


class MLB_DispacherHandle;

/**
* 
*/
class G_DISPACHER_API MLB_Dispach_Event
{
	
	public:
	//MLB_Dispach_Event() = default;
	~MLB_Dispach_Event() {};
	FDispacherHandle AddListener_Implement_CPP(UObject* WorldContextObject,EventType Type,const Event_Dispacher& Dispacher,EDispachOpportunity DispachOpportunity = EDispachOpportunity::Directly , float Time = -1);
	//UFUNCTION(BlueprintCallable,Category = Dispach)
	FDispacherHandle AddListener(EventType Type,UObject* Dispacher,FName DispachFuncName = NAME_None,EDispachOpportunity DispachOpportunity = EDispachOpportunity::Directly , float Time = -1);
	//UFUNCTION(BlueprintCallable,Category = Dispach)
	void TriggerEvent(EventType Type,FString Param);
	void TriggerEvent_CPP(EventType Type,const AnyType& Param);
	void TriggerEvent_CPP_Internal(FDispatcherEvent* DispachEvent,EventType Type, const AnyType& Param);
	void ClearEvent(EventType Type);

	using _sp_func = std::shared_ptr<Event_Dispacher>;
private:
	//Avoid GC
	UPROPERTY()
	TMap<EventType,FDispatcherEvent>AllDispacher_CPP;
	std::map<int64,_sp_func>m_sp_data;
	bool bExecFuncLocked;
	//std::map<EventType,UObject>AllDispacher_BP;
};


/*class MLB_DispacherHandle
{
public:

	bool isValidhandle()const{return ListenHandle > 0;};

	int64 GetHandle()const {return ListenHandle;};
private:
	MLB_DispacherHandle(int64 InHandle)
        :ListenHandle(InHandle)
	{
	};

	mutable int64 ListenHandle;

	static MLB_DispacherHandle Create()
	{
		return MLB_DispacherHandle(++s_ListenHandle);
	}
	static MLB_DispacherHandle InvalidCreate()
	{
		return MLB_DispacherHandle(i_ListenHandle);
	}
private:
	static int64 s_ListenHandle;
	static int64 i_ListenHandle;
	//Get Authority Creat Handle
	friend class MLB_Dispach_Event;
};*/