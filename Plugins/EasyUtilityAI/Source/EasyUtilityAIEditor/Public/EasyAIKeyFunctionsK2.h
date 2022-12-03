// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EasyAIKeyFunctionsK2.generated.h"

//Base Class for Get/Set Key K2 functions

namespace K2KeyHelper
{
	const FName TypeName	= "Type";
	const FName ValueName	= "Value";
	const FName KeyName		= "Key";
	const FName SharedName	= "Shared";
	const FName FunctionSharedName = "bShared";
}

UCLASS(abstract)
class EASYUTILITYAIEDITOR_API UEasyAIKeyFunctionsK2 : public UK2Node
{
	GENERATED_BODY()


	virtual bool IsCompatibleWithGraph(const UEdGraph* TargetGraph) const override;
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;

	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	virtual bool CanSplitPin(const UEdGraphPin* Pin)  const override { return false; }

protected:

	virtual void RestorePinConnections(UEdGraphPin* RestorePin);

	void ResetPinToWildcard(UEdGraphPin* Pin);

	bool AllowWildcard(UEdGraphPin* Pin);
};

