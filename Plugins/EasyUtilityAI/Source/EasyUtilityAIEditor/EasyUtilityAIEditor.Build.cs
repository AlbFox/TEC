// Easy Utility AI - (C) 2022 by Michael Hegemann

using UnrealBuildTool;

public class EasyUtilityAIEditor : ModuleRules
{
	public EasyUtilityAIEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		

		PublicIncludePaths.AddRange(
			new string[] 
			{
			});


		PrivateIncludePaths.AddRange(
			new string[] {
	
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"GameplayTags",
				"EasyUtilityAI",
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"UnrealEd",
			    "AIModule",
				"Slate",
				"SlateCore",
				"Kismet",
				"KismetCompiler",
				"BlueprintGraph",
				"AssetTools",       // class FAssetTypeActions_Base
			});

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			});
	}
}
