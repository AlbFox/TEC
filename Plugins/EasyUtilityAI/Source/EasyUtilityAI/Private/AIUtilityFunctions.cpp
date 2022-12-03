// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "AIUtilityFunctions.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "AIController.h"
#include "EasyAIQueryObject.h"
#include "EasyAINavigationQuery.h"
#include "EasyAIActorQuery.h"
#include "EasyAITask.h"
#include "EasyAILocationQuery.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

//
//Task Manager
//

static UEasyAITaskManager* GetTaskManagerFromOwner(const AActor*& Owner)
{
	if (Owner)
	{
		UEasyAITaskManager* TaskManagerComp = nullptr;

		TaskManagerComp = Owner->FindComponentByClass<UEasyAITaskManager>();
		if (TaskManagerComp)
		{
			return TaskManagerComp;
		}
		else
		{
			const APawn* AIPawn = Cast<APawn>(Owner);
			if (AIPawn)
			{
				const AAIController* AIOwner = Cast<AAIController>(AIPawn->GetController());
				if (AIOwner)
				{
					TaskManagerComp = AIOwner->FindComponentByClass<UEasyAITaskManager>();
					if (TaskManagerComp)
					{
						return TaskManagerComp;
					}
				}
			}
		}
	}

	return nullptr;
}

UEasyAITaskManager* UAIUtilityFunctions::FindTaskManager(UObject* WorldContextObject)
{
	//For Tasks
	const UEasyAITask* Task = Cast<UEasyAITask>(WorldContextObject);
	if (Task)
	{
		return Task->Manager;
	}

	//For Task Manager
	UEasyAITaskManager* Manager = Cast<UEasyAITaskManager>(WorldContextObject);
	if (Manager)
	{
		return Manager;
	}

	//For Querys
	const UEasyAIQueryObject* Query = Cast<UEasyAIQueryObject>(WorldContextObject);
	if (Query)
	{
		const AActor* Owner = Query->Querier;
		if (Owner)
		{
			return GetTaskManagerFromOwner(Owner);
		}
	}

	//For Actors
	const AActor* Owner = Cast<AActor>(WorldContextObject);
	if (Owner)
	{
		return GetTaskManagerFromOwner(Owner);
	}

	//For Anim 
	const UAnimInstance* AnimBP = Cast<UAnimInstance>(WorldContextObject);
	if (AnimBP)
	{
		const AActor* AnimOwner = Cast<AActor>(AnimBP->GetOwningActor());
		if (AnimOwner)
		{
			return GetTaskManagerFromOwner(AnimOwner);
		}

	}

	return nullptr;
}

UEasyAITaskManager* UAIUtilityFunctions::GetTaskManager(UObject* WorldContextObject)
{
	UEasyAITaskManager* Manager = FindTaskManager(WorldContextObject);
	return Manager;
}


//
//Helpers
//

void UAIUtilityFunctions::DrawDebugLocations(UObject* WorldContextObject, FDrawDebugLocationData DrawData)
{
	if (!WorldContextObject)
	{
		return;
	}

	if (DrawData.Locations.Num() > 0)
	{
		FLinearColor DebugCol = FLinearColor(0, 0, 0, 1);
		int Num = 0;
		const FVector ZOffset = FVector(0, 0, DrawData.ZOffset);
		for (const FVector& Loc : DrawData.Locations)
		{
			const float NumFraction = UKismetMathLibrary::MapRangeClamped(Num, 0, DrawData.Locations.Num() - 1, 0.0, 1.0f);
			DebugCol = DrawData.Locations.Num() > 1 ? FLinearColor::LerpUsingHSV(FLinearColor::Green, FLinearColor::Red, NumFraction) : FLinearColor::Green;
			UKismetSystemLibrary::DrawDebugPoint(WorldContextObject, Loc + ZOffset, DrawData.Size, DebugCol, DrawData.Duration);
			Num++;
		}

		UKismetSystemLibrary::DrawDebugString(WorldContextObject, DrawData.Locations[0] + FVector(0, 0, DrawData.Size) + ZOffset, TEXT("Best"), NULL, FLinearColor::White, DrawData.Duration);
	}
}

//
//Scoring
//

