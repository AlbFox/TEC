// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "EasyAITaskManager.h"
#include "Engine/World.h"
#include "EasyAITask.h"
#include "AIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AssetData.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "AssetRegistryModule.h"
#include "EasyAIQueryObject.h"
#include "AIUtilityFunctions.h"
#include "EasyAIKeyStorage.h"

DEFINE_LOG_CATEGORY(LogEasyAI);

UEasyAITaskManager::UEasyAITaskManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}

void UEasyAITaskManager::BeginPlay()
{
	const UEasyAITaskManager* TaskManComp = GetOwner()->FindComponentByClass<UEasyAITaskManager>();
	if (TaskManComp)
	{
		if (TaskManComp != this)
		{
			UE_LOG(LogEasyAI, Error, TEXT("%s has multiple Task Managers!"), *GetOwner()->GetName());
		}
	}

	AIOwner = Cast<AAIController>(GetOwner());
	if (!AIOwner)
	{
		AIOwnerPawn = Cast<APawn>(GetOwner());
		if (!AIOwnerPawn)
		{
			UE_LOG(LogEasyAI, Warning, TEXT("Task Manager must be attached to AIController or Pawn!"));
			return;
		}
	}

	//Allow Prechaching of Tasks
	TArray<FString> TaskPaths;
	{
		for (const FDirectoryPath& TaskFolder : PrecacheTaskFolders)
		{
			if (!TaskFolder.Path.IsEmpty())
			{
				//Only add path once
				TaskPaths.AddUnique(TaskFolder.Path);
			}
		}

		if (TaskPaths.Num() > 0)
		{
			for (const FString& PathString : TaskPaths)
			{
				PrecacheTaskClasses(PathString);
			}

			if (HasAnyTaskClasses())
			{
				AllocateTasks();
				bTaskPrecacheCompleted = true;
			}
		}
		else
		{
			//Not doing any precaching
			bTaskPrecacheCompleted = true;
		}
	}

	InitKeyManagers();

	Super::BeginPlay();
}

bool UEasyAITaskManager::CanValidatePawnForAiOwner()
{
	//Get the Pawn from the owning AI Controller.
	if (AIOwner)
	{
		if (!AIOwner->GetPawn())
		{
			return false;
		}
		else
		{
			if (AIOwnerPawn)
			{
				return true;
			}
			else
			{
				AIOwnerPawn = AIOwner->GetPawn();
			}
		}
	}
	//Return true if attached to pawn
	else
	{
		if (AIOwnerPawn)
		{
			return true;
		}
		else
		{
			AIOwnerPawn = Cast<APawn>(GetOwner());
			return true;
		}
	}

	return false;
}

void UEasyAITaskManager::ResetPawnForAiOwner()
{
	//Reset the Pawn

	AIOwnerPawn = nullptr;

	if (CanValidatePawnForAiOwner())
	{
		//Also for Tasks
		for (UEasyAITask* TaskObject : AllTasks)
		{
			if (TaskObject)
			{
				TaskObject->AIOwnerPawn = AIOwnerPawn;
			}
		}
	}
	else
	{
		UE_LOG(LogEasyAI, Warning, TEXT("Task Manager : Reset AI Owner Pawn failed. Call this after possess!"));
	}
}

