// (C) 2025 Kimmo Kotajärvi <kimmo.kotajarvi@gmail.com>

#include "P6Benchmark.h"

#define LOCTEXT_NAMESPACE "FP6BenchmarkModule"

DEFINE_LOG_CATEGORY(LogP6Benchmark);

void FP6BenchmarkModule::StartupModule()
{
}

void FP6BenchmarkModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FP6BenchmarkModule, P6Benchmark)