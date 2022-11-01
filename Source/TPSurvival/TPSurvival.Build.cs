// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TPSurvival : ModuleRules
{
	public TPSurvival(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
