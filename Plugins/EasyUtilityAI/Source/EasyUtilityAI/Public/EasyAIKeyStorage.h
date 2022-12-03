// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "AIUtilityData.h"
#include "UObject/NoExportTypes.h"
#include "EasyAIKeyStorage.generated.h"

UCLASS()
class EASYUTILITYAI_API UEasyAIKeyStorage : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(Transient)
	TMap<FGameplayTag, uint8> BoolMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, float> FloatMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, int> IntMap;
	
	UPROPERTY(Transient)
	TMap<FGameplayTag, uint8> ByteMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FVector> VectorMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FRotator> RotatorMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FName> NameMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FString> StringMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, UClass*> ClassMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, UObject*> ObjectMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, AActor*> ActorMap;

public:

	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = "true"))
	bool GetBoolKey(const FGameplayTag& Key) const;

	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = "true"))
	int GetIntKey(const FGameplayTag& Key) const;

	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = "true"))
	float GetFloatKey(const FGameplayTag& Key) const;

	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = "true"))
	uint8 GetByteKey(const FGameplayTag& Key) const;

	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = "true"))
	FVector GetVectorKey(const FGameplayTag& Key) const;

	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = "true"))
	FRotator GetRotatorKey(const FGameplayTag& Key) const;

	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = "true"))
	FName GetNameKey(const FGameplayTag& Key) const;
	
	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = "true"))
	FString GetStringKey(const FGameplayTag& Key) const;

	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = "true"))
	UClass* GetClassKey(const FGameplayTag& Key) const;

	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = "true"))
	UObject* GetObjectKey(const FGameplayTag& Key) const;

	UFUNCTION(BlueprintPure, meta = (BlueprintInternalUseOnly = "true"))
	AActor* GetActorKey(const FGameplayTag& Key) const;

public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	void SetBoolKey(const FGameplayTag& Key, const bool& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	void SetIntKey(const FGameplayTag& Key, const int& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	void SetFloatKey(const FGameplayTag& Key, const float& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	void SetByteKey(const FGameplayTag& Key, const uint8& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	void SetVectorKey(const FGameplayTag& Key, const FVector& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	void SetRotatorKey(const FGameplayTag& Key, const FRotator& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	void SetNameKey(const FGameplayTag& Key, const FName& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	void SetStringKey(const FGameplayTag& Key, const FString& Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	void SetClassKey(const FGameplayTag& Key, UClass* Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	void SetObjectKey(const FGameplayTag& Key, UObject* Value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	void SetActorKey(const FGameplayTag& Key, AActor* Value);
};

