// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FourdLZ4 : ModuleRules
{
	public FourdLZ4(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicIncludePaths.AddRange(
			new string[] {
				ModuleDirectory + "/lz4frame/include"
			}
			);
		
		PublicAdditionalLibraries.Add(ModuleDirectory + "/lz4frame/lib/liblz4_static.lib");

		PrivateIncludePaths.AddRange(
			new string[] {
				// "Runtime/Core/Private/Compression",
				"FourdLZ4/Private",
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
