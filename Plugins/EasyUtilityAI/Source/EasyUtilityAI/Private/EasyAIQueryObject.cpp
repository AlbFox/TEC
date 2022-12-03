// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "EasyAIQueryObject.h"
#include "AIUtilityFunctions.h"
#include "NavigationSystem.h"

UWorld* UEasyAIQueryObject::GetWorld() const
{
	return World;
}

void UEasyAIQueryObject::InitQuery(AActor* QueryInstigator)
{
	if (QueryInstigator)
	{
		World = GEngine->GetWorldFromContextObject(QueryInstigator, EGetWorldErrorMode::LogAndReturnNull);
		Querier = QueryInstigator;
		OnBeginPlay();
	}
}

void UEasyAIQueryObject::DisplayDebug(const TArray<FVector>& DebugLocs)
{
	FDrawDebugLocationData DrawData;
	DrawData.ZOffset = DebugLocationZOffset;
	DrawData.Locations = DebugLocs;
	DrawData.Duration = DrawDebugDuration;
	UAIUtilityFunctions::DrawDebugLocations(this, DrawData);
}

void UEasyAIQueryObject::GetOrigin_Implementation(FVector& OriginLocation, FRotator& OriginRotation)
{
	if (Querier)
	{
		OriginLocation = Querier->GetActorLocation();
		OriginRotation = Querier->GetActorRotation();
	}
	else
	{
		UE_LOG(LogEasyAI, Warning, TEXT("Query object has no valid origin, because it has no Querier! Please implement GetOrigin manually!"));
	}
}
