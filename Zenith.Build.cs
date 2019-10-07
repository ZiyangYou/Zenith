// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Zenith : ModuleRules
{
	public Zenith(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "GameplayTasks", "UMG", "Slate", "SlateCore", "GameplayTags", "NavigationSystem", "Navmesh", "CableComponent" });
        PrivateDependencyModuleNames.AddRange(new string[] { "CableComponent" });
        PrivateIncludePathModuleNames.AddRange(new string[] { "CableComponent" });
    }
}
