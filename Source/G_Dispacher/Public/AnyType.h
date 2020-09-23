// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include <memory>
#include <typeindex>

#define print(text) GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::White,text) 
/**
 * 
 */
class G_DISPACHER_API RTTIAnyType
{
public:
    //ctor
	RTTIAnyType()
	    :mlb_type(std::type_index(typeid(void)))
    {};
    // cp ctor
    RTTIAnyType(RTTIAnyType& Other)
        :mlb_type(Other.mlb_type),ReallyPtr(Other.copy())
    {}

    RTTIAnyType(const RTTIAnyType& Other)
        :mlb_type(Other.mlb_type), ReallyPtr(Other.copy())
    {}
    //mv ctor
    RTTIAnyType(RTTIAnyType&& Other)
        :mlb_type(Other.mlb_type),ReallyPtr(Other.copy())
    {}
    
    RTTIAnyType& operator=(RTTIAnyType& Other)
    {
        mlb_type = Other.mlb_type;
        ReallyPtr = Other.copy();
        return  *this;
    }

    RTTIAnyType& operator=(const RTTIAnyType& Other)
    {
        mlb_type = Other.mlb_type;
        ReallyPtr = Other.copy();
        return  *this;
    }
    //语法检测
    //std::enable_if<!std::is_same<typename std::decay<U>::type,
    //RTTIAnyType>::value>::type
    //比较std::decay<U>::type ，RTTIAnyType类型不为空
    //std::forward 完美转发数据
    //std::decay<>::type  完美坍缩类型
    template<typename U , typename = typename std::enable_if<!std::is_same<typename std::decay<U>::type,
    RTTIAnyType>::value>::type>
    RTTIAnyType(U&& Value)
        :ReallyPtr(new DrivedPtr<typename std::decay<U>::type>(std::forward<U>(Value)))
        ,mlb_type(std::type_index(typeid(typename std::decay<U>::type)))
    {
        //c17 才支持static_assert(!std::is_same<typename std::decay<U>::type,const char*>::value）
        //
        static_assert(!std::is_same<typename std::decay<U>::type,const char*>::value,"c17 编译可以不加这一段");
    }

    template<typename U>
    bool IsSameType()const
    {
        bool ret = mlb_type == std::type_index(typeid(U));
        //std::typeInfo Compare Not Guarantee , std::type_index Guarantee
        return mlb_type == std::type_index(typeid(U)); 
    }

    //两个类构造时完成了两件事 构建ReallyPtr管理指针 ， 将真实的Value用std::forward指向Realptr
    //                       RTTIAnyType 构造         DrivedPtr 构造
    //new DrivedPtr<typename std::decay<U>::type>(std::forward<U>(Value))
    //再由AnyCast 将指针取出还原
    template<typename U>
    U AnyCast()const
    {
        if (IsSameType<U>())
        {
           auto ret = static_cast<DrivedPtr<U>*>(ReallyPtr.get());
           return ret->Realptr;
        }
        ensureMsgf(false, TEXT("CONVERT TO REALLY TYPE FAILED"));
        return U();
    };
    template<typename U>
    bool IsValidType()
    {
        /*
         *\ !!强制布尔值
         */
        return !!ReallyPtr;
    }

private:
   
    class Base;
    // Base 类型的智能管理指针，接管生命周期，无法直接赋值 用std::make_unique赋予需要管理的指针 
    using BasePtr = std::unique_ptr<Base>;
    class Base
    {
    public:
        virtual ~Base() {};
        virtual BasePtr copy()const = 0;
    
    };

    //std::forward 类型转发
    template<typename T>
    class DrivedPtr :public Base
    {
    public:
        template<typename U>
        DrivedPtr(U&& Other)
            :Realptr(std::forward<U>(Other))
        {};

        ~DrivedPtr(){};
    
        BasePtr copy() const
        {
            return std::make_unique<DrivedPtr<T>>(Realptr);
        }



        T Realptr;
    };

     BasePtr copy() const
    {
        if (ReallyPtr)
        {
            return ReallyPtr->copy();
        }
         return nullptr;
    }

private:
    std::type_index mlb_type;
    BasePtr ReallyPtr;
};



#if PLATFORM_WINDOWS
using AnyType = RTTIAnyType;
#endif