template<class TSortProperty>
static TArray<TSortProperty> SortScoredProperties(const TArray<TSortProperty>& ScoredProperties)
{
	if (ScoredProperties.Num() <= 0)
	{
		return TArray<TSortProperty>();
	}

	TArray<TSortProperty> TempScored;

	float TempScores = 0;
	for (const TSortProperty& InActor : ScoredProperties)
	{
		//Only add the ones that did not score zero
		if (InActor.Score > 0)
		{
			TempScores += InActor.Score;
			TempScored.Add(InActor);
		}
	}

	if (TempScores <= 0)
	{
		return TArray<TSortProperty>();
	}

	//Sort by highest score
	TempScored.Sort([](const TSortProperty& A, const TSortProperty& B)
	{
		return A.Score > B.Score;
	});

	return TempScored;
}

TArray<FUtilityScoreActor> UAIUtilityFunctions::SortScoredActors(const TArray<FUtilityScoreActor>& ScoredActors)
{
	if (ScoredActors.Num() > 0)
	{
		return SortScoredProperties(ScoredActors);
	}

	return TArray<FUtilityScoreActor>();
}

TArray<FUtilityScoreVector> UAIUtilityFunctions::SortScoredVectors(const TArray<FUtilityScoreVector>& ScoredVectors)
{
	if (ScoredVectors.Num() > 0)
	{
		return SortScoredProperties(ScoredVectors);
	}

	return TArray<FUtilityScoreVector>();
}

static float AddAllScoresBalanced(const TArray<float>& Inputs)
{
	if (Inputs.Num() <= 0)
	{
		return 0;
	}

	float FinalFloat = 1.0f;
	for (const float& InFloat : Inputs)
	{
		//Compensation factor
		const float ModFactor = 1.0f - (1.0f / Inputs.Num());
		const float MakeUpVal = (1.0f - InFloat) * ModFactor;
		const float FinalScore = InFloat + (MakeUpVal * InFloat);

		FinalFloat *= FinalScore;	
	}

	return FinalFloat;
}

static float AddAllScoresAverage(const TArray<float>& Inputs)
{
	if (Inputs.Num() <= 0)
	{
		return 0;
	}

	float FinalFloat = 0;
	for (const float& InFloat : Inputs)
	{
		FinalFloat += InFloat;
	}

	FinalFloat = FinalFloat / float(Inputs.Num());

	return FinalFloat;
}

static float AddAllScoresDefault(const TArray<float>& Inputs)
{
	if (Inputs.Num() <= 0)
	{
		return 0;
	}

	float FinalFloat = 1.0f;
	for (const float& InFloat : Inputs)
	{
		FinalFloat *= InFloat;
	}

	return FinalFloat;
}

float UAIUtilityFunctions::CreateUtilityScore(const TArray<FUtilityScore>& Scorers, EScoringType ScoringType)
{
	if (Scorers.Num() <= 0)
	{
		return 0;
	}

	float Input = 0;
	TArray<float> ScoreValues;

	for (FUtilityScore InScore : Scorers)
	{
		if (!InScore.bCondition)
		{
			return 0;
		}

		Input = InScore.MakeUtilityScore();
		ScoreValues.Add(Input);
	}

	if (ScoreValues.Num() > 0)
	{
		if (ScoringType == EScoringType::ST_Average)
		{
			return AddAllScoresAverage(ScoreValues);
		}
		else if (ScoringType == EScoringType::ST_Balanced)
		{
			return AddAllScoresBalanced(ScoreValues);
		}
		else
		{
			return AddAllScoresDefault(ScoreValues);
		}
	}

	return 0;
}

void UAIUtilityFunctions::PrintUtilityScore(const TArray<FUtilityScore>& Scorers, EScoringType ScoringType, float DisplayTime)
{
	const float ScorerResult = CreateUtilityScore(Scorers, ScoringType);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, DisplayTime, FColor::Yellow, FString::Printf(TEXT("Utility Score: %f"), ScorerResult));
	}
}

//
//Math
//

