// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class SimpleDataIntegration : ModuleRules
	{
        public SimpleDataIntegration(TargetInfo Target)
		{
            PrivateIncludePaths.AddRange(
                new string[] {
                    "Editor/BlueprintGraph/Private",
                    "Editor/KismetCompiler/Public"
                }
            );

            PublicDependencyModuleNames.AddRange(
                new string[] {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "InputCore",
                    "Slate",
                    "EditorStyle",
                    "AIModule"
                }
            );

            PrivateDependencyModuleNames.AddRange(
                new string[] {
                    "EditorStyle",
                    "KismetCompiler",
                    "UnrealEd",
                    "GraphEditor",
                    "SlateCore",
                    "Kismet",
                    "PropertyEditor"
                }
            );

            CircularlyReferencedDependentModules.AddRange(
                new string[] {
                    "KismetCompiler",
                    "UnrealEd",
                    "GraphEditor",
                    "Kismet"
                }
            );

            PrivateDependencyModuleNames.AddRange(
 				new string[]
 				{
                    "BlueprintGraph"
 				}
 			);

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "HTTP"
                }
            );

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
				PrivateDependencyModuleNames.AddRange(new string[] { "LibOVRAudio" });
				RuntimeDependencies.Add(new RuntimeDependency("$(EngineDir)/Binaries/ThirdParty/Oculus/Audio/Win64/ovraudio64.dll"));
            }

            AddThirdPartyPrivateStaticDependencies(Target, "DX11Audio");
		}
	}
}