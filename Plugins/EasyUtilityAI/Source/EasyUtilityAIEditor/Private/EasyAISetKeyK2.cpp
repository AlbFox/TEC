// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "EasyAISetKeyK2.h"
#include "AIUtilityFunctions.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_CallFunction.h"
#include "EasyAITask.h"
#include "AIUtilityData.h"
#include "EasyAIKeyFunctions.h"

#define LOCTEXT_NAMESPACE "K2SetKey"

void UEasyAISetKeyK2::AllocateDefaultPins()
{
    Super::AllocateDefaultPins();

    const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
   
    //Execs
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

    //Gameplay Tag
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FGameplayTag::StaticStruct(), K2KeyHelper::KeyName);

    //Wildcard value
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, K2KeyHelper::ValueName);

    //Shared check
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, K2KeyHelper::SharedName);
}

UEdGraphPin* UEasyAISetKeyK2::GetValuePin() const
{
    UEdGraphPin* Pin = FindPinChecked(K2KeyHelper::ValueName);
    check(Pin->Direction == EGPD_Input);
    return Pin;
}

void UEasyAISetKeyK2::ChangeValuePin()
{
    UEdGraphPin* Pin = GetValuePin();
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

UFunction* UEasyAISetKeyK2::FindSetKeyFunction()
{
    UEdGraphPin* InPin = GetValuePin();
    FName PinCat = InPin->PinType.PinCategory;
    if (InPin)
    {
        FName FunctionName;

        if (PinCat == UEdGraphSchema_K2::PC_Boolean)
        {
            FunctionName = "SetBoolKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Int)
        {
            FunctionName = "SetIntKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Float)
        {
            FunctionName = "SetFloatKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Byte)
        {
            FunctionName = "SetByteKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Struct)
        {
            const TWeakObjectPtr<UObject> PinSub = InPin->PinType.PinSubCategoryObject;

            if (PinSub == TBaseStructure<FVector>::Get())
            {
                FunctionName = "SetVectorKey";
            }
            else  if (PinSub == TBaseStructure<FRotator>::Get())
            {
                FunctionName = "SetRotatorKey";
            }
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Name)
        {
            FunctionName = "SetNameKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_String)
        {
            FunctionName = "SetStringKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Class)
        {
            FunctionName = "SetClassKey";
        }
        else if (PinCat == UEdGraphSchema_K2::PC_Object)
        {
            //AActor handled in SetObjectKey Function.
            FunctionName = "SetObjectKey";
        }

        UFunction* ReturnFunc = UEasyAIKeyFunctions::StaticClass()->FindFunctionByName(FunctionName);
        return ReturnFunc;
    }

    return nullptr;
}


void UEasyAISetKeyK2::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
    Super::PinDefaultValueChanged(ChangedPin);
}

void UEasyAISetKeyK2::PinConnectionListChanged(UEdGraphPin* Pin)
{
    Super::PinConnectionListChanged(Pin);

    if (Pin && Pin->PinName == K2KeyHelper::ValueName)
    {
        ChangeValuePin();

        bool bAllowed = AllowWildcard(Pin);
        if(!bAllowed)
        { 
            ResetPinToWildcard(Pin);
        } 
    }
}

void UEasyAISetKeyK2::PostReconstructNode()
{
    Super::PostReconstructNode();

    ChangeValuePin();
}

void UEasyAISetKeyK2::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    UEdGraphPin* KeyPin = FindPinChecked(K2KeyHelper::KeyName);
    if (KeyPin->GetDefaultAsString().IsEmpty())
    {
        return;
    }

    const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

    //Get function
    UFunction* BlueprintFunction = FindSetKeyFunction();
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

    //Value input
    UEdGraphPin* ValuePin = GetValuePin();
    UEdGraphPin* FunctionValuePin = CallFunction->FindPinChecked(K2KeyHelper::ValueName);
    CompilerContext.MovePinLinksToIntermediate(*ValuePin, *FunctionValuePin);

    //Shared bool
    UEdGraphPin* SharedPin = FindPinChecked(K2KeyHelper::SharedName);
    UEdGraphPin* FunctionSharedPin = CallFunction->FindPinChecked(K2KeyHelper::FunctionSharedName);
    CompilerContext.MovePinLinksToIntermediate(*SharedPin, *FunctionSharedPin);

    //Exec
    CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(CallFunction->GetExecPin()));
    CompilerContext.MovePinLinksToIntermediate(*FindPin(UEdGraphSchema_K2::PN_Then), *(CallFunction->GetThenPin()));
}

FText UEasyAISetKeyK2::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("K2SetKey_Title", "Set Runtime Key Value");
}

FText UEasyAISetKeyK2::GetTooltipText() const
{
    return LOCTEXT("K2SetKey_Tooltip", "Sets a Runtime Key Value from Gameplay Tag.");
}

FText UEasyAISetKeyK2::GetMenuCategory() const
{
    return LOCTEXT("K2SetKey_MenuCategory", "Easy Utility AI | Keys");
}

#undef LOCTEXT_NAMESPACE