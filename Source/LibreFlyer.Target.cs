// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class LibreFlyerTarget : TargetRules
{
	public LibreFlyerTarget( TargetInfo Target) : base(Target)
	{

        Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "LibreFlyer"} );
	}
}