void UEasyAITaskManager::PrecacheTaskClasses(const FString& SearchPath)
{
	if (SearchPath.IsEmpty())
	{
		return;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >(FName("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	TArray<FAssetData> AssetData;

	FARFilter Filter;
	{
		Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
		Filter.PackagePaths.Add(*SearchPath);
		Filter.bRecursiveClasses = false;
		Filter.bRecursivePaths = false;
		AssetRegistry.GetAssets(Filter, AssetData);
	}

	TSet< FName > DerivedNames;
	{
		FName BaseClassName = UEasyAITask::StaticClass()->GetFName();

		TArray< FName > BaseNames;
		BaseNames.Add(BaseClassName);

		TSet< FName > Excluded;
		AssetRegistry.GetDerivedClassNames(BaseNames, Excluded, DerivedNames);
	}

	for (const FAssetData& Asset : AssetData)
	{
		// Get the the class this blueprint generates (this is stored as a full path)
		FAssetDataTagMapSharedView::FFindTagResult GeneratedClassPathPtr = Asset.TagsAndValues.FindTag(TEXT("GeneratedClass"));
		if(GeneratedClassPathPtr.IsSet())
		{
			//Thanks Cameron Angus
			const FString ClassObjectPath = FPackageName::ExportTextPathToObjectPath(GeneratedClassPathPtr.GetValue());
			const FString ClassName = FPackageName::ObjectPathToObjectName(ClassObjectPath);

			if (!DerivedNames.Contains(*ClassName))
			{
				continue;
			}

			//Load the actual asset.
			TSoftClassPtr<UEasyAITask> TaskClassSoftObj = TSoftClassPtr<UEasyAITask>(FStringAssetReference(ClassObjectPath));

			if (!TaskClassSoftObj.IsValid())
			{
				TaskClassSoftObj.LoadSynchronous();
			}

			if (TaskClassSoftObj.IsValid())
			{
				UClass* NewClass = TaskClassSoftObj.Get();
				if (NewClass)
				{
					//Make sure its the right class
					if (NewClass->IsChildOf(UEasyAITask::StaticClass()))
					{
						//Skip Abstract
						if (NewClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated))
						{
							continue;
						}

						AllTaskClasses.Add(NewClass);
					}
				}
			}
			else
			{
				UE_LOG(LogEasyAI, Error, TEXT("Task Manager %s has invalid Task %s"), *GetName(), *Asset.AssetName.ToString());
			}

		}
	}
}

void UEasyAITaskManager::InitKeyManagers()
{
	//Local
	if (!LocalKeys)
	{
		UEasyAIKeyStorage* NewKeys = NewObject<UEasyAIKeyStorage>(GetTransientPackage(), UEasyAIKeyStorage::StaticClass());
		LocalKeys = NewKeys;
	}

	//Shared
	TArray<AActor*> Actors;
	const EActorIteratorFlags Flags = EActorIteratorFlags::SkipPendingKill;
	for (FActorIterator It(GetWorld(), Flags); It; ++It)
	{
		const AActor* Actor = *It;
		if (Actor && IsValid(Actor) && Actor != GetOwner())
		{
			const UEasyAITaskManager* TaskManComp = Actor->FindComponentByClass<UEasyAITaskManager>();
			if (TaskManComp)
			{
				if (TaskManComp->GetClass() == GetClass())
				{
					if (TaskManComp->SharedKeys)
					{
						SharedKeys = TaskManComp->SharedKeys;
						return;
					}
				}
			}
		}
	}

	if (!SharedKeys)
	{
		UEasyAIKeyStorage* NewSharedKeys = NewObject<UEasyAIKeyStorage>(GetTransientPackage(), UEasyAIKeyStorage::StaticClass());
		SharedKeys = NewSharedKeys;
	}
}

bool UEasyAITaskManager::ShouldUpdateTask() const
{
	return GetWorld()->TimeSeconds - TaskManagerUpdateTime >= TaskUpdateRate;
}

void UEasyAITaskManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner())
	{
		return;
	}

	//The pawn is assigned when possessed, so we cannot just set it in BeginPlay and need to set and validate it here.
	if (!CanValidatePawnForAiOwner())
	{
		return;
	}

	if (IsActive() && bTaskPrecacheCompleted)
	{
		for (UEasyAITask* TaskObject : AllTasks)
		{
			if (TaskObject)
			{
				//Get real score from previous frame, so it can be read across all tasks alike no matter in which order they are updated.
				TaskObject->ActualScore = TaskObject->Score;

				//Tick when active
				if (TaskObject->IsActive())
				{
					TaskObject->ReceiveTick(DeltaTime);
				}

				//Set default score
				if (ShouldUpdateTask())
				{
					TaskObject->TaskDefaultUpdate();
				}
			}
		}

		UpdateTasks();
	}
	
}

