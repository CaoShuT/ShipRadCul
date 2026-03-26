// ShipType.h
// 船舶部件类型枚举 - UE5版本
// 对应Unity版本: ShipType.cs

#pragma once

#include "CoreMinimal.h"
#include "ShipType.generated.h"

/**
 * 船舶部件类型枚举
 * 定义了不同的船舶组件，每种组件具有不同的热辐射特性
 */
UENUM(BlueprintType)
enum class EShipType : uint8
{
    ShipBody        UMETA(DisplayName = "船体"),
    ShipCannon      UMETA(DisplayName = "大炮"),
    AirDefenseGun   UMETA(DisplayName = "防空炮"),
    ShipMissile     UMETA(DisplayName = "导弹发射器"),
    Chimney         UMETA(DisplayName = "烟囱"),
    BoilerRoom      UMETA(DisplayName = "锅炉舱")
};
