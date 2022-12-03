// Easy Utility AI - (C) 2022 by Michael Hegemann

#include "EasyUtilityAI.h"
#include "GameplayTagsManager.h"
#include "AIUtilityData.h"

#define LOCTEXT_NAMESPACE "FEasyUtilityAIModule"

void FEasyUtilityAIModule::StartupModule()
{
	//Add a default gameplay tag for task channels
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	TagsManager.AddNativeGameplayTag(DefaultChannelTag, "");
}

void FEasyUtilityAIModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEasyUtilityAIModule, EasyUtilityAI)