bool UAIUtilityFunctions::IsWithinCone(float Angle, FVector OriginLocation, FRotator OriginRotation, FVector TargetLocation)
{
	const float HalfAngleDot = FMath::Cos(FMath::DegreesToRadians(Angle));
	const FVector PointDir = (TargetLocation - OriginLocation).GetSafeNormal();
	const FVector PointRotation = OriginRotation.Vector();

	return PointDir.CosineAngle2D(PointRotation) > HalfAngleDot;
}

bool UAIUtilityFunctions::IsWithinConeActor(float Angle, AActor* Origin, AActor* Target)
{
	if (!Origin || !Target)
	{
		return false;
	}

	return IsWithinCone(Angle, Origin->GetActorLocation(), Origin->GetActorRotation(), Target->GetActorLocation());
}

float UAIUtilityFunctions::GetNormalizedAngle(FVector From, FRotator Rotation, FVector To, ENormalizedDotType Type)
{
	const FVector Dir = (To - From).GetSafeNormal();
	const FVector RotVector = Rotation.Vector();
	const float Angle = FVector::DotProduct(Dir, RotVector);

	float Result = 0;

	if(Type == ENormalizedDotType::Forward)
	{ 
		Result = UKismetMathLibrary::MapRangeClamped(Angle, 0.0, 1.0, 0.0, 1.0);
	}
	else if (Type == ENormalizedDotType::Backward)
	{
		Result = UKismetMathLibrary::MapRangeClamped(Angle, -1.0, 0.0, 0.0, 1.0);
	}
	else
	{
		Result = UKismetMathLibrary::MapRangeClamped(Angle, -1.0, 1.0, 0.0, 1.0);
	}

	return Result;
}

void UAIUtilityFunctions::CreateGrid(FVector Origin, const FQueryGridData& Data, TArray<FVector>& Points)
{
	const float Extent = Data.Extent;
	const float Spacing = Data.Spacing;

	const float ItemCount = Extent * 2.0f / Spacing;
	const FVector GridOrigin = Origin - ((Extent - Spacing * 0.5f) * FVector(1,1,0));

	TArray<FVector> TempGridPoints;
	TempGridPoints.Reserve(ItemCount * ItemCount);

	for (int32 IndexX = 0; IndexX < ItemCount; ++IndexX)
	{
		for (int32 IndexY = 0; IndexY < ItemCount; ++IndexY)
		{
			const FVector TestPoint = GridOrigin + FVector(Spacing * IndexX, Spacing * IndexY, 0);
			TempGridPoints.Add(TestPoint);
		}
	}

	Points = TempGridPoints;
}

static bool IsAngleAllowed(const float& SectionAngle, const float& RotationAngle, const float& ArcAngle)
{
	if (ArcAngle != 0)
	{
		const float TestAngleDeg = FMath::RadiansToDegrees(SectionAngle);
		const float AngleDelta = FRotator::NormalizeAxis(TestAngleDeg - RotationAngle);
		return (FMath::Abs(AngleDelta) - 0.01f) < ArcAngle;
	}

	return true;
}

void UAIUtilityFunctions::CreateTorus(FVector Origin, FRotator Rotation, const FQueryTorusData& Data, TArray<FVector>& Points)
{
	const float InnerRadius = Data.InnerRadius;
	const float OuterRadius = Data.OuterRadius;
	const float NumRings = Data.NumRings;
	const float NumPoints = Data.NumPoints;
	const float ArcAngle = FMath::Clamp(Data.ConstraintAngle, 0.0f, 360.0f);
	const float RotationAngle = Rotation.Yaw;

	const float RadiusDelta = (OuterRadius - InnerRadius) / (NumRings - 1);
	const float AngleDelta = 2.0 * PI / NumPoints;
	float SectionAngle = FMath::DegreesToRadians(RotationAngle);

	for (int32 SectionIdx = 0; SectionIdx < NumPoints; SectionIdx++, SectionAngle += AngleDelta)
	{
		if (IsAngleAllowed(SectionAngle, RotationAngle, ArcAngle))
		{
			const float SinValue = FMath::Sin(SectionAngle);
			const float CosValue = FMath::Cos(SectionAngle);

			float RingRadius = InnerRadius;
			for (int32 RingIdx = 0; RingIdx < NumRings; RingIdx++, RingRadius += RadiusDelta)
			{
				const FVector RingPos = FVector(RingRadius * CosValue, RingRadius * SinValue, 0.0f) + Origin;
				Points.Add(RingPos);
			}
		}
	}
}

