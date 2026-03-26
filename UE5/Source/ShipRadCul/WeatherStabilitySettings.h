// WeatherStabilitySettings.h
// 天气稳定性设置结构体 - UE5版本
// 对应Unity版本: ShipRadCalculation.cs 中的 WeatherStabilitySettings 类

#pragma once

#include "CoreMinimal.h"
#include "WeatherStabilitySettings.generated.h"

/**
 * 天气稳定性设置
 * 用于控制不同天气条件下温度和辐射计算的平滑、偏差和限制参数
 */
USTRUCT(BlueprintType)
struct FWeatherStabilitySettings
{
    GENERATED_BODY()

    /** 温度平滑强度 (0-1, 越低平滑越强) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stability", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float TemperatureSmoothing = 0.5f;

    /** 辐射平滑强度 (0-1, 越低平滑越强) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stability", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float RadiationSmoothing = 0.3f;

    /** 最小辐射值 (W/m²·sr) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stability")
    float MinRadiation = 0.001f;

    /** 最大辐射值 (W/m²·sr) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stability")
    float MaxRadiation = 10.0f;

    /** 温度偏差补偿 (K) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stability")
    float TemperatureBias = 0.0f;

    /** 辐射偏差补偿 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stability")
    float RadiationBias = 0.0f;

    /** 是否使用低通滤波器 (用于恶劣天气) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stability")
    bool bUseLowPassFilter = true;
};
