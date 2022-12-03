// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "EasyAIKeyFunctionsK2.h"
#include "EasyAIGetKeyK2.generated.h"

UCLASS()
class EASYUTILITYAIEDITOR_API UEasyAIGetKeyK2 : public UEasyAIKeyFunctionsK2
{
	GENERATED_BODY()

	UFunction* GetKeyFunction() const;
	UEdGraphPin* GetResultPin() const;

	void ChangeResultPin();

	virtual void AllocateDefaultPins() override;
	virtual void PostReconstructNode() override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetMenuCategory() const override;

	virtual bool IsNodePure() const override { return true; }
};
