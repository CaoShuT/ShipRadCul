// ShipRadCalculation.h
// 船舶红外辐射核心计算引擎 - UE5版本
// 对应Unity版本: ShipRadCalculation.cs

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ShipType.h"
#include "ShipRadConstants.h"
#include "WeatherStabilitySettings.h"
#include "ShipRadCalculation.generated.h"

/**
 * UShipRadCalculation
 * 
 * 核心物理计算引擎，负责：
 * - 太阳辐射（直接、散射、地面反射）计算
 * - 大气长波辐射计算
 * - 热平衡方程（Stefan-Boltzmann + 对流换热）
 * - 牛顿迭代法求解平衡温度
 * - 零视距红外辐射亮度计算
 * - 大气透过率及天气衰减
 * - 温度和辐射的平滑稳定处理
 */
UCLASS(BlueprintType)
class SHIPRADCUL_API UShipRadCalculation : public UObject
{
    GENERATED_BODY()

public:
    UShipRadCalculation();

    // ==================== 公共接口 ====================

    /** 设置天气类型 */
    UFUNCTION(BlueprintCallable, Category = "ShipRad|Weather")
    void SetWeather(EWeatherType Weather);

    /** 获取当前天气类型 */
    UFUNCTION(BlueprintPure, Category = "ShipRad|Weather")
    EWeatherType GetWeatherType() const;

    /** 获取环境温度 (K) */
    UFUNCTION(BlueprintPure, Category = "ShipRad|Environment")
    float GetAmbientTemperature() const;

    /** 获取当前能见度 (m) */
    UFUNCTION(BlueprintPure, Category = "ShipRad|Environment")
    float GetVisibility() const;

    /**
     * 计算指定部件的平衡温度
     * @param Part 船舶部件类型
     * @param FireCount 开火次数（仅对武器部件有效）
     * @param EnginePower 发动机功率（仅对动力部件有效）
     * @param DeltaTime 帧间隔时间(秒)
     * @return 稳定后的温度 (K)
     */
    UFUNCTION(BlueprintCallable, Category = "ShipRad|Calculation")
    float GetTemperature(EShipType Part, int32 FireCount, float EnginePower, float DeltaTime);

    /**
     * 计算指定部件的零视距红外辐射亮度
     * @param Part 船舶部件类型
     * @param DeltaTime 帧间隔时间(秒)
     * @return 稳定后的辐射亮度 (W/m²·sr)
     */
    UFUNCTION(BlueprintCallable, Category = "ShipRad|Calculation")
    float CalculateLdetzeroview(EShipType Part, float DeltaTime);

    /** 获取天气描述信息 */
    UFUNCTION(BlueprintPure, Category = "ShipRad|Weather")
    FString GetWeatherDescription() const;

    /** 获取天气影响摘要 */
    UFUNCTION(BlueprintPure, Category = "ShipRad|Weather")
    FString GetWeatherImpactSummary() const;

private:
    // ==================== 天气状态 ====================
    EWeatherType CurrentWeather;

    // 大气传输参数
    float CurrentPm;
    float CurrentTau;
    float CurrentAOD;
    float CurrentIR_Tau;
    float CurrentVisibility;
    float CurrentTf;
    float CurrentTgrd;
    float CurrentFsi;
    float CurrentVwind;
    float CurrentRH;
    float CurrentPressure;
    float CurrentCloud;
    float CurrentRainRate;
    float CurrentRhod;
    float CurrentEpslgrd;

    EScatteringType CurrentScattering;

    // ==================== 部件材料属性 ====================
    // 吸收率 [3波段]
    static const float AShipBody[3];
    static const float AShipCannon[3];
    static const float AAirDefenseGun[3];
    static const float AShipMissile[3];
    static const float AChimney[3];
    static const float ABoilerRoom[3];

    // 发射率
    static constexpr float EShipBody = 0.75f;
    static constexpr float EShipCannon_Val = 0.85f;
    static constexpr float EAirDefenseGun = 0.85f;
    static constexpr float EShipMissile = 0.75f;
    static constexpr float EChimney = 0.9f;
    static constexpr float EBoilerRoom = 0.9f;

    // 反射率 [2波段]
    static const float RShipBody[2];
    static const float RShipCannon[2];
    static const float RAirDefenseGun[2];
    static const float RShipMissile[2];
    static const float RChimney[2];
    static const float RBoilerRoom[2];

    // ==================== 状态变量 ====================
    float Qsundir, Qsunsca, Qsunearth, Qsky, Qearth;
    float Conve1, Conve2;
    float Eabsorbenvir, EfireVal, EengineVal;
    float Einternal;

    // 各部件温度
    float TShipBody, TShipCannon_T, TAirDefenseGun;
    float TShipMissile, TChimney_T, TBoilerRoom_T;

    // ==================== 稳定性控制 ====================
    TMap<EWeatherType, FWeatherStabilitySettings> StabilitySettings;
    float LastCalculatedTemperature;
    float LastCalculatedRadiation;
    float SmoothedTemperature;
    float SmoothedRadiation;
    TArray<float> TemperatureHistory;
    TArray<float> RadiationHistory;
    static constexpr int32 HISTORY_SIZE = 5;
    /** 平滑帧率因子 (与Unity中Time.deltaTime * 30f对应) */
    static constexpr float SMOOTHING_FRAMERATE_FACTOR = 30.0f;
    bool bIsInitialized;

    // ==================== 内部计算方法 ====================
    void UpdateWeatherParameters();
    void SetSunnyParameters();
    void SetCloudyParameters();
    void SetRainyParameters();
    void SetFoggyParameters();
    void SetSnowyParameters();

    void Initialization();
    void InitializeStabilitySettings();

    // 热平衡方程
    float Expression(EShipType Part, float T) const;
    float DerExpression(EShipType Part, float T) const;
    float CalculateTemperature(EShipType Part, float T, float Precision, int32 MaxCyc, float DeltaTime);

    // 稳定性处理
    float StabilizeTemperature(float RawTemperature, float DeltaTime);
    float StabilizeRadiation(float RawRadiation, float DeltaTime);

    // 环境辐射计算
    float CalculateEabsorbenvirWithAtmosphere(const float* Absorb) const;
    float CalculateQsundirWithAtmosphere(float SunH, float Day) const;
    float CalculateQsunscaWithAtmosphere(float SunH, float InBeta) const;
    float CalculateQsunearthWithAtmosphere(float InQsundir, float InBeta) const;
    float CalculateQskyWithAtmosphere(float Tf, float Fsi) const;
    float CalculateQearthWithAtmosphere(float Tgrd, float Fsi) const;

    // 对流换热
    float CalculateConvection1WithWind() const;
    float CalculateConvection2WithWind() const;

    // 热源
    float CalculateEengine(float Power) const;
    float CalculateEfire(int32 FireCount) const;

    // 辐射计算
    float Calculateapprox(float Wave, float T, float Emm) const;
    float CalculateAtmosphericTransmittance(float Distance, float Wavelength) const;
    float CalculatePrecipitationAttenuation(float Distance, float PrecipitationRate) const;

    // 辅助查询
    float GetScatteringCoefficient() const;
    float GetWeatherScatteringFactor() const;
    float GetSkyRadiationCorrection() const;
    float GetWeatherAttenuationFactor() const;
    float GetAngstromExponent() const;
};
