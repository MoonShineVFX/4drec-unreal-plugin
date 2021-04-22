// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FourdRecEditor : ModuleRules
{
	public FourdRecEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				ModuleDirectory + "/Public",
			}
		);
		
		
		PublicAdditionalLibraries.Add(ModuleDirectory + "/Private/LZ4wrapper/liblz4_static.lib");
		
		
		PrivateIncludePaths.AddRange(
			new string[] {
				ModuleDirectory + "/Private",
				ModuleDirectory + "/Private/LZ4wrapper",
				ModuleDirectory + "/Private/Factories",
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"ImageCore",
				"ImageWrapper",
				"ProceduralMeshComponent",
				"FourdRecRuntime",
				"UnrealEd",
				"TextureCompressor"
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
