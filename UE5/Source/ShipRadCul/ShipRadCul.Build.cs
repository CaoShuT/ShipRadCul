// ShipRadCul.Build.cs
// UE5 模块构建文件

using UnrealBuildTool;

public class ShipRadCul : ModuleRules
{
    public ShipRadCul(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "RenderCore",
            "RHI"
        });
    }
}
