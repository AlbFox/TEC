// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "EasyAITask.h"
#include "EasyAIQueryObject.h"


class FAssetTypeActions_EasyAITask : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_EasyAITask(EAssetTypeCategories::Type InAssetCategoryBit)
		: AssetCategoryBit(InAssetCategoryBit)
	{
	}

	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_EasyAITask", "Easy AI Task"); }
	virtual FColor GetTypeColor() const override { return FColor::White; }
	virtual bool HasActions ( const TArray<UObject*>& InObjects ) const override { return false; }
	virtual UClass* GetSupportedClass() const override { return UEasyAITask::StaticClass(); }
	virtual bool CanFilter() override { return true; }
	virtual uint32 GetCategories() override { return AssetCategoryBit; }

private:

	EAssetTypeCategories::Type AssetCategoryBit;
};

class FAssetTypeActions_EasyAIManager: public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_EasyAIManager(EAssetTypeCategories::Type InAssetCategoryBit)
		: AssetCategoryBit(InAssetCategoryBit)
	{
	}

	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_EasyAITaskManager", "Easy AI Task Manager"); }
	virtual FColor GetTypeColor() const override { return FColor::White; }
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return false; }
	virtual UClass* GetSupportedClass() const override { return UEasyAITaskManager::StaticClass(); }
	virtual bool CanFilter() override { return true; }
	virtual uint32 GetCategories() override { return AssetCategoryBit; }

private:

	EAssetTypeCategories::Type AssetCategoryBit;
};

class FAssetTypeActions_EasyAIQueryObject : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_EasyAIQueryObject(EAssetTypeCategories::Type InAssetCategoryBit)
		: AssetCategoryBit(InAssetCategoryBit)
	{
	}

	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_EasyAIQueryObject", "Easy AI Query"); }
	virtual FColor GetTypeColor() const override { return FColor::White; }
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return false; }
	virtual UClass* GetSupportedClass() const override { return UEasyAIQueryObject::StaticClass(); }
	virtual bool CanFilter() override { return true; }
	virtual uint32 GetCategories() override { return AssetCategoryBit; }

private:

	EAssetTypeCategories::Type AssetCategoryBit;
};