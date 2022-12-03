// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"
#include "EasyAITask.h"
#include "EasyAITaskFactory.generated.h"

UCLASS(hidecategories=Object)
class EASYUTILITYAIEDITOR_API UEasyAITaskFactory : public UFactory
{
	GENERATED_BODY()

public:

	UEasyAITaskFactory();

	UPROPERTY(EditAnywhere, Category = EasyAITaskFactory)
	TSubclassOf<UEasyAITask> TaskClass;

	//~ Begin UFactory Interface
	virtual bool ConfigureProperties() override;


	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End of UFactory interface
};