void UEasyAITaskManager::UpdateTasks()
{
	//Not updating every frame
	if (ShouldUpdateTask())
	{
		//This is where run utility task has to be called
		OnUpdateTasks();

		//Select task for every channel
		for (const FGameplayTag& TaskChannel : AllChannels)
		{
			SelectTask(TaskChannel);
		}

		TaskManagerUpdateTime = GetWorld()->TimeSeconds;
	}
}

void UEasyAITaskManager::SelectTask(const FGameplayTag& Channel)
{
	if (!HasAnyTasks() || !Channel.IsValid())
	{
		return;
	}

	//Check if we have an active task that has to be finished first
	const UEasyAITask* ActiveTask = GetActiveTask(Channel);
	if (ActiveTask)
	{
		//Check if we can override with the higher scoring one, or if we need to wait until the current task is finished
		if (ActiveTask->IsExplicitFinish() || ActiveTask->IsForced() || ActiveTask->IsBelowMinActiveTime())
		{
			return;
		}
	}

	//Group Scoring Tasks by Channel
	TArray<UEasyAITask*> ScoringTasks;
	for (UEasyAITask* TaskObject : AllTasks)
	{
		if (TaskObject->IsValidForScoring(Channel))
		{
			ScoringTasks.Add(TaskObject);
		}
	}

	if (ScoringTasks.Num() <= 0)
	{
		return;
	}

	//Here we can begin the scoring process
	bool bHasEquals = false;
	UEasyAITask* HighestScoring = CompareScores(ScoringTasks, bHasEquals);
	if (!HighestScoring)
	{
		return;
	}

	if (HighestScoring->IsActive())
	{
		return;
	}

	//Handle Finish
	if (ActiveTask)
	{
		//Favor active until one scores higher or it finishes, to prevent flipping around between equal tasks.
		if (bHasEquals)
		{
			return;
		}

		//Finish Task with failsave, so we never have a case where multiple tasks per channel are active
		uint16 ActiveCount = 0;
		const TArray<UEasyAITask*> FinTasks = ActiveTasks;
		for (UEasyAITask* FinTask : FinTasks)
		{
			if (FinTask)
			{
				if (FinTask->Channel == Channel)
				{
					ActiveCount++;
					FinTask->FinishTask();
				}
			}
		}

		//Should never happen
		if (ActiveCount > 1)
		{
			UE_LOG(LogEasyAI, Warning, TEXT("Multiple Tasks were finished for Channel: %s !  Last Finished Task may be inaccurate!"), *Channel.ToString());
		}
	}

	//Get the last finished task and proceed.
	UEasyAITask* LastFinished = GetLastFinishedTask(Channel);
	if (LastFinished)
	{
		HighestScoring->SetTaskActive(LastFinished);
		LastFinishedTasks.Remove(LastFinished);
	}
	else
	{
		HighestScoring->SetTaskActive(nullptr);
	}
}

UEasyAITask* UEasyAITaskManager::CompareScores(const TArray<UEasyAITask*>& Scorers, bool& bHasEquals) const
{
	if (Scorers.Num() <= 0)
	{
		return nullptr;
	}

	if (Scorers.Num() <= 1)
	{
		return Scorers[0];
	}

	TArray<UEasyAITask*> SortedTasks = Scorers;

	SortedTasks.Sort([&bHasEquals](const UEasyAITask& A, const UEasyAITask& B)
	{
		bHasEquals = FMath::IsNearlyEqual(A.Score, B.Score);
		return A.Score > B.GetOverrideScore();
	});

	if (bHasEquals)
	{
		const uint8 EqualsRand = FMath::RandRange(0, FMath::Min(1, SortedTasks.Num() - 1));
		return SortedTasks[EqualsRand];
	}

	return SortedTasks[0];
}

void UEasyAITaskManager::RunUtilityTask(TSubclassOf<UEasyAIUtilityTask> TaskClass)
{
	UEasyAITask* NewTask = GetTask(TaskClass);

	if (!NewTask)
	{
		NewTask = SpawnNewTask(TaskClass);
		return;
	}

	if (NewTask)
	{
		UEasyAIUtilityTask* UtilityTask = Cast<UEasyAIUtilityTask>(NewTask);
		if (UtilityTask)
		{
			UtilityTask->UpdateTask();
		}
	}
}

