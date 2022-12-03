// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "EasyAIGetKeyK2.h"
#include "AIUtilityFunctions.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_CallFunction.h"
#include "EasyAITask.h"
#include "AIUtilityData.h"
#include "EasyAIKeyFunctions.h"

#define LOCTEXT_NAMESPACE "K2GetKey"

void UEasyAIGetKeyK2::AllocateDefaultPins()
{
    Super::AllocateDefaultPins();

    const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

    //Gameplay Tag
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FGameplayTag::StaticStruct(), K2KeyHelper::KeyName);

    //Wilcard output
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UEdGraphSchema_K2::PN_ReturnValue);

    //Shared check
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, K2KeyHelper::SharedName);
}

UEdGraphPin* UEasyAIGetKeyK2::GetResultPin() const
{
    UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
    check(Pin->Direction == EGPD_Output);
    return Pin;
}

void UEasyAIGetKeyK2::ChangeResultPin()
{
    UEdGraphPin* Pin = GetResultPin();
    if (Pin->LinkedTo.Num() > 0)
    {
        UEdGraphPin* ToPin = Pin->LinkedTo[0];
        Pin->PinType = ToPin->PinType;
        Pin->Modify();
    }
    else
    {
        ResetPinToWildcard(Pin);
    }
}

void UEasyAIGetKeyK2::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
    Super::PinDefaultValueChanged(ChangedPin);
}

void UEasyAIGetKeyK2::PinConnectionListChanged(UEdGraphPin* Pin)
{
    Super::PinConnectionListChanged(Pin);

    if (Pin && Pin->PinName == UEdGraphSchema_K2::PN_ReturnValue)
    {
        ChangeResultPin();

        bool bAllowed = AllowWildcard(Pin);
        if (!bAllowed)
        {
            ResetPinToWildcard(Pin);
        }
    }
}

void UEasyAIGetKeyK2::PostReconstructNode()
{
    Super::PostReconstructNode();

    UEdGraphPin* InPin = GetResultPin();
    if (InPin)
    {
        ChangeResultPin();
        RestorePinConnections(GetResultPin());
    }
}

UFunction* UEasyAIGetKeyK2::GetKeyFunction() const
{
    UEdGraphPin* OutPin = GetResultPin();
    if (OutPin)
    { 
        const FName PinCat = OutPin->PinType.PinCategory;
        const TWeakObjectPtr<UObject> PinObj = OutPin->PinType.PinSubCategoryObject;

        FName FunctionName;

        if (PinCat == UEdGraphSchema_K2::PC_Boolean)
        {
            FunctionName = "GetBoolKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Int)
        {
            FunctionName = "GetIntKey";
        } 
        else if (PinCat == UEdGraphSchema_K2::PC_Float)
        {
            FunctionName = "GetFloatKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Byte)
        {
            FunctionName = "GetByteKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Struct)
        {
            //Get the right var type as struct
            if (PinObj == TBaseStructure<FVector>::Get())
            {
                FunctionName = "GetVectorKey";
            }
            else if (PinObj == TBaseStructure<FRotator>::Get())
            {
                FunctionName = "GetRotatorKey";
            }       
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Name)
        {
            FunctionName = "GetNameKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_String)
        {
            FunctionName = "GetStringKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Class)
        {
            FunctionName = "GetClassKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Object)
        {
            FunctionName = "GetObjectKey";
        }
        else
        {
            FunctionName = "GetBoolKey";
        }
    
        UFunction* ReturnFunc = UEasyAIKeyFunctions::StaticClass()->FindFunctionByName(FunctionName);
        return ReturnFunc;
    }

    return nullptr;
}

void UEasyAIGetKeyK2::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    UEdGraphPin* KeyPin = FindPinChecked(K2KeyHelper::KeyName);
    if (KeyPin->GetDefaultAsString().IsEmpty())
    {
        return;
    }

    const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

    //Get function
    UFunction* BlueprintFunction = GetKeyFunction();
    if (!BlueprintFunction)
    {
        return;
    }

    UK2Node_CallFunction* CallFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    if (!CallFunction)
    {
        return;
    }

    CallFunction->SetFromFunction(BlueprintFunction);
    CallFunction->AllocateDefaultPins();
    CompilerContext.MessageLog.NotifyIntermediateObjectCreation(CallFunction, this);

    //Key Tag
    UEdGraphPin* FunctionKeyPin = CallFunction->FindPinChecked(K2KeyHelper::KeyName);
    CompilerContext.MovePinLinksToIntermediate(*KeyPin, *FunctionKeyPin);

    //Shared bool
    UEdGraphPin* SharedPin = FindPinChecked(K2KeyHelper::SharedName);
    UEdGraphPin* FunctionSharedPin = CallFunction->FindPinChecked(K2KeyHelper::FunctionSharedName);
    CompilerContext.MovePinLinksToIntermediate(*SharedPin, *FunctionSharedPin);

    //Return Var
    UEdGraphPin* ReturnPin = GetResultPin();
    UEdGraphPin* FunctionReturnPin = CallFunction->GetReturnValuePin();
    FunctionReturnPin->PinType = ReturnPin->PinType; //So we get the correct type for Actors etc.
    CompilerContext.MovePinLinksToIntermediate(*ReturnPin, *FunctionReturnPin);

    BreakAllNodeLinks();
}

FText UEasyAIGetKeyK2::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("K2GetKey_Title", "Get Runtime Key Value");
}

FText UEasyAIGetKeyK2::GetTooltipText() const
{
    return LOCTEXT("K2GetKey_Tooltip",  "Returns a Runtime Key Value from Gameplay Tag.");
}

FText UEasyAIGetKeyK2::GetMenuCategory() const
{
    return LOCTEXT("K2GetKey_MenuCategory", "Easy Utility AI | Keys");
}

#undef LOCTEXT_NAMESPACE