// (C) 2025 Kimmo Kotajärvi <kimmo.kotajarvi@gmail.com>

#pragma once

#include "Engine/DeveloperSettings.h"

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "P6BenchmarkSettings.generated.h"

// Config values that will appear in Project Settings
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "P6 Benchmark Settings"))
class P6BENCHMARK_API UP6BenchmarkSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditDefaultsOnly, BlueprintReadOnly, Category = P6Benchmark, meta = (MetaClass = "/Script/UMG.UserWidget"))
	FSoftClassPath ResultsWidgetClass;
};
