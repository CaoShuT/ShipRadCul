// ShipRadCulModule.h
// UE5 模块声明

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FShipRadCulModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
