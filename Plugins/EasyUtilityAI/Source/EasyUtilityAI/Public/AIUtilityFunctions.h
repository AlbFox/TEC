// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AIUtilityData.h"
#include "NavigationSystem.h"
#include "UObject/UnrealType.h"
#include "EasyAITaskManager.h"
#include "EasyAINavigationQuery.h"
#include "AI/Navigation/NavQueryFilter.h"
#include "UObject/WeakObjectPtr.h"
#include "Engine/LatentActionManager.h"
#include "LatentActions.h"
#include "AIUtilityFunctions.generated.h"

UENUM()
enum class ERotateTowardsAction : uint8
{
	Start,
	Stop,
	Return,
};

UENUM()
enum class ERotateTowardsBlend : uint8
{
	Linear,
	EaseInOut,
};

UENUM()
enum class ENormalizedDotType : uint8
{
	Full,
	Forward,
	Backward,
};

UCLASS()
class EASYUTILITYAI_API UAIUtilityFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static void DrawDebugLocations(UObject* WorldContextObject, FDrawDebugLocationData DrawData);

	static TArray<FUtilityScoreActor> SortScoredActors(const TArray<FUtilityScoreActor>& ScoredActors);
	static TArray<FUtilityScoreVector> SortScoredVectors(const TArray<FUtilityScoreVector>& ScoredVectors);

public:

	/**
	* Tries to find a Task Manager Component.
	* Works with: Pawn, AIController(self and in Pawn), EasyAIQueryObject
	*
	* @return The Task Manager
	*/
	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Helpers", meta = (WorldContext = "WorldContextObject"))
	static UEasyAITaskManager* GetTaskManager(UObject* WorldContextObject);

	static UEasyAITaskManager* FindTaskManager(UObject* WorldContextObject);

public:

	/**
	* Check if Target is within Cone. Starting from OriginLocation in direction of OriginRotation.
	*
	* @param Angle Half Angle of Cone. 
	* @param OriginLocation Location of check origin.
	* @param OriginRotation Rotation of check origin.
	* @param Target Location of Target to check against.
	* @return if Target is within Angle
	*/
	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Math", meta = (DisplayName = "Is Within Cone"))
	static bool IsWithinCone(float Angle, FVector OriginLocation, FRotator OriginRotation, FVector Target);

	/**
	* Check if Target is within Cone. Actor version.
	*/
	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Math", meta = (DisplayName = "Is Within Cone(Actor)"))
	static bool IsWithinConeActor(float Angle, AActor* Origin, AActor* Target);

	/**
	* Returns the dot product normalized to 0-1. Useful for scoring.
	* 
	* Full: -1 to 1
	* Forward: 0 to 1 
	* Backward: -1 to 0
	* 
	*/
	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Math")
	static float GetNormalizedAngle(FVector From, FRotator FromRotation, FVector To, ENormalizedDotType Type);

	/**
	* Creates a 2D grid of locations.
	*
	* @param Origin - Grid Center.
	* @param Data - The input data.
	* @param Points - Output locations.
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Math", meta = (WorldContext = "WorldContextObject"))
	static void CreateGrid(FVector Origin, const FQueryGridData& Data, TArray<FVector>& Points);

	/**
	* Creates a 2D Torus of locations.
	*
	* @param Origin - Torus Center.
	* @param Rotation - The rotation angle if ConstraintAngle is not zero.
	* @param Data - The input data.
	* @param Points - Output locations.
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Math", meta = (WorldContext = "WorldContextObject"))
	static void CreateTorus(FVector Origin, FRotator Rotation, const FQueryTorusData& Data, TArray<FVector>& Points);

	/**
	* Creates a score from Utility Scorers
	*
	* @param Scorers - The input Scorers.
	* @param ScorigType - The desired scoring type.
	* @return - The calculated utility score. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Math")
	static float CreateUtilityScore(const TArray<FUtilityScore>& Scorers, EScoringType ScoringType = EScoringType::ST_Default);

	/**
	* Prints the output of Utility Scorers to the screen
	*
	* @param Scorers - The input Scorers.
	* @param ScorigType - The desired scoring type.
	* @return - The calculated utility score.
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Math")
	static void PrintUtilityScore(const TArray<FUtilityScore>& Scorers, EScoringType ScoringType = EScoringType::ST_Default, float DisplayTime = 2.0f);

public:

	/**
	* Adds a new navigation query object. This should be set as variable and then you can get the results.
	*
	* @param Querier - Specifiy the Query Origin Actor. If not set, it will default to owner/self.
	* @param QueryClass The query class to use.
	* @param Query The resulting query object.
	* @return If the Query was created successfully.
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Query System", meta = (DefaultToSelf = "Querier"))
	static bool AddNavigationQuery(AActor* Querier, TSubclassOf<UEasyAINavigationQuery> QueryClass, UEasyAINavigationQuery*& Query);

	/**
	* Adds a new actor query object. This should be set as variable and then you can get the results.
	*
	* @param Querier - Specifiy the Query Origin Actor. If not set, it will default to owner/self.
	* @param QueryClass The query class to use.
	* @param Query The resulting query object.
	* @return If the Query was created successfully.
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Query System", meta = (DefaultToSelf = "Querier"))
	static bool AddActorQuery(AActor* Querier, TSubclassOf<UEasyAIActorQuery> QueryClass, UEasyAIActorQuery*& Query);

	/**
	* Adds a new location query object. This should be set as variable and then you can get the results.
	*
	* @param Querier - Specifiy the Query Origin Actor. If not set, it will default to owner/self.
	* @param QueryClass The query class to use.
	* @param Query The resulting query object.
	* @return If the Query was created successfully.
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Utility AI | Query System", meta = (DefaultToSelf = "Querier"))
	static bool AddLocationQuery(AActor* Querier, TSubclassOf<UEasyAILocationQuery> QueryClass, UEasyAILocationQuery*& Query);

public:

	/**
	* A more complex way of projecting a point to the navmesh
	*
	* @param Point The input location to check.
	* @param Extent The size of the projection.
	* @param Accuracy How accurate the projection should be, higher values come at a higher cost.
	* @return If the point was successfully projected.
	*/
	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Navigation", meta = (Extent = "400", Accuracy = "8.0"))
	static bool ProjectNavigationPointAdvanced(AAIController* AIController, const FVector& Point, FVector& ProjectedPoint, float Extent, float Accuracy, TSubclassOf<UNavigationQueryFilter> FilterClass);

	/**
	* Checks if a certain point can be reached.
	*
	* @param Location The input location to check.
	* @param bHierarchicalTest If the faster test shall be used.
	* @param FilterClass Navigation Filter class.
	* @return If the location can be reached.
	*/
	UFUNCTION(BlueprintPure, Category = "Easy Utility AI | Navigation", meta = (DefaultToSelf = "AIController"))
	static bool HasNavigationPath(AAIController* AIController, const FVector& Location, bool bHierarchicalTest=true, TSubclassOf<UNavigationQueryFilter> FilterClass=NULL);

