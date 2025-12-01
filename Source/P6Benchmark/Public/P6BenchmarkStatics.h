// (C) 2025 Kimmo Kotajärvi <kimmo.kotajarvi@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "P6BenchmarkStatics.generated.h"

class UP6BenchmarkDefinition;

UCLASS()
class P6BENCHMARK_API UP6BenchmarkStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/// Read the ProjectVersion field from DefaultGame.ini
	UFUNCTION(BlueprintPure, Category = "P6|Benchmark")
	static FString GetProjectVersion();

	/// Insert an Insights bookmark event
	UFUNCTION(BlueprintCallable, Category = "P6|Benchmark")
	static void TraceBookmark(const FString& Bookmark);

	/// Get the number of PSOs pending precompiling
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "P6|Benchmark")
	static int GetNumPSOPrecompilesRemaining();

	// Load a level from a benchmark definition and run it
	UFUNCTION(BlueprintCallable, Category = "P6|Benchmark", meta=(WorldContext="WorldContextObject"))
	static void LoadBenchmark(const UObject* WorldContextObject, UP6BenchmarkDefinition* BenchmarkDefinition);
};
