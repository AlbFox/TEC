// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "EasyAIQueryObject.h"
#include "EasyAIQueryFactory.generated.h"


UCLASS()
class EASYUTILITYAIEDITOR_API UEasyAIQueryFactory : public UFactory
{
	GENERATED_BODY()
	
	UEasyAIQueryFactory();

	UPROPERTY(EditAnywhere, Category = EasyAIQueryFactory)
	TSubclassOf<UEasyAIQueryObject> QueryClass;

	//~ Begin UFactory Interface
	virtual bool ConfigureProperties() override;

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End of UFactory interface
};
