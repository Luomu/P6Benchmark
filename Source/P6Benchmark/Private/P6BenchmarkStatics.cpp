// (C) 2025 Kimmo Kotajärvi <kimmo.kotajarvi@gmail.com>

#include "P6BenchmarkStatics.h"

#include "P6Benchmark.h" //for logging

// #include "Algo/SelectRandomWeighted.h"
// #include "AnimationSharingManager.h"
// #include "Components/SkeletalMeshComponent.h"
// #include "Engine/Engine.h"
// #include "Misc/FileHelper.h"
#include "Misc/NetworkVersion.h"
// #include "ProceduralMeshComponent.h"
// #include "Rendering/SkeletalMeshRenderData.h"
#include "Kismet/GameplayStatics.h"
#include "P6BenchmarkDefinition.h"
#include "ShaderPipelineCache.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(P6BenchmarkStatics)

FString UP6BenchmarkStatics::GetProjectVersion()
{
	// Might seem a little odd, but that's where it's accessible from
	// Note it can be overridden by FNetworkVersion::SetProjectVersion
	return FNetworkVersion::GetProjectVersion();
}

void UP6BenchmarkStatics::TraceBookmark(const FString& Bookmark)
{
	TRACE_BOOKMARK(TEXT("%s"), *Bookmark);
}

int UP6BenchmarkStatics::GetNumPSOPrecompilesRemaining()
{
#if UE_SERVER
	return 0;
#else
	return FShaderPipelineCache::NumPrecompilesRemaining();
#endif
}

void UP6BenchmarkStatics::LoadBenchmark(const UObject* WorldContextObject, UP6BenchmarkDefinition* BenchmarkDefinition)
{
	if (!WorldContextObject || !WorldContextObject->GetWorld())
	{
		FFrame::KismetExecutionMessage(TEXT("No benchmark world context object"), ELogVerbosity::Warning);
		return;
	}

	if (!BenchmarkDefinition)
	{
		FFrame::KismetExecutionMessage(TEXT("No benchmark definition specified"), ELogVerbosity::Warning);
		return;
	}

	const FString MapName = BenchmarkDefinition->Level.GetAssetName();
	if (MapName.IsEmpty())
	{
		FFrame::KismetExecutionMessage(TEXT("Benchmark definition has no valid level"), ELogVerbosity::Warning);
		return;
	}

	const FString GameModeName = BenchmarkDefinition->GameModeOverride.ToString().IsEmpty()
	                                 ? "/Script/P6Benchmark.P6BenchmarkGameMode"
	                                 : BenchmarkDefinition->GameModeOverride.ToString();

	// Override game mode & store the definition path
	// no need to prepend this with a ? since OpenLevel handles that part for us
	FString OptionsString;
	OptionsString += "game=" + GameModeName;
	OptionsString += "?benchmark=" + BenchmarkDefinition->GetPathName();

	UE_LOG(LogP6Benchmark, Log, TEXT("Opening map %s?%s"), *MapName, *OptionsString);
	UGameplayStatics::OpenLevel(WorldContextObject->GetWorld(), *MapName, true, OptionsString);
}
