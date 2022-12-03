// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "EasyAITask.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIUtilityFunctions.h"

//
//Easy AI Task 
//

UWorld* UEasyAITask::GetWorld() const
{
	return World;
}

void UEasyAITask::InitTask(UEasyAITaskManager* TaskManager)
{
	//Always use default channel if invalid
	if (!Channel.IsValid())
	{
		Channel = FGameplayTag::RequestGameplayTag(DefaultChannelTag);
	}

	Manager = TaskManager;

	if (Manager)
	{
		World = Manager->GetWorld();

		//Try find the owner
		if (Manager->AIOwner)
		{
			AIOwner = Manager->AIOwner;
		}

		if (Manager->AIOwnerPawn)
		{
			AIOwnerPawn = Manager->AIOwnerPawn;
		}

		OnBeginPlay();
	}
}

void UEasyAITask::SetTaskActive(UEasyAITask* Previous)
{
	if (!Manager)
	{
		return;
	}

	if (Previous && Previous != this)
	{
		Previous->ConsecutiveCount = 0;
	}

	Manager->AddActiveTask(this);

	bWasActiveOnce = true;
	bIsActive = true;
	ElapsedTime = 0.0f;
	OverallCount++;
	ConsecutiveCount++;
	CooldownTime = 0.0f;

	OnStart(Previous);
}

void UEasyAITask::FinishTask()
{
	ElapsedTime = 0.0f;
	FinishTimeStamp = GetWorld()->GetTimeSeconds();
	bIsActive = false;

	if (Manager)
	{
		Manager->RemoveActiveTask(this);
	}

	OnFinish();
}

void UEasyAITask::ReceiveTick(float DeltaTime)
{
	if (!Manager)
	{
		return;
	}

	if (!IsActive())
	{
		return;
	}

	LastFinishTime = 0;
	ElapsedTime += DeltaTime;
	OnTick(DeltaTime);
}

UEasyAITask* UEasyAITask::GetTask(TSubclassOf<UEasyAITask> TaskClass)
{
	if (Manager)
	{
		return Manager->GetTask(TaskClass);
	}

	return nullptr;
}

UEasyAITask* UEasyAITask::GetActiveTask()
{
	if (Manager)
	{
		return Manager->GetActiveTask(Channel);
	}

	return nullptr;
}

bool UEasyAITask::IsChannelActive(FGameplayTag ActiveChannel)
{
	if (Manager)
	{
		return Manager->IsChannelActive(ActiveChannel);
	}

	return false;
}

//
//Easy AI Utility Task 
//

float UEasyAIUtilityTask::GetOverrideScore() const
{
	return Score + Manager->MinOverrideScore;
}

void UEasyAIUtilityTask::NormalizeScore()
{
	Score = FMath::Clamp(Score, 0.0f, 1.0f);
	Score = FMath::RoundToFloat(Score * SCORE_ROUND) / SCORE_ROUND;
}

void UEasyAIUtilityTask::SetCooldown(float Time)
{
	CooldownTime = Time;
}

void UEasyAIUtilityTask::UpdateCooldown()
{
	LastFinishTime = GetWorld()->GetTimeSeconds() - FinishTimeStamp;

	//Auto finish . We call this here since this function is always called regardless of branch.
	if (MaxActiveTime > 0 && ElapsedTime >= MaxActiveTime)
	{
		FinishTask();
	}
}

void UEasyAIUtilityTask::SetScore(float NewScore)
{
	Score = NewScore;
	NormalizeScore();
}

void UEasyAIUtilityTask::SetUtilityScore(TArray<FUtilityScore> Scorers)
{
	const float NewScore = UAIUtilityFunctions::CreateUtilityScore(Scorers, ScoringType);
	SetScore(NewScore);
}

void UEasyAIUtilityTask::TaskDefaultUpdate()
{
	//This can happen with spawned AI using precached Tasks
	if (!AIOwnerPawn)
	{
		AIOwnerPawn = Manager->AIOwnerPawn;
	}

	SetScore(0);
	UpdateCooldown();

	bIsUpdating = false;
}

void UEasyAIUtilityTask::UpdateTask()
{
	if (IsDelayed())
	{
		SetScore(0);
		return;
	}
	else
	{
		SetScore(DefaultScore);
	}

	OnScore();

	if (Score > 0)
	{
		NormalizeScore();
	}
	else
	{
		Score = 0;
	}

	bIsUpdating = true;
}

float UEasyAIUtilityTask::GetTaskScore(TSubclassOf<UEasyAIUtilityTask> TaskClass)
{
	const UEasyAITask* Task = UEasyAITask::GetTask(TaskClass);
	if (Task)
	{
		return Task->ActualScore;
	}

	return 0.0f;
}


