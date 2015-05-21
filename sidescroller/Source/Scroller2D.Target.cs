

using UnrealBuildTool;
using System.Collections.Generic;

public class Scroller2DTarget : TargetRules
{
	public Scroller2DTarget(TargetInfo Target)
	{
		Type = TargetType.Game;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "Scroller2D" } );
	}
}
