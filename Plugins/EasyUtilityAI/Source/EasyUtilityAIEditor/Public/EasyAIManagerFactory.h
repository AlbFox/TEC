// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "EasyAIManagerFactory.generated.h"

UCLASS()
class EASYUTILITYAIEDITOR_API UEasyAIManagerFactory : public UFactory
{
	GENERATED_BODY()

	UEasyAIManagerFactory();

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	// End of UFactory interface
};
