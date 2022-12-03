// Easy Utility AI - (C) 2022 by Michael Hegemann

#include "EasyUtilityAIEditor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EasyAITaskFactory.h"
#include "EasyAITask.h"
#include "EasyAIQueryFactory.h"
#include "EasyAIManagerFactory.h"
#include "EasyAITaskManager.h"
#include "EasyAIQueryObject.h"
#include "AssetTypeActions_EasyAI.h"
#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"
#include "Kismet2/SClassPickerDialog.h"

#define LOCTEXT_NAMESPACE "FEasyUtilityAIEditorModule"

class FAssetClassParentFilter : public IClassViewerFilter
{
public:
	FAssetClassParentFilter()
		: DisallowedClassFlags(CLASS_None)
	{}

	TSet< const UClass* > AllowedChildrenOfClasses;
	EClassFlags DisallowedClassFlags;
	EClassFlags AllowedClassFlags;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InClass->HasAnyClassFlags(DisallowedClassFlags) && InClass->HasAnyClassFlags(AllowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags) && InUnloadedClassData->HasAnyClassFlags(AllowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}
};

//
// UEasyAITaskFactory
//

UEasyAITaskFactory::UEasyAITaskFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UEasyAITask::StaticClass();
}

bool UEasyAITaskFactory::ConfigureProperties()
{
	// nullptr the Class so we can check for selection
	TaskClass = nullptr;

	// Load the classviewer module to display a class picker
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;

	TSharedPtr<FAssetClassParentFilter> Filter = MakeShareable(new FAssetClassParentFilter);
	Options.ClassFilter = Filter;
	//Options.ClassFilters.Add(Filter.ToSharedRef());  

	Filter->AllowedClassFlags = CLASS_Native;
	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists;
	Filter->AllowedChildrenOfClasses.Add(UEasyAITask::StaticClass());

	const FText TitleText = LOCTEXT("CreateTaskObject", "Pick Task Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UEasyAITask::StaticClass());

	if (bPressedOk)
	{
		TaskClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* UEasyAITaskFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UClass* ClassToUse = TaskClass.Get() ? TaskClass.Get() : Class;
	check(ClassToUse->IsChildOf(UEasyAITask::StaticClass()));

	return FKismetEditorUtilities::CreateBlueprint(ClassToUse, InParent, Name, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
}

//
// UEasyAIManagerFactory
//

UEasyAIManagerFactory::UEasyAIManagerFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UEasyAITaskManager::StaticClass();
}

UObject* UEasyAIManagerFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FKismetEditorUtilities::CreateBlueprint(Class, InParent, Name, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
}


//
// UEasyAIQueryFactory
//

UEasyAIQueryFactory::UEasyAIQueryFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UEasyAIQueryObject::StaticClass();
}

bool UEasyAIQueryFactory::ConfigureProperties()
{
	// nullptr the Class so we can check for selection
	QueryClass = nullptr;

	// Load the classviewer module to display a class picker
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;

	TSharedPtr<FAssetClassParentFilter> Filter = MakeShareable(new FAssetClassParentFilter);
	Options.ClassFilter = Filter;
	//Options.ClassFilters.Add(Filter.ToSharedRef()); 

	Filter->AllowedClassFlags = CLASS_Native;
	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists;
	Filter->AllowedChildrenOfClasses.Add(UEasyAIQueryObject::StaticClass());

	const FText TitleText = LOCTEXT("CreateQueryObject", "Pick Query Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, UEasyAIQueryObject::StaticClass());

	if (bPressedOk)
	{
		QueryClass = ChosenClass;
	}

	return bPressedOk;
}

UObject* UEasyAIQueryFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UClass* ClassToUse = QueryClass.Get() ? QueryClass.Get() : Class;
	check(ClassToUse->IsChildOf(UEasyAIQueryObject::StaticClass()));

	return FKismetEditorUtilities::CreateBlueprint(ClassToUse, InParent, Name, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
}

//
// Module
//

void FEasyUtilityAIEditorModule::StartupModule()
{
	// Register asset types
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	EAssetTypeCategories::Type AssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("EasyAI")), LOCTEXT("EasyAICategory", "Easy Utility AI"));

	// this category should have been registered already
	check(AssetCategoryBit != EAssetTypeCategories::Misc);

	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_EasyAITask(AssetCategoryBit)));
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_EasyAIQueryObject(AssetCategoryBit)));
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_EasyAIManager(AssetCategoryBit)));

	RegisterAutoGeneratedDefaultEvents();
}


void FEasyUtilityAIEditorModule::RegisterAutoGeneratedDefaultEvents()
{
	FKismetEditorUtilities::RegisterAutoGeneratedDefaultEvent(this, UEasyAIForcedTask::StaticClass(), "OnStart");

	FKismetEditorUtilities::RegisterAutoGeneratedDefaultEvent(this, UEasyAIUtilityTask::StaticClass(), "OnStart");
	FKismetEditorUtilities::RegisterAutoGeneratedDefaultEvent(this, UEasyAIUtilityTask::StaticClass(), "OnScore");

	FKismetEditorUtilities::RegisterAutoGeneratedDefaultEvent(this, UEasyAIQueryObject::StaticClass(), "OnBeginPlay");
	FKismetEditorUtilities::RegisterAutoGeneratedDefaultEvent(this, UEasyAIQueryObject::StaticClass(), "PreQuery");

	FKismetEditorUtilities::RegisterAutoGeneratedDefaultEvent(this, UEasyAITaskManager::StaticClass(), "OnUpdateTasks");
}

void FEasyUtilityAIEditorModule::ShutdownModule()
{
	// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

		for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
		}
	}

	CreatedAssetTypeActions.Empty();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEasyUtilityAIEditorModule, EasyUtilityAIEditor)