public:

	/**
	 * Rotates Actor towards the desired Location over Duration.
	 */
	UFUNCTION(BlueprintCallable, meta=(Latent, LatentInfo="LatentInfo", WorldContext="WorldContextObject", ExpandEnumAsExecs = "RotateExec", Duration="1.0", bOnlyYaw="true"), Category="Easy Utility AI | Actors")
	static void RotateTowards(AActor* Actor, FVector Location, ERotateTowardsBlend BlendType, float Duration, bool bForceShortestRotationPath, bool bOnlyYaw, ERotateTowardsAction RotateExec, FLatentActionInfo LatentInfo);

	/**
	 * Rotates Actor towards the desired Location by interpolation speed.
	*/
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", bOnlyYaw = "true"), Category = "Easy Utility AI | Actors")
	static void RotateTowardsInterp(AActor* Actor, FVector Location, float DeltaTime, float Speed, bool bOnlyYaw, bool bConstant);

};

class EasyAIRotateTo : public FPendingLatentAction
{

public:
	float TotalTime;
	float TimeElapsed;
	bool bInterpolating;

	FName ExecutionFunction;
	int32 OutputLink;

	FWeakObjectPtr CallbackTarget;

	TWeakObjectPtr<AActor> TargetActor;
	FRotator InitialRotation;
	FRotator TargetRotation;

	ERotateTowardsBlend BlendType;

	bool bForceShortestRotationPath;

	EasyAIRotateTo(float Duration, const FLatentActionInfo& LatentInfo, AActor* Actor, ERotateTowardsBlend BlendType, bool bInForceShortestRotationPath)
		: TotalTime(Duration)
		, TimeElapsed(0.f)
		, bInterpolating(true)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, TargetActor(Actor)
		, InitialRotation(FRotator::ZeroRotator)
		, TargetRotation(FRotator::ZeroRotator)
		, BlendType(BlendType)
		, bForceShortestRotationPath(bInForceShortestRotationPath)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		// Update elapsed time
		TimeElapsed += Response.ElapsedTime();

		bool bComplete = (TimeElapsed >= TotalTime);

		// If we have a component to modify..
		if (TargetActor.IsValid() && bInterpolating)
		{
			// Work out 'Blend Percentage'
			const float BlendExp = 2.f;
			float DurationPct = TimeElapsed / TotalTime;
			float BlendPct;
			if (BlendType == ERotateTowardsBlend::EaseInOut)
			{
				BlendPct = FMath::InterpEaseInOut(0.f, 1.f, DurationPct, BlendExp);
			}
			else
			{
				BlendPct = FMath::Lerp(0.f, 1.f, DurationPct);
			}

			if (TargetActor.IsValid())
			{
				FRotator NewRotation;

				if (bComplete)
				{
					NewRotation = TargetRotation;
				}
				else if (bForceShortestRotationPath)
				{
					// We want the shortest path 
					FQuat AQuat(InitialRotation);
					FQuat BQuat(TargetRotation);

					FQuat Result = FQuat::Slerp(AQuat, BQuat, BlendPct);
					Result.Normalize();
					NewRotation = Result.Rotator();
				}
				else
				{
					// dont care about it being the shortest path - just lerp
					NewRotation = FMath::Lerp(InitialRotation, TargetRotation, BlendPct);
				}

				TargetActor->SetActorRotation(NewRotation);
				
			}
		}

		Response.ElapsedTime();

		Response.FinishAndTriggerIf(bComplete || !bInterpolating, ExecutionFunction, OutputLink, CallbackTarget);
	}

};
