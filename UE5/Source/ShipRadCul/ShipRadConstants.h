// ShipRadConstants.h
// 船舶红外辐射计算所需的全部物理常数和天气参数 - UE5版本
// 对应Unity版本: Constants.cs

#pragma once

#include "CoreMinimal.h"
#include "ShipRadConstants.generated.h"

/**
 * 天气类型枚举
 */
UENUM(BlueprintType)
enum class EWeatherType : uint8
{
    Sunny   UMETA(DisplayName = "晴天"),
    Cloudy  UMETA(DisplayName = "多云"),
    Rainy   UMETA(DisplayName = "雨天"),
    Foggy   UMETA(DisplayName = "雾天"),
    Snowy   UMETA(DisplayName = "雪天")
};

/**
 * 散射类型枚举
 */
UENUM(BlueprintType)
enum class EScatteringType : uint8
{
    Rayleigh    UMETA(DisplayName = "瑞利散射"),
    Mie         UMETA(DisplayName = "米氏散射"),
    Cloud       UMETA(DisplayName = "云散射"),
    Rain        UMETA(DisplayName = "雨滴散射"),
    Fog         UMETA(DisplayName = "雾散射"),
    Snow        UMETA(DisplayName = "雪花散射")
};

/**
 * 船舶红外辐射计算所需的全部物理常数
 */
namespace ShipRadConstants
{
    // 基础物理常数
    constexpr float ATR = PI * 2.0f / 360.0f;          // 角度转弧度
    constexpr float Isc = 1353.0f;                       // 太阳常数 W/m^2

    // 时间参数
    constexpr float Day = 180.0f;                        // 距离一月一日的天数

    // 大气传输经验系数
    constexpr float Sca1 = 0.275f;
    constexpr float Sca2 = 0.53f;

    // ==================== 晴天参数 ====================
    constexpr float Sunny_Pm = 0.8f;
    constexpr float Sunny_Tau = 0.9f;
    constexpr float Sunny_AOD = 0.1f;
    constexpr float Sunny_IR_Tau = 0.95f;
    constexpr float Sunny_Visibility = 30000.0f;
    constexpr float Sunny_Tf = 298.0f;
    constexpr float Sunny_Tgrd = 303.0f;
    constexpr float Sunny_Fsi = 0.6f;
    constexpr float Sunny_Vwind = 2.0f;
    constexpr float Sunny_RH = 0.4f;
    constexpr float Sunny_Pressure = 1013.25f;
    constexpr float Sunny_Cloud = 0.1f;
    constexpr float Sunny_RainRate = 0.0f;

    // ==================== 多云参数 ====================
    constexpr float Cloudy_Pm = 0.5f;
    constexpr float Cloudy_Tau = 0.7f;
    constexpr float Cloudy_AOD = 0.3f;
    constexpr float Cloudy_IR_Tau = 0.8f;
    constexpr float Cloudy_Visibility = 10000.0f;
    constexpr float Cloudy_Tf = 293.0f;
    constexpr float Cloudy_Tgrd = 295.0f;
    constexpr float Cloudy_Fsi = 0.5f;
    constexpr float Cloudy_Vwind = 3.0f;
    constexpr float Cloudy_RH = 0.7f;
    constexpr float Cloudy_Pressure = 1008.0f;
    constexpr float Cloudy_Cloud = 0.7f;
    constexpr float Cloudy_RainRate = 0.0f;

    // ==================== 雨天参数 ====================
    constexpr float Rainy_Pm = 0.3f;
    constexpr float Rainy_Tau = 0.4f;
    constexpr float Rainy_AOD = 0.5f;
    constexpr float Rainy_IR_Tau = 0.6f;
    constexpr float Rainy_Visibility = 5000.0f;
    constexpr float Rainy_Tf = 288.0f;
    constexpr float Rainy_Tgrd = 290.0f;
    constexpr float Rainy_Fsi = 0.4f;
    constexpr float Rainy_Vwind = 4.0f;
    constexpr float Rainy_RH = 0.95f;
    constexpr float Rainy_Pressure = 1005.0f;
    constexpr float Rainy_Cloud = 0.9f;
    constexpr float Rainy_RainRate = 5.0f;

