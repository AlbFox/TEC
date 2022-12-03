// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIController.h"
#include "AIUtilityData.h"
#include "GameFramework/Character.h"
#include "EasyAITaskManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEasyAI, Log, All);

class UEasyAITaskGroup;
class UEasyAIKeyStorage;

UCLASS(BlueprintType, Blueprintable, abstract, hidecategories = (Collision, Tags, Events, Sockets, Activation, Cooking, AssetUserData, Variable, ComponentTick), ClassGroup=("Easy Utility AI"), meta=(DisplayName = "Easy AI Task Manager", BlueprintSpawnableComponent))
class EASYUTILITYAI_API UEasyAITaskManager : public UActorComponent
{
	GENERATED_BODY()

	UEasyAITaskManager();

protected:
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	/*All tasks that have been spawned by the manager*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager")
	TArray<UEasyAITask*> AllTasks;

	/*If owned by an AI Controller*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager")
	AAIController* AIOwner;

	/*The Pawn owner or the Pawn of the AI Controller that owns this manager*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager")
	APawn* AIOwnerPawn;

	/*The storage object for local runtime keys*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager")
	UEasyAIKeyStorage* LocalKeys;

	/*The storage object of shared runtime keys*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager")
	UEasyAIKeyStorage* SharedKeys;

	/*All Task channels that are part of this Task Manager*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Task Manager", meta = (DisplayName = "Task Channels"))
	TArray<FGameplayTag> AllChannels;

public:

	/*All Easy AI Task classes in this folder are added to the Task Manager on Begin Play*/
	UPROPERTY(EditDefaultsOnly, Category = "Task Manager", meta = (RelativeToGameContentDir, LongPackageName))
	TArray<FDirectoryPath> PrecacheTaskFolders;

	/*A Tasks needs a score with a min. higher than this value to override another*/
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Task Manager")
	float MinOverrideScore = 0.005;

	/*The rate at which UpdateTasks is called*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, AdvancedDisplay, Category = "Task Manager", meta = (ClampMin = "0.01", ClampMax = "1.0", UIMin = "0.01", UIMax = "1.0"))
	float TaskUpdateRate = 0.1f;

public:	
	
	/*Get a Task based on a specific class*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task Manager", meta = (DeterminesOutputType = "TaskClass"))
	UEasyAITask* GetTask(TSubclassOf<UEasyAITask> TaskClass);

	/*Get the currently active Task of a channel*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task Manager")
	UEasyAITask* GetActiveTask(FGameplayTag Channel);

	/*Activates a Forced Task without Scoring*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Task Manager", meta = (DisplayName = "Run Forced Task"))
	bool ForceTask(TSubclassOf<UEasyAIForcedTask> TaskClass);

	/*Calls the Score function on the Task*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Task Manager", meta = (DisplayName = "Run Utility Task"))
	void RunUtilityTask(TSubclassOf<UEasyAIUtilityTask> TaskClass);

	/*Finishes the currently active Task*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Task Manager", meta = (DisplayName = "Finish Active Task"))
	void FinishActiveTask(FGameplayTag Channel);

	/*Main function to run Tasks from*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy AI Task Manager", meta = (DisplayName = "Update Tasks"))
	void OnUpdateTasks();

	/*Check to see if a Task-Channel currently has an active Task.*/
	UFUNCTION(BlueprintPure, Category = "Easy AI Task Manager")
	bool IsChannelActive(FGameplayTag Channel);

	/*Re-Initializes the Pawn when the Task Manager is attached to an AI Controller. Useful when possessing a new Pawn.*/
	UFUNCTION(BlueprintCallable, Category = "Easy AI Task Manager", meta = (DisplayName = "Reset AI Owner Pawn"))
	void ResetPawnForAiOwner();

public:

	void AddActiveTask(UEasyAITask* TheTask);
	void RemoveActiveTask(UEasyAITask* TheTask);
	
	TArray<TSubclassOf<UEasyAITask>> AllTaskClasses;

	TArray<UEasyAITask*> ActiveTasks;
	TArray<UEasyAITask*> LastFinishedTasks;

private:

	float TaskManagerUpdateTime;

	bool bTaskPrecacheCompleted;
	
	void InitKeyManagers();
	void PrecacheTaskClasses(const FString& SearchPath);
	bool AllocateTasks();
	void SelectTask(const FGameplayTag& Channel = FGameplayTag::RequestGameplayTag(DefaultChannelTag));
	UEasyAITask* SpawnNewTask(const TSubclassOf<UEasyAITask>& TaskClass);
	UEasyAITask* GetLastFinishedTask(const FGameplayTag& Channel) const;

	bool CanValidatePawnForAiOwner();

	bool ShouldUpdateTask() const;
	void UpdateTasks();

	FORCEINLINE bool HasAnyTaskClasses()
	{
		return AllTaskClasses.Num() > 0;
	}

	FORCEINLINE bool HasAnyTasks()
	{
		return AllTasks.Num() > 0;
	}

	UEasyAITask* CompareScores(const TArray<UEasyAITask*>& Scorers, bool& bHasEquals) const;

};
