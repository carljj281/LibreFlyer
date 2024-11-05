// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class LibreFlyer : ModuleRules {
    public LibreFlyer(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        string GDKPath = System.Environment.GetEnvironmentVariable("GRDKLatest");
        PublicIncludePaths.Add(GDKPath + "GameKit/Include");
        PublicAdditionalLibraries.Add(GDKPath + "GameKit/Lib/amd64/GameInput.lib");

        PublicDependencyModuleNames.AddRange(
            new string[] { 
                "Core", 
                "CoreUObject", 
                "Engine", 
                "InputCore",
            }
        );
		PrivateDependencyModuleNames.AddRange(new string[] { 
            "Slate",
            "SlateCore",
            "NetCore",
            "UMG",
            "PhysicsCore"
        });
    }
}