//
// Query System
//

static UEasyAIQueryObject* CreateQuery(AActor* Querier, const TSubclassOf<UEasyAIQueryObject>& QueryClass)
{
	if (QueryClass.GetDefaultObject() && Querier)
	{
		UEasyAIQueryObject* NewQuery = NewObject<UEasyAIQueryObject>(Querier, QueryClass);
		if (NewQuery)
		{
			NewQuery->InitQuery(Querier);
			return NewQuery;
		}
		else
		{
			UE_LOG(LogEasyAI, Error, TEXT("CreateQuery failed for %s"), *QueryClass->GetName());
		}
	}

	return nullptr;
}

static bool CreateQueryErrorCheck(const FString& QueryType, const AActor* Querier, const TSubclassOf<UEasyAIQueryObject>& QueryClass)
{
	if (!QueryClass.GetDefaultObject())
	{
		UE_LOG(LogEasyAI, Error, TEXT("%s failed. QueryClass is invalid!"), *QueryType);
		return false;
	}

	if (!Querier)
	{
		UE_LOG(LogEasyAI, Error, TEXT("%s failed. Querier is invalid!"), *QueryType);
		return false;
	}

	return true;
}

bool UAIUtilityFunctions::AddNavigationQuery(AActor* Querier, TSubclassOf<UEasyAINavigationQuery> QueryClass, UEasyAINavigationQuery*& Query)
{
	if (!CreateQueryErrorCheck("Add Navigation Query", Querier, QueryClass))
	{
		return false;
	}

	Query = Cast<UEasyAINavigationQuery>(CreateQuery(Querier, QueryClass));
	return Query != nullptr;
}

bool UAIUtilityFunctions::AddActorQuery(AActor* Querier, TSubclassOf<UEasyAIActorQuery> QueryClass, UEasyAIActorQuery*& Query)
{
	if (!CreateQueryErrorCheck("Add Actor Query", Querier, QueryClass))
	{
		return false;
	}

	Query = Cast<UEasyAIActorQuery>(CreateQuery(Querier, QueryClass));
	return Query != nullptr;
}

bool UAIUtilityFunctions::AddLocationQuery(AActor* Querier, TSubclassOf<UEasyAILocationQuery> QueryClass, UEasyAILocationQuery*& Query)
{
	if (!CreateQueryErrorCheck("Add Location Query", Querier, QueryClass))
	{
		return false;
	}

	Query = Cast<UEasyAILocationQuery>(CreateQuery(Querier, QueryClass));
	return Query != nullptr;
}

//
//Navigation
//

ANavigationData* FindNavigationData(UNavigationSystemV1& NavSys, UObject* Owner)
{
	const INavAgentInterface* NavAgent = Cast<INavAgentInterface>(Owner);
	if (NavAgent)
	{
		return NavSys.GetNavDataForProps(NavAgent->GetNavAgentPropertiesRef(), NavAgent->GetNavAgentLocation());
	}

	return NavSys.GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
}

