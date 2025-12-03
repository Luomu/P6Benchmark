// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class P6Benchmark : ModuleRules
{
	public P6Benchmark(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"LevelSequence",
				"UMG" // results widget
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"DeveloperSettings",
				"Engine",
				"MovieScene", // Sequence player
				"RenderCore", // For PSO status
				//"Slate",
				//"SlateCore",
			}
			);
	}
}