    // ==================== 雾天参数 ====================
    constexpr float Foggy_Pm = 0.1f;
    constexpr float Foggy_Tau = 0.2f;
    constexpr float Foggy_AOD = 2.0f;
    constexpr float Foggy_IR_Tau = 0.3f;
    constexpr float Foggy_Visibility = 500.0f;
    constexpr float Foggy_Tf = 285.0f;
    constexpr float Foggy_Tgrd = 286.0f;
    constexpr float Foggy_Fsi = 0.3f;
    constexpr float Foggy_Vwind = 1.0f;
    constexpr float Foggy_RH = 1.0f;
    constexpr float Foggy_Pressure = 1010.0f;
    constexpr float Foggy_Cloud = 0.4f;
    constexpr float Foggy_RainRate = 0.0f;

    // ==================== 雪天参数 ====================
    constexpr float Snowy_Pm = 0.2f;
    constexpr float Snowy_Tau = 0.3f;
    constexpr float Snowy_AOD = 0.6f;
    constexpr float Snowy_IR_Tau = 0.5f;
    constexpr float Snowy_Visibility = 2000.0f;
    constexpr float Snowy_Tf = 273.0f;
    constexpr float Snowy_Tgrd = 273.0f;
    constexpr float Snowy_Fsi = 0.5f;
    constexpr float Snowy_Vwind = 3.0f;
    constexpr float Snowy_RH = 0.8f;
    constexpr float Snowy_Pressure = 1015.0f;
    constexpr float Snowy_Cloud = 0.8f;
    constexpr float Snowy_RainRate = 0.5f;

    // 地面反射率
    constexpr float Sunny_Rhod = 0.3f;
    constexpr float Cloudy_Rhod = 0.2f;
    constexpr float Rainy_Rhod = 0.15f;
    constexpr float Foggy_Rhod = 0.1f;
    constexpr float Snowy_Rhod = 0.8f;

    // 地面发射率
    constexpr float Sunny_Epslgrd = 0.95f;
    constexpr float Cloudy_Epslgrd = 0.98f;
    constexpr float Rainy_Epslgrd = 0.98f;
    constexpr float Foggy_Epslgrd = 0.98f;
    constexpr float Snowy_Epslgrd = 0.85f;

    // 散射系数
    constexpr float Rayleigh_Beta = 0.1f;
    constexpr float Mie_Beta = 0.5f;
    constexpr float Cloud_Beta = 2.0f;
    constexpr float Rain_Beta = 1.5f;
    constexpr float Fog_Beta = 3.0f;
    constexpr float Snow_Beta = 2.5f;

    // 通用常数
    constexpr float Beta = 60.0f;                        // 斜面倾角
    constexpr float Sigma = 5.67e-8f;                    // 斯蒂芬玻尔兹曼常数
    constexpr float Aexp = 0.51f;
    constexpr float Bexp = 0.208f;

    // 对流换热
    constexpr float Re = 10000.0f;                       // 雷诺数
    constexpr float Pr = 0.75f;                          // 普朗特数
    constexpr float Us = 1.0f;
    constexpr float Uinf = 1.0f;
    constexpr float Dbar = 1.0f;
    constexpr float Dcan = 0.4f;
    constexpr float Airc = 0.0242f;
    constexpr float Tamb = 295.0f;

    // 辐射常数
    constexpr float Radc1 = 3.742e-16f;
    constexpr float Radc2 = 1.439e-2f;
    constexpr float Wave1 = 3e-6f;
    constexpr float Wave2 = 5e-6f;
    constexpr float Rn = 1.0f;
    constexpr float AirDensity = 1.225f;
    constexpr float AirRatio = 1.0f;
    constexpr float Ch = 1.0f;
    constexpr float SunH = 30.0f;

    // 衰减系数 (dB/km)
    constexpr float Rain_Attenuation = 0.5f;
    constexpr float Snow_Attenuation = 1.0f;
    constexpr float Fog_Attenuation = 5.0f;

    // Ångström指数
    constexpr float Sunny_Angstrom = 1.3f;
    constexpr float Cloudy_Angstrom = 1.0f;
    constexpr float Rainy_Angstrom = 0.5f;
    constexpr float Foggy_Angstrom = 0.2f;
    constexpr float Snowy_Angstrom = 0.3f;
}