bool UAIUtilityFunctions::ProjectNavigationPointAdvanced(AAIController* AIController, const FVector& Point, FVector& ProjectedPoint, float Extent, float Accuracy, TSubclassOf<UNavigationQueryFilter> FilterClass)
{
	AAIController* AI = AIController;
	if (!AI)
	{
		return false;
	}

	UWorld* World = AI->GetWorld();
	if (!World)
	{
		return false;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	ANavigationData* NavData = FindNavigationData(*NavSys, AI);

	TArray<FVector> GridPoints;
	FQueryGridData Data;

	Data.Extent = Extent;
	Data.Spacing = Extent / Accuracy;

	UAIUtilityFunctions::CreateGrid(Point, Data, GridPoints);

	GridPoints.Sort([Point](const FVector& A, const FVector& B)
	{
		const float DistA = FVector::Dist2D(A, Point);
		const float DistB = FVector::Dist2D(B, Point);

		return DistA + 10.0f < DistB; //Leeway to prevent flipping around
	});

	for (const FVector& TestLoc : GridPoints)
	{
		if (NavSys->K2_ProjectPointToNavigation(AI, TestLoc, ProjectedPoint, NavData, FilterClass))
		{
			return true;
		}
	}

	return false;
}

bool UAIUtilityFunctions::HasNavigationPath(AAIController* AIController, const FVector& Location, bool bHierarchicalTest, TSubclassOf<UNavigationQueryFilter> FilterClass)
{
	AAIController* AI = AIController;
	if (!AI)
	{
		return false;
	}

	UWorld* World = AI->GetWorld();
	if (!World)
	{
		return false;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	const ANavigationData* NavData = FindNavigationData(*NavSys, AI);
	const FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, AI, FilterClass);

	const FPathFindingQuery Query = FPathFindingQuery(AI, *NavData, AI->GetNavAgentLocation(), Location, NavFilter);
	const bool bSuccess = NavSys->TestPathSync(Query, bHierarchicalTest ? EPathFindingMode::Hierarchical : EPathFindingMode::Regular);

	return bSuccess;
}

//
//Actors
//

void UAIUtilityFunctions::RotateTowards(AActor* Actor, FVector Target, ERotateTowardsBlend BlendType, float Duration, bool bForceShortestRotationPath, bool bOnlyYaw, ERotateTowardsAction RotateExec, FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(Actor, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		EasyAIRotateTo* Action = LatentActionManager.FindExistingAction<EasyAIRotateTo>(LatentInfo.CallbackTarget, LatentInfo.UUID);

		const FRotator ActorRotation = (Actor != NULL) ? Actor->GetActorRotation() : FRotator::ZeroRotator;
		FRotator LookAtRot = UKismetMathLibrary::MakeRotFromX(Target - Actor->GetActorLocation());
		
		if (bOnlyYaw)
		{
			LookAtRot.Pitch = 0;
			LookAtRot.Roll = 0;
		}
		
		const FRotator TargetRotation = (Actor != NULL) ? LookAtRot : FRotator::ZeroRotator;

		// If not currently running
		if (Action == NULL)
		{
			if (RotateExec == ERotateTowardsAction::Start)
			{
				// Only act if not running
				Action = new EasyAIRotateTo(Duration, LatentInfo, Actor, BlendType, bForceShortestRotationPath);

				Action->TargetRotation = TargetRotation;
				Action->InitialRotation = ActorRotation;

				LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
			}
		}
		else
		{
			if (RotateExec == ERotateTowardsAction::Start)
			{
				// restarts interpolation
				Action->TotalTime = Duration;
				Action->TimeElapsed = 0.f;

				Action->TargetRotation = TargetRotation;
				Action->InitialRotation = ActorRotation;
			}
			else if (RotateExec == ERotateTowardsAction::Stop)
			{
				// 'Stop' just stops the interpolation where it is
				Action->bInterpolating = false;
			}
			else if (RotateExec == ERotateTowardsAction::Return)
			{
				// Return moves back to the beginning
				Action->TotalTime = Action->TimeElapsed;
				Action->TimeElapsed = 0.f;

				// Set our target to be our initial, and set the new initial to be the current position
				Action->TargetRotation = Action->InitialRotation;
				Action->InitialRotation = ActorRotation;
			}
		}
	}
}

void UAIUtilityFunctions::RotateTowardsInterp(AActor* Actor, FVector Location, float DeltaTime, float Speed, bool bOnlyYaw, bool bConstant)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(Actor, EGetWorldErrorMode::LogAndReturnNull))
	{
		const FRotator LookAtRot = UKismetMathLibrary::MakeRotFromX(Location - Actor->GetActorLocation());

		FRotator NewRot = bConstant 
			? FMath::RInterpConstantTo(Actor->GetActorRotation(), LookAtRot, DeltaTime, Speed) 
			: FMath::RInterpTo(Actor->GetActorRotation(), LookAtRot, DeltaTime, Speed);
		
		if (bOnlyYaw)
		{
			NewRot.Pitch = 0;
			NewRot.Roll = 0;
		}
		
		Actor->SetActorRotation(NewRot);
	}
}

