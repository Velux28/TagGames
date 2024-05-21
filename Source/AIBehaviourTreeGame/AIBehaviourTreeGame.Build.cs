// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AIBehaviourTreeGame : ModuleRules
{
	public AIBehaviourTreeGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });
	}
}
