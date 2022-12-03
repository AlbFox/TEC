// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "EasyAIKeyFunctions.h"
#include "EasyAITaskManager.h"
#include "EasyAITask.h"
#include "EasyAIQueryObject.h"
#include "AIUtilityFunctions.h"

//Helpers

static UEasyAIKeyStorage* GetKeyStorage(UObject* Object, const bool bShared)  
{
	const UEasyAITaskManager* Manager = UAIUtilityFunctions::FindTaskManager(Object);
	if (Manager)
	{
		return bShared ? Manager->SharedKeys : Manager->LocalKeys;
	}

	return nullptr;
}

//Getters

bool UEasyAIKeyFunctions::GetBoolKey(UObject* Object, FGameplayTag Key, bool bShared)
{
	const UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	return Keys ? Keys->GetBoolKey(Key) : false;
}

int UEasyAIKeyFunctions::GetIntKey(UObject* Object, FGameplayTag Key, bool bShared)
{
	const UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	return Keys ? Keys->GetIntKey(Key) : 0;
}

float UEasyAIKeyFunctions::GetFloatKey(UObject* Object, FGameplayTag Key, bool bShared)
{
	const UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	return Keys ? Keys->GetFloatKey(Key) : 0;
}

uint8 UEasyAIKeyFunctions::GetByteKey(UObject* Object, FGameplayTag Key, bool bShared)
{
	const UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	return Keys ? Keys->GetByteKey(Key) : 0;
}

FVector UEasyAIKeyFunctions::GetVectorKey(UObject* Object, FGameplayTag Key, bool bShared)
{
	const UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	return Keys ? Keys->GetVectorKey(Key) : FVector::ZeroVector;
}

FRotator UEasyAIKeyFunctions::GetRotatorKey(UObject* Object, FGameplayTag Key, bool bShared)
{
	const UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	return Keys ? Keys->GetRotatorKey(Key) : FRotator::ZeroRotator;
}

FName UEasyAIKeyFunctions::GetNameKey(UObject* Object, FGameplayTag Key, bool bShared)
{
	const UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	return Keys ? Keys->GetNameKey(Key) : FName();
}

FString UEasyAIKeyFunctions::GetStringKey(UObject* Object, FGameplayTag Key, bool bShared)
{
	const UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	return Keys ? Keys->GetStringKey(Key) : FString();
}

UClass* UEasyAIKeyFunctions::GetClassKey(UObject* Object, FGameplayTag Key, bool bShared)
{
	const UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	return Keys ? Keys->GetClassKey(Key) : NULL;
}

UObject* UEasyAIKeyFunctions::GetObjectKey(UObject* Object, FGameplayTag Key, bool bShared, bool& bValid)
{
	const UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	if (Keys)
	{
		//The GetKey K2 function only calls GetObjectKey, so we cast to actor here in case it has an Actor input.
		AActor* ActorVal = Keys->GetActorKey(Key);
		if (ActorVal)
		{
			bValid = true;
			return ActorVal;
		}

		UObject* ObjectVal = Keys->GetObjectKey(Key);
		if (ObjectVal)
		{
			bValid = true;
			return ObjectVal;
		}
	}

	bValid = false;
	return nullptr; 
}

AActor* UEasyAIKeyFunctions::GetActorKey(UObject* Object, FGameplayTag Key, bool bShared, bool& bValid)
{
	const UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	if (Keys)
	{
		AActor* ActorKey = Keys->GetActorKey(Key);
		if (ActorKey)
		{
			bValid = true;
			return ActorKey;
		}
	}

	bValid = false;
	return nullptr;
}


//Setters

void UEasyAIKeyFunctions::SetBoolKey(UObject* Object, FGameplayTag Key, bool Value, bool bShared)
{
	UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	if (Keys)
	{
		Keys->SetBoolKey(Key, Value);
	}
}

void UEasyAIKeyFunctions::SetIntKey(UObject* Object, FGameplayTag Key, int Value, bool bShared)
{
	UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	if (Keys)
	{
		Keys->SetIntKey(Key, Value);
	}
}

void UEasyAIKeyFunctions::SetFloatKey(UObject* Object, FGameplayTag Key, float Value, bool bShared)
{
	UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	if (Keys)
	{
		Keys->SetFloatKey(Key, Value);
	}
}

void UEasyAIKeyFunctions::SetByteKey(UObject* Object, FGameplayTag Key, uint8 Value, bool bShared)
{
	UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	if (Keys)
	{
		Keys->SetByteKey(Key, Value);
	}
}

void UEasyAIKeyFunctions::SetVectorKey(UObject* Object, FGameplayTag Key, FVector Value, bool bShared)
{
	UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	if (Keys)
	{
		Keys->SetVectorKey(Key, Value);
	}
}

void UEasyAIKeyFunctions::SetRotatorKey(UObject* Object, FGameplayTag Key, FRotator Value, bool bShared)
{
	UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	if (Keys)
	{
		Keys->SetRotatorKey(Key, Value);
	}
}

void UEasyAIKeyFunctions::SetNameKey(UObject* Object, FGameplayTag Key, FName Value, bool bShared)
{
	UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	if (Keys)
	{
		Keys->SetNameKey(Key, Value);
	}
}

void UEasyAIKeyFunctions::SetStringKey(UObject* Object, FGameplayTag Key, FString Value, bool bShared)
{
	UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	if (Keys)
	{
		Keys->SetStringKey(Key, Value);
	}
}

void UEasyAIKeyFunctions::SetClassKey(UObject* Object, FGameplayTag Key, UClass* Value, bool bShared)
{
	UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);
	if (Keys)
	{
		Keys->SetClassKey(Key, Value);
	}
}

void UEasyAIKeyFunctions::SetObjectKey(UObject* Object, FGameplayTag Key, UObject* Value, bool bShared)
{
	UEasyAIKeyStorage* Keys = GetKeyStorage(Object, bShared);

	if (Keys)
	{
		//The SetKey K2 function only calls SetObjectKey, so we cast to actor here in case it has an Actor input.
		if (Value)
		{
			AActor* ActorVal = Cast<AActor>(Value);
			if (ActorVal)
			{
				Keys->SetActorKey(Key, ActorVal);
			}
			else
			{
				Keys->SetObjectKey(Key, Value);
			}
		}
		else
		{
			//Clear both. No way around it.
			Keys->SetActorKey(Key, nullptr);
			Keys->SetObjectKey(Key, nullptr);
		}
	}
}

void UEasyAIKeyFunctions::SetActorKey(UObject* Object, FGameplayTag Key, AActor* Value, bool bShared)
{
	SetObjectKey(Object, Key, Value, bShared);
}

