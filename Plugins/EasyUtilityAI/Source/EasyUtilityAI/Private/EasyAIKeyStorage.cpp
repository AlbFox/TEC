// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "EasyAIKeyStorage.h"
#include "EasyAITaskManager.h"

//Getters

bool UEasyAIKeyStorage::GetBoolKey(const FGameplayTag& Key) const
{
	const uint8* Result = BoolMap.Find(Key);
	if (Result)
	{
		return *Result == 1;
	}

	return false;
}

int UEasyAIKeyStorage::GetIntKey(const FGameplayTag& Key) const
{
	const int* Result = IntMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return 0;
}

float UEasyAIKeyStorage::GetFloatKey(const FGameplayTag& Key) const
{
	const float* Result = FloatMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return 0;
}

uint8 UEasyAIKeyStorage::GetByteKey(const FGameplayTag& Key) const
{
	const uint8* Result = ByteMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return 0;
}

FVector UEasyAIKeyStorage::GetVectorKey(const FGameplayTag& Key) const
{
	const FVector* Result = VectorMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return FVector::ZeroVector;
}

FRotator UEasyAIKeyStorage::GetRotatorKey(const FGameplayTag& Key) const
{
	const FRotator* Result = RotatorMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return FRotator::ZeroRotator;
}

FName UEasyAIKeyStorage::GetNameKey(const FGameplayTag& Key) const
{
	const FName* Result = NameMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return FName();
}

FString UEasyAIKeyStorage::GetStringKey(const FGameplayTag& Key) const
{
	const FString* Result = StringMap.Find(Key);
	if (Result)
	{
		return *Result;
	}

	return FString();
}

UClass* UEasyAIKeyStorage::GetClassKey(const FGameplayTag& Key) const
{
	return ClassMap.FindRef(Key);
}

UObject* UEasyAIKeyStorage::GetObjectKey(const FGameplayTag& Key) const
{
	return ObjectMap.FindRef(Key);
}

AActor* UEasyAIKeyStorage::GetActorKey(const FGameplayTag& Key) const
{
	return ActorMap.FindRef(Key);
}

//Setters

void UEasyAIKeyStorage::SetBoolKey(const FGameplayTag& Key, const bool& Value)
{
	BoolMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetIntKey(const FGameplayTag& Key, const int& Value)
{
	IntMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetFloatKey(const FGameplayTag& Key, const float& Value)
{
	FloatMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetByteKey(const FGameplayTag& Key, const uint8& Value)
{
	ByteMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetVectorKey(const FGameplayTag& Key, const FVector& Value)
{
	VectorMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetRotatorKey(const FGameplayTag& Key, const FRotator& Value)
{
	RotatorMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetNameKey(const FGameplayTag& Key, const FName& Value)
{
	NameMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetStringKey(const FGameplayTag& Key, const FString& Value)
{
	StringMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetClassKey(const FGameplayTag& Key, UClass* Value)
{
	ClassMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetObjectKey(const FGameplayTag& Key, UObject* Value)
{
	ObjectMap.Add(Key, Value);
}

void UEasyAIKeyStorage::SetActorKey(const FGameplayTag& Key, AActor* Value)
{
	ActorMap.Add(Key, Value);
}
