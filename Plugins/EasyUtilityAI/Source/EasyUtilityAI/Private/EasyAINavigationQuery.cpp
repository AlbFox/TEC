// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "EasyAINavigationQuery.h"
#include "AIUtilityFunctions.h"
#include "NavigationSystem.h"


bool UEasyAINavigationQuery::CheckLocation_Implementation(const FVector& Location, FVector& OutLocation)
{
	OutLocation = Location;
	return true;
}

void UEasyAINavigationQuery::GetScorers_Implementation(const FVector& Location, TArray<FUtilityScore>& Scorers)
{
	FUtilityScore TempScore = FUtilityScore();
	Scorers.Add(TempScore);
}

void UEasyAINavigationQuery::GetQueryResults(TArray<FVector>& Locations, bool& bIsValid)
{
	if (!ValidateResultQuery())
	{
		return;
	}

	PreQuery();

	FVector Origin;
	FRotator Rotation;
	GetOrigin(Origin, Rotation);

	TArray<FVector> LocationPoints;

	if (Pattern == EQueryPatternType::Grid)
	{
		UAIUtilityFunctions::CreateGrid(Origin, GridData, LocationPoints);
	}
	else if (Pattern == EQueryPatternType::Torus)
	{
		UAIUtilityFunctions::CreateTorus(Origin, Rotation, TorusData, LocationPoints);
	}

	if (LocationPoints.Num() > 0)
	{
		FUtilityScoreVector UtilityScoreLoc;
		TArray<FUtilityScoreVector> FinalScoreLocations;
		TArray<FUtilityScore> LocScorers;

		for (const FVector& ScoreLoc : LocationPoints)
		{
			FVector ProjectedPoint;
			if (UNavigationSystemV1::K2_ProjectPointToNavigation(Querier, ScoreLoc, ProjectedPoint, NULL, FilterClass, ProjectionExtent))
			{
				ProjectedPoint = ProjectedPoint + FVector(0, 0, ProjectionZOffset);
				GetScorers(ProjectedPoint, LocScorers);

				if (LocScorers.Num() > 0)
				{
					UtilityScoreLoc.Location = ProjectedPoint;
					UtilityScoreLoc.Score = UAIUtilityFunctions::CreateUtilityScore(LocScorers, ScoringType);
					FinalScoreLocations.Add(UtilityScoreLoc);
				}
				else
				{
					UE_LOG(LogEasyAI, Warning, TEXT("GetScoreData returns zero scorers"));
					return;
				}
			}
		}

		if (FinalScoreLocations.Num() > 0)
		{
			const TArray<FUtilityScoreVector> SortedScoreLocs = UAIUtilityFunctions::SortScoredVectors(FinalScoreLocations);
			const uint16 ReducedEntries = GetOutputPercent(SortedScoreLocs.Num());
			uint16 ReducedNum = 0;

			if (ReducedEntries <= 0)
			{
				return;
			}

			for (const FUtilityScoreVector& TestSort : SortedScoreLocs)
			{		
				if (ReducedNum <= ReducedEntries)
				{
					FVector FinalLocation;
					if (CheckLocation(TestSort.Location, FinalLocation))
					{
						Locations.Add(FinalLocation);
					}

					ReducedNum++;
				}
				else
				{
					break;
				}
			}
		}
	}

	if (Locations.Num() > 0)
	{
		bIsValid = true;

		if (bDisplayDebugLocations)
		{
			DisplayDebug(Locations);
		}
	}

}