void UEasyAITaskManager::FinishActiveTask(FGameplayTag Channel)
{
	if (!Channel.IsValid())
	{
		Channel = FGameplayTag::RequestGameplayTag(DefaultChannelTag);
	}

	UEasyAITask* CurrentTask = GetActiveTask(Channel);

	if (CurrentTask)
	{
		CurrentTask->FinishTask();
	}
}

bool UEasyAITaskManager::ForceTask(TSubclassOf<UEasyAIForcedTask> TaskClass)
{
	UEasyAITask* NewTask = GetTask(TaskClass);
	if (!NewTask)
	{
		NewTask = SpawnNewTask(TaskClass);
		if (!NewTask)
		{
			return false;
		}
	}

	UEasyAITask* PreviousTask = GetActiveTask(TaskClass.GetDefaultObject()->Channel);
	if (PreviousTask)
	{
		NewTask->SetTaskActive(PreviousTask);
		PreviousTask->FinishTask();
	}
	else
	{
		NewTask->SetTaskActive(nullptr);
	}

	return true;
}

bool UEasyAITaskManager::IsChannelActive(FGameplayTag Channel)
{
	const UEasyAITask* CurrentTask = GetActiveTask(Channel);

	if (CurrentTask && CurrentTask->bIgnoreChannelActive)
	{
		return false;
	}

	return CurrentTask != nullptr;
}

UEasyAITask* UEasyAITaskManager::GetTask(TSubclassOf<UEasyAITask> TaskClass)
{
	if (!HasAnyTasks())
	{
		return nullptr;
	}

	for (UEasyAITask* TaskObject : AllTasks)
	{
		if (TaskObject)
		{
			if (TaskObject->GetClass() == TaskClass)
			{
				return TaskObject;
			}
		}
	}

	return nullptr;
}

UEasyAITask* UEasyAITaskManager::GetActiveTask(FGameplayTag Channel)
{
	if (!Channel.IsValid())
	{
		Channel = FGameplayTag::RequestGameplayTag(DefaultChannelTag);
	}

	for (UEasyAITask* ActiveTask : ActiveTasks)
	{
		if (ActiveTask->Channel == Channel)
		{
			return ActiveTask;
		}
	}

	return nullptr;
}

void UEasyAITaskManager::AddActiveTask(UEasyAITask* TheTask)
{
	ActiveTasks.Add(TheTask);
}

void UEasyAITaskManager::RemoveActiveTask(UEasyAITask* TheTask)
{
	LastFinishedTasks.Add(TheTask);
	ActiveTasks.Remove(TheTask);
}

UEasyAITask* UEasyAITaskManager::GetLastFinishedTask(const FGameplayTag& Channel) const
{
	if (!Channel.IsValid())
	{
		return nullptr;
	}

	for (UEasyAITask* LastTask : LastFinishedTasks)
	{
		if (LastTask->Channel == Channel)
		{
			return LastTask;
		}
	}

	return nullptr;
}

UEasyAITask* UEasyAITaskManager::SpawnNewTask(const TSubclassOf<UEasyAITask>& TaskClass)
{
	if (TaskClass.GetDefaultObject())
	{
		UEasyAITask* NewTask = NewObject<UEasyAITask>(GetOwner(), TaskClass);
		if (NewTask)
		{
			NewTask->InitTask(this);

			AllTasks.Add(NewTask);

			//Add the new tag. If invalid the task will always use the default channel.
			AllChannels.AddUnique(NewTask->Channel);

			return NewTask;
		}
	}

	return nullptr;
}

bool UEasyAITaskManager::AllocateTasks()
{
	bool bAllocated = false;

	for (const TSubclassOf<UEasyAITask>& TaskClass : AllTaskClasses)
	{
		SpawnNewTask(TaskClass);
	}

	bAllocated = AllTasks.Num() == AllTaskClasses.Num();

	if (!bAllocated)
	{
		UE_LOG(LogEasyAI, Error, TEXT("Precache Task Allocation failed"));
		return false;
	}

	return bAllocated;
}


