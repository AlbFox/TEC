// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "EasyAIKeyFunctionsK2.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "AIUtilityData.h"
#include "AIUtilityFunctions.h"
#include "EasyAITask.h"
#include "EasyAIQueryObject.h"
#include "EasyAIKeyFunctions.h"
#include "Animation/AnimInstance.h"

#define LOCTEXT_NAMESPACE "K2KeyFunctions"

void UEasyAIKeyFunctionsK2::ResetPinToWildcard(UEdGraphPin* Pin)
{
    Pin->BreakAllPinLinks(true);
    Pin->PinType.ResetToDefaults();
    Pin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
}

bool UEasyAIKeyFunctionsK2::AllowWildcard(UEdGraphPin* Pin)
{
    FName PinCat = Pin->PinType.PinCategory;
    bool bAllowed = false;

    if (PinCat == UEdGraphSchema_K2::PC_Boolean
        || PinCat == UEdGraphSchema_K2::PC_Int
        || PinCat == UEdGraphSchema_K2::PC_Float
        || PinCat == UEdGraphSchema_K2::PC_Byte
        || PinCat == UEdGraphSchema_K2::PC_Name
        || PinCat == UEdGraphSchema_K2::PC_String
        || PinCat == UEdGraphSchema_K2::PC_Class
        || PinCat == UEdGraphSchema_K2::PC_Object)
    {
        bAllowed = true;
    }
    else if (PinCat == UEdGraphSchema_K2::PC_Struct)
    {
        const TWeakObjectPtr<UObject> PinSub = Pin->PinType.PinSubCategoryObject;
        if (PinSub == TBaseStructure<FVector>::Get() || PinSub == TBaseStructure<FRotator>::Get())
        {
            bAllowed = true;
        }
    }

    return bAllowed;
}

bool UEasyAIKeyFunctionsK2::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
    bool bIsCompatible = false;
    UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
    if (Blueprint && Blueprint->SkeletonGeneratedClass)
    {
        bIsCompatible =
            Blueprint->ParentClass->IsChildOf(UEasyAITask::StaticClass())
            || Blueprint->ParentClass->IsChildOf(UEasyAITaskManager::StaticClass())
            || Blueprint->ParentClass->IsChildOf(UAnimInstance::StaticClass())
            || Blueprint->ParentClass->IsChildOf(APawn::StaticClass())
            || Blueprint->ParentClass->IsChildOf(AAIController::StaticClass())
            || Blueprint->ParentClass->IsChildOf(UEasyAIQueryObject::StaticClass());
    }

    return bIsCompatible && Super::IsCompatibleWithGraph(TargetGraph);
}

void UEasyAIKeyFunctionsK2::RestorePinConnections(UEdGraphPin* RestorePin)
{
    // Cache all the pin connections to the RestorePin, we will attempt to recreate them
    TArray<UEdGraphPin*> RestorePinConnectionList = RestorePin->LinkedTo;

    // Because the archetype has changed, we break the output link as the output pin type will change
    RestorePin->BreakAllPinLinks(true);

    const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

    // Recreate any pin links to the Result pin that are still valid
    for (UEdGraphPin* Connections : RestorePinConnectionList)
    {
        K2Schema->TryCreateConnection(RestorePin, Connections);
    }

    // Refresh the UI for the graph so the pin changes show up
    GetGraph()->NotifyGraphChanged();

    // Mark dirty
    FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

void UEasyAIKeyFunctionsK2::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
    Super::ValidateNodeDuringCompilation(MessageLog);

    //Make sure tag is not null
    const UEdGraphPin* KeyPin = FindPinChecked(K2KeyHelper::KeyName);
    const FString KeyTagValue = KeyPin->GetDefaultAsString();
    if (KeyTagValue.IsEmpty())
    {
        const FText MessageText = LOCTEXT("K2KeyFunctions_Warning", "Missing Gameplay Tag Key for @@");
        MessageLog.Warning(*MessageText.ToString(), this);
    }
}

FSlateIcon UEasyAIKeyFunctionsK2::GetIconAndTint(FLinearColor& OutColor) const
{
    OutColor = GetNodeTitleColor();
    static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
    return Icon;
}

void UEasyAIKeyFunctionsK2::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();

    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(NodeSpawner != nullptr);

        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

#undef LOCTEXT_NAMESPACE