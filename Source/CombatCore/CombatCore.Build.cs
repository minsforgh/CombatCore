// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CombatCore : ModuleRules
{
	public CombatCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"GameplayTasks",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"CombatCore",
			"CombatCore/Variant_Platforming",
			"CombatCore/Variant_Platforming/Animation",
			"CombatCore/Variant_Combat",
			"CombatCore/Variant_Combat/AI",
			"CombatCore/Variant_Combat/Animation",
			"CombatCore/Variant_Combat/Gameplay",
			"CombatCore/Variant_Combat/Interfaces",
			"CombatCore/Variant_Combat/UI",
			"CombatCore/Variant_SideScrolling",
			"CombatCore/Variant_SideScrolling/AI",
			"CombatCore/Variant_SideScrolling/Gameplay",
			"CombatCore/Variant_SideScrolling/Interfaces",
			"CombatCore/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
