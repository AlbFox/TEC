// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AIUtilityData.h"
#include "EasyAIKeyStorage.h"
#include "EasyAIKeyFunctions.generated.h"

UCLASS(meta = (RestrictedToClasses = "EasyAITask, EasyAITaskManager, Pawn, AIController, EasyAIQueryObject, AnimInstance"))
class EASYUTILITYAI_API UEasyAIKeyFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Get Runtime Bool", WorldContext = "Object"))
	static bool GetBoolKey(UObject* Object, FGameplayTag Key, bool bShared);

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Get Runtime Int", WorldContext = "Object"))
	static int GetIntKey(UObject* Object, FGameplayTag Key, bool bShared);

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Get Runtime Float", WorldContext = "Object"))
	static float GetFloatKey(UObject* Object, FGameplayTag Key, bool bShared);

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Get Runtime Byte", WorldContext = "Object"))
	static uint8 GetByteKey(UObject* Object, FGameplayTag Key, bool bShared);

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Get Runtime Vector", WorldContext = "Object"))
	static FVector GetVectorKey(UObject* Object, FGameplayTag Key, bool bShared);

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Get Runtime Rotator", WorldContext = "Object"))
	static FRotator GetRotatorKey(UObject* Object, FGameplayTag Key, bool bShared);

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Get Runtime Name", WorldContext = "Object"))
	static FName GetNameKey(UObject* Object, FGameplayTag Key, bool bShared);

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Get Runtime String", WorldContext = "Object"))
	static FString GetStringKey(UObject* Object, FGameplayTag Key, bool bShared);

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Get Runtime Class", WorldContext = "Object"))
	static UClass* GetClassKey(UObject* Object, FGameplayTag Key, bool bShared);

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Get Runtime Object", WorldContext = "Object"))
	static UObject* GetObjectKey(UObject* Object, FGameplayTag Key, bool bShared, bool& bValid);

	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Get Runtime Actor", WorldContext = "Object"))
	static AActor* GetActorKey(UObject* Object, FGameplayTag Key, bool bShared, bool& bValid);

public:

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Set Runtime Bool", WorldContext = "Object"))
	static void SetBoolKey(UObject* Object, FGameplayTag Key, bool Value, bool bShared);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Set Runtime Int", WorldContext = "Object"))
	static void SetIntKey(UObject* Object, FGameplayTag Key, int Value, bool bShared);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Set Runtime Float", WorldContext = "Object"))
	static void SetFloatKey(UObject* Object, FGameplayTag Key, float Value, bool bShared);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Set Runtime Byte", WorldContext = "Object"))
	static void SetByteKey(UObject* Object, FGameplayTag Key, uint8 Value, bool bShared);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Set Runtime Vector", WorldContext = "Object"))
	static void SetVectorKey(UObject* Object, FGameplayTag Key, FVector Value, bool bShared);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Set Runtime Rotator", WorldContext = "Object"))
	static void SetRotatorKey(UObject* Object, FGameplayTag Key, FRotator Value, bool bShared);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Set Runtime Name", WorldContext = "Object"))
	static void SetNameKey(UObject* Object, FGameplayTag Key, FName Value, bool bShared);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Set Runtime String", WorldContext = "Object"))
	static void SetStringKey(UObject* Object, FGameplayTag Key, FString Value, bool bShared);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Set Runtime Class", WorldContext = "Object"))
	static void SetClassKey(UObject* Object, FGameplayTag Key, UClass* Value, bool bShared);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Set Runtime Object", WorldContext = "Object"))
	static void SetObjectKey(UObject* Object, FGameplayTag Key, UObject* Value, bool bShared);

	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Keys Raw", meta = (DisplayName = "Set Runtime Actor", WorldContext = "Object"))
	static void SetActorKey(UObject* Object, FGameplayTag Key, AActor* Value, bool bShared);
};
