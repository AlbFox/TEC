// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FEasyUtilityAIModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
