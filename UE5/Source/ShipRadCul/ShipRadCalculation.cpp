// ShipRadCalculation.cpp
// 船舶红外辐射核心计算引擎实现 - UE5版本
// 对应Unity版本: ShipRadCalculation.cs

#include "ShipRadCalculation.h"

// ==================== 静态材料属性定义 ====================
const float UShipRadCalculation::AShipBody[3] = { 0.85f, 0.8f, 0.65f };
const float UShipRadCalculation::AShipCannon[3] = { 0.7f, 0.7f, 0.7f };
const float UShipRadCalculation::AAirDefenseGun[3] = { 0.7f, 0.7f, 0.7f };
const float UShipRadCalculation::AShipMissile[3] = { 0.85f, 0.8f, 0.65f };
const float UShipRadCalculation::AChimney[3] = { 0.9f, 0.8f, 0.75f };
const float UShipRadCalculation::ABoilerRoom[3] = { 0.9f, 0.8f, 0.75f };

const float UShipRadCalculation::RShipBody[2] = { 0.2f, 0.25f };
const float UShipRadCalculation::RShipCannon[2] = { 0.15f, 0.2f };
const float UShipRadCalculation::RAirDefenseGun[2] = { 0.15f, 0.2f };
const float UShipRadCalculation::RShipMissile[2] = { 0.2f, 0.25f };
const float UShipRadCalculation::RChimney[2] = { 0.1f, 0.2f };
const float UShipRadCalculation::RBoilerRoom[2] = { 0.1f, 0.2f };

// ==================== 构造函数 ====================
UShipRadCalculation::UShipRadCalculation()
    : CurrentWeather(EWeatherType::Sunny)
    , CurrentPm(0.0f), CurrentTau(0.0f), CurrentAOD(0.0f)
    , CurrentIR_Tau(0.0f), CurrentVisibility(0.0f)
    , CurrentTf(0.0f), CurrentTgrd(0.0f), CurrentFsi(0.0f)
    , CurrentVwind(0.0f), CurrentRH(0.0f), CurrentPressure(0.0f)
    , CurrentCloud(0.0f), CurrentRainRate(0.0f)
    , CurrentRhod(0.0f), CurrentEpslgrd(0.0f)
    , CurrentScattering(EScatteringType::Mie)
    , Qsundir(0.0f), Qsunsca(0.0f), Qsunearth(0.0f)
    , Qsky(0.0f), Qearth(0.0f)
    , Conve1(0.0f), Conve2(0.0f)
    , Eabsorbenvir(0.0f), EfireVal(0.0f), EengineVal(0.0f)
    , Einternal(450.0f)
    , TShipBody(300.0f), TShipCannon_T(300.0f), TAirDefenseGun(300.0f)
    , TShipMissile(300.0f), TChimney_T(300.0f), TBoilerRoom_T(300.0f)
    , LastCalculatedTemperature(300.0f), LastCalculatedRadiation(0.0f)
    , SmoothedTemperature(300.0f), SmoothedRadiation(0.0f)
    , bIsInitialized(false)
{
}

// ==================== 公共接口 ====================

void UShipRadCalculation::SetWeather(EWeatherType Weather)
{
    CurrentWeather = Weather;
    UpdateWeatherParameters();
    InitializeStabilitySettings();
}

EWeatherType UShipRadCalculation::GetWeatherType() const
{
    return CurrentWeather;
}

float UShipRadCalculation::GetAmbientTemperature() const
{
    return CurrentTf;
}

float UShipRadCalculation::GetVisibility() const
{
    return CurrentVisibility;
}

// ==================== 天气参数更新 ====================

void UShipRadCalculation::UpdateWeatherParameters()
{
    switch (CurrentWeather)
    {
    case EWeatherType::Sunny:   SetSunnyParameters();  break;
    case EWeatherType::Cloudy:  SetCloudyParameters(); break;
    case EWeatherType::Rainy:   SetRainyParameters();  break;
    case EWeatherType::Foggy:   SetFoggyParameters();  break;
    case EWeatherType::Snowy:   SetSnowyParameters();  break;
    }
}

void UShipRadCalculation::SetSunnyParameters()
{
    CurrentPm = ShipRadConstants::Sunny_Pm;
    CurrentTau = ShipRadConstants::Sunny_Tau;
    CurrentAOD = ShipRadConstants::Sunny_AOD;
    CurrentIR_Tau = ShipRadConstants::Sunny_IR_Tau;
    CurrentVisibility = ShipRadConstants::Sunny_Visibility;
    CurrentTf = ShipRadConstants::Sunny_Tf;
    CurrentTgrd = ShipRadConstants::Sunny_Tgrd;
    CurrentFsi = ShipRadConstants::Sunny_Fsi;
    CurrentVwind = ShipRadConstants::Sunny_Vwind;
    CurrentRH = ShipRadConstants::Sunny_RH;
    CurrentPressure = ShipRadConstants::Sunny_Pressure;
    CurrentCloud = ShipRadConstants::Sunny_Cloud;
    CurrentRainRate = ShipRadConstants::Sunny_RainRate;
    CurrentRhod = ShipRadConstants::Sunny_Rhod;
    CurrentEpslgrd = ShipRadConstants::Sunny_Epslgrd;
    CurrentScattering = EScatteringType::Mie;
}

void UShipRadCalculation::SetCloudyParameters()
{
    CurrentPm = ShipRadConstants::Cloudy_Pm;
    CurrentTau = ShipRadConstants::Cloudy_Tau;
    CurrentAOD = ShipRadConstants::Cloudy_AOD;
    CurrentIR_Tau = ShipRadConstants::Cloudy_IR_Tau;
    CurrentVisibility = ShipRadConstants::Cloudy_Visibility;
    CurrentTf = ShipRadConstants::Cloudy_Tf;
    CurrentTgrd = ShipRadConstants::Cloudy_Tgrd;
    CurrentFsi = ShipRadConstants::Cloudy_Fsi;
    CurrentVwind = ShipRadConstants::Cloudy_Vwind;
    CurrentRH = ShipRadConstants::Cloudy_RH;
    CurrentPressure = ShipRadConstants::Cloudy_Pressure;
    CurrentCloud = ShipRadConstants::Cloudy_Cloud;
    CurrentRainRate = ShipRadConstants::Cloudy_RainRate;
    CurrentRhod = ShipRadConstants::Cloudy_Rhod;
    CurrentEpslgrd = ShipRadConstants::Cloudy_Epslgrd;
    CurrentScattering = EScatteringType::Cloud;
}

void UShipRadCalculation::SetRainyParameters()
{
    CurrentPm = ShipRadConstants::Rainy_Pm;
    CurrentTau = ShipRadConstants::Rainy_Tau;
    CurrentAOD = ShipRadConstants::Rainy_AOD;
    CurrentIR_Tau = ShipRadConstants::Rainy_IR_Tau;
    CurrentVisibility = ShipRadConstants::Rainy_Visibility;
    CurrentTf = ShipRadConstants::Rainy_Tf;
    CurrentTgrd = ShipRadConstants::Rainy_Tgrd;
    CurrentFsi = ShipRadConstants::Rainy_Fsi;
    CurrentVwind = ShipRadConstants::Rainy_Vwind;
    CurrentRH = ShipRadConstants::Rainy_RH;
    CurrentPressure = ShipRadConstants::Rainy_Pressure;
    CurrentCloud = ShipRadConstants::Rainy_Cloud;
    CurrentRainRate = ShipRadConstants::Rainy_RainRate;
    CurrentRhod = ShipRadConstants::Rainy_Rhod;
    CurrentEpslgrd = ShipRadConstants::Rainy_Epslgrd;
    CurrentScattering = EScatteringType::Rain;
}

void UShipRadCalculation::SetFoggyParameters()
{
    CurrentPm = ShipRadConstants::Foggy_Pm;
    CurrentTau = ShipRadConstants::Foggy_Tau;
    CurrentAOD = ShipRadConstants::Foggy_AOD;
    CurrentIR_Tau = ShipRadConstants::Foggy_IR_Tau;
    CurrentVisibility = ShipRadConstants::Foggy_Visibility;
    CurrentTf = ShipRadConstants::Foggy_Tf;
    CurrentTgrd = ShipRadConstants::Foggy_Tgrd;
    CurrentFsi = ShipRadConstants::Foggy_Fsi;
    CurrentVwind = ShipRadConstants::Foggy_Vwind;
    CurrentRH = ShipRadConstants::Foggy_RH;
    CurrentPressure = ShipRadConstants::Foggy_Pressure;
    CurrentCloud = ShipRadConstants::Foggy_Cloud;
    CurrentRainRate = ShipRadConstants::Foggy_RainRate;
    CurrentRhod = ShipRadConstants::Foggy_Rhod;
    CurrentEpslgrd = ShipRadConstants::Foggy_Epslgrd;
    CurrentScattering = EScatteringType::Fog;
}

void UShipRadCalculation::SetSnowyParameters()
{
    CurrentPm = ShipRadConstants::Snowy_Pm;
    CurrentTau = ShipRadConstants::Snowy_Tau;
    CurrentAOD = ShipRadConstants::Snowy_AOD;
    CurrentIR_Tau = ShipRadConstants::Snowy_IR_Tau;
    CurrentVisibility = ShipRadConstants::Snowy_Visibility;
    CurrentTf = ShipRadConstants::Snowy_Tf;
    CurrentTgrd = ShipRadConstants::Snowy_Tgrd;
    CurrentFsi = ShipRadConstants::Snowy_Fsi;
    CurrentVwind = ShipRadConstants::Snowy_Vwind;
    CurrentRH = ShipRadConstants::Snowy_RH;
    CurrentPressure = ShipRadConstants::Snowy_Pressure;
    CurrentCloud = ShipRadConstants::Snowy_Cloud;
    CurrentRainRate = ShipRadConstants::Snowy_RainRate;
    CurrentRhod = ShipRadConstants::Snowy_Rhod;
    CurrentEpslgrd = ShipRadConstants::Snowy_Epslgrd;
    CurrentScattering = EScatteringType::Snow;
}

// ==================== 稳定性设置 ====================

void UShipRadCalculation::InitializeStabilitySettings()
{
    if (StabilitySettings.Num() == 0)
    {
        // 晴天
        FWeatherStabilitySettings SunnySettings;
        SunnySettings.TemperatureSmoothing = 0.8f;
        SunnySettings.RadiationSmoothing = 0.7f;
        SunnySettings.MinRadiation = 0.01f;
        SunnySettings.MaxRadiation = 20.0f;
        SunnySettings.TemperatureBias = 0.0f;
        SunnySettings.RadiationBias = 0.0f;
        SunnySettings.bUseLowPassFilter = false;
        StabilitySettings.Add(EWeatherType::Sunny, SunnySettings);

        // 多云
        FWeatherStabilitySettings CloudySettings;
        CloudySettings.TemperatureSmoothing = 0.6f;
        CloudySettings.RadiationSmoothing = 0.5f;
        CloudySettings.MinRadiation = 0.005f;
        CloudySettings.MaxRadiation = 15.0f;
        CloudySettings.TemperatureBias = -2.0f;
        CloudySettings.RadiationBias = -0.2f;
        CloudySettings.bUseLowPassFilter = false;
        StabilitySettings.Add(EWeatherType::Cloudy, CloudySettings);

        // 雨天
        FWeatherStabilitySettings RainySettings;
        RainySettings.TemperatureSmoothing = 0.3f;
        RainySettings.RadiationSmoothing = 0.2f;
        RainySettings.MinRadiation = 0.001f;
        RainySettings.MaxRadiation = 10.0f;
        RainySettings.TemperatureBias = -5.0f;
        RainySettings.RadiationBias = -0.5f;
        RainySettings.bUseLowPassFilter = true;
        StabilitySettings.Add(EWeatherType::Rainy, RainySettings);

        // 雾天
        FWeatherStabilitySettings FoggySettings;
        FoggySettings.TemperatureSmoothing = 0.2f;
        FoggySettings.RadiationSmoothing = 0.1f;
        FoggySettings.MinRadiation = 0.0005f;
        FoggySettings.MaxRadiation = 5.0f;
        FoggySettings.TemperatureBias = -3.0f;
        FoggySettings.RadiationBias = -0.8f;
        FoggySettings.bUseLowPassFilter = true;
        StabilitySettings.Add(EWeatherType::Foggy, FoggySettings);

        // 雪天
        FWeatherStabilitySettings SnowySettings;
        SnowySettings.TemperatureSmoothing = 0.4f;
        SnowySettings.RadiationSmoothing = 0.3f;
        SnowySettings.MinRadiation = 0.002f;
        SnowySettings.MaxRadiation = 8.0f;
        SnowySettings.TemperatureBias = -10.0f;
        SnowySettings.RadiationBias = -0.3f;
        SnowySettings.bUseLowPassFilter = true;
        StabilitySettings.Add(EWeatherType::Snowy, SnowySettings);
    }

    // 清空历史数据并初始化
    TemperatureHistory.Empty();
    RadiationHistory.Empty();
    for (int32 i = 0; i < HISTORY_SIZE; i++)
    {
        TemperatureHistory.Add(300.0f);
        RadiationHistory.Add(0.0f);
    }

    bIsInitialized = true;
}

// ==================== 初始化计算 ====================

void UShipRadCalculation::Initialization()
{
    UpdateWeatherParameters();

    // 计算辐射分量
    Qsundir = CalculateQsundirWithAtmosphere(ShipRadConstants::SunH, ShipRadConstants::Day);
    Qsunsca = CalculateQsunscaWithAtmosphere(ShipRadConstants::SunH, ShipRadConstants::Beta);
    Qsunearth = CalculateQsunearthWithAtmosphere(Qsundir, ShipRadConstants::Beta);
    Qsky = CalculateQskyWithAtmosphere(CurrentTf, CurrentFsi);
    Qearth = CalculateQearthWithAtmosphere(CurrentTgrd, CurrentFsi);

    // 计算对流换热系数
    Conve1 = CalculateConvection1WithWind();
    Conve2 = CalculateConvection2WithWind();
}

// ==================== 温度计算 ====================

float UShipRadCalculation::GetTemperature(EShipType Part, int32 FireCount, float EnginePower, float DeltaTime)
{
    Initialization();

    if (!bIsInitialized) InitializeStabilitySettings();

    float RawTemp = 0.0f;
    switch (Part)
    {
    case EShipType::ShipBody:
        Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(AShipBody);
        TShipBody = CalculateTemperature(EShipType::ShipBody, TShipBody, 0.1f, 100, DeltaTime);
        RawTemp = TShipBody;
        break;

    case EShipType::ShipCannon:
        Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(AShipCannon);
        EfireVal = CalculateEfire(FireCount);
        TShipCannon_T = CalculateTemperature(EShipType::ShipCannon, TShipCannon_T, 0.1f, 100, DeltaTime);
        RawTemp = TShipCannon_T;
        break;

    case EShipType::AirDefenseGun:
        Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(AAirDefenseGun);
        EfireVal = CalculateEfire(FireCount);
        TAirDefenseGun = CalculateTemperature(EShipType::AirDefenseGun, TAirDefenseGun, 0.1f, 100, DeltaTime);
        RawTemp = TAirDefenseGun;
        break;

    case EShipType::ShipMissile:
        Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(AShipMissile);
        EfireVal = CalculateEfire(FireCount);
        TShipMissile = CalculateTemperature(EShipType::ShipMissile, TShipMissile, 0.1f, 100, DeltaTime);
        RawTemp = TShipMissile;
        break;

    case EShipType::Chimney:
        Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(AChimney);
        EengineVal = CalculateEengine(EnginePower / 50.0f);
        TChimney_T = CalculateTemperature(EShipType::Chimney, TChimney_T, 0.1f, 100, DeltaTime);
        RawTemp = TChimney_T;
        break;

    case EShipType::BoilerRoom:
        Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(ABoilerRoom);
        EengineVal = CalculateEengine(EnginePower / 50.0f);
        TBoilerRoom_T = CalculateTemperature(EShipType::BoilerRoom, TBoilerRoom_T, 0.1f, 100, DeltaTime);
        RawTemp = TBoilerRoom_T;
        break;

    default:
        RawTemp = 300.0f;
        break;
    }

    return StabilizeTemperature(RawTemp, DeltaTime);
}

// ==================== 热平衡方程 ====================

float UShipRadCalculation::Expression(EShipType Part, float T) const
{
    const float EnvTemp = CurrentTf;

    switch (Part)
    {
    case EShipType::ShipBody:
        return EShipBody * ShipRadConstants::Sigma * (FMath::Pow(T, 4) - FMath::Pow(EnvTemp, 4))
            + (0.7331f * (T - EnvTemp) + 1.9f * CurrentVwind + 1.8f) * (T - EnvTemp)
            + Einternal - Eabsorbenvir;

    case EShipType::ShipCannon:
        return EShipCannon_Val * ShipRadConstants::Sigma * (FMath::Pow(T, 4) - FMath::Pow(EnvTemp, 4))
            + Conve1 * (T - EnvTemp) + Einternal - Eabsorbenvir - EfireVal;

    case EShipType::AirDefenseGun:
        return EAirDefenseGun * ShipRadConstants::Sigma * (FMath::Pow(T, 4) - FMath::Pow(EnvTemp, 4))
            + Conve2 * (T - EnvTemp) + Einternal - Eabsorbenvir - EfireVal;

    case EShipType::ShipMissile:
        return EShipMissile * ShipRadConstants::Sigma * (FMath::Pow(T, 4) - FMath::Pow(EnvTemp, 4))
            + Conve2 * (T - EnvTemp) + Einternal - Eabsorbenvir - EfireVal;

    case EShipType::Chimney:
        return EChimney * ShipRadConstants::Sigma * (FMath::Pow(T, 4) - FMath::Pow(EnvTemp, 4))
            + (0.7331f * (T - EnvTemp) + 1.9f * CurrentVwind + 1.8f) * (T - EnvTemp)
            + Einternal - Eabsorbenvir - EengineVal;

    case EShipType::BoilerRoom:
        return EBoilerRoom * ShipRadConstants::Sigma * (FMath::Pow(T, 4) - FMath::Pow(EnvTemp, 4))
            + (0.7331f * (T - EnvTemp) + 1.9f * CurrentVwind + 1.8f) * (T - EnvTemp)
            + Einternal - Eabsorbenvir - EengineVal;

    default:
        return 0.0f;
    }
}

float UShipRadCalculation::DerExpression(EShipType Part, float T) const
{
    const float EnvTemp = CurrentTf;

    switch (Part)
    {
    case EShipType::ShipBody:
        return 4 * EShipBody * ShipRadConstants::Sigma * FMath::Pow(T, 3)
            + 2 * 0.7331f * (T - EnvTemp) + 1.9f * CurrentVwind + 1.8f;

    case EShipType::ShipCannon:
        return 4 * EShipCannon_Val * ShipRadConstants::Sigma * FMath::Pow(T, 3) + Conve1;

    case EShipType::AirDefenseGun:
        return 4 * EAirDefenseGun * ShipRadConstants::Sigma * FMath::Pow(T, 3) + Conve2;

    case EShipType::ShipMissile:
        return 4 * EShipMissile * ShipRadConstants::Sigma * FMath::Pow(T, 3) + Conve2;

    case EShipType::Chimney:
        return 4 * EChimney * ShipRadConstants::Sigma * FMath::Pow(T, 3)
            + 2 * 0.7331f * (T - EnvTemp) + 1.9f * CurrentVwind + 1.8f;

    case EShipType::BoilerRoom:
        return 4 * EBoilerRoom * ShipRadConstants::Sigma * FMath::Pow(T, 3)
            + 2 * 0.7331f * (T - EnvTemp) + 1.9f * CurrentVwind + 1.8f;

    default:
        return 0.0f;
    }
}

float UShipRadCalculation::CalculateTemperature(EShipType Part, float T, float Precision, int32 MaxCyc, float DeltaTime)
{
    float T1 = T;
    const float T0 = T;

    const float DampingFactor = 0.1f;

    const float Expr = Expression(Part, T0);
    const float Dexpr = DerExpression(Part, T0);

    if (FMath::Abs(Dexpr) > SMALL_NUMBER)
    {
        T1 = T0 - (Expr / Dexpr) * DampingFactor;
    }

    // 限制每秒最大温度变化率
    const float MaxRateOfChange = 5.0f;
    const float MaxDeltaT = MaxRateOfChange * DeltaTime;

    T1 = FMath::Clamp(T1, T0 - MaxDeltaT, T0 + MaxDeltaT);

    return T1;
}

// ==================== 稳定性处理 ====================

float UShipRadCalculation::StabilizeTemperature(float RawTemperature, float DeltaTime)
{
    const FWeatherStabilitySettings* Settings = StabilitySettings.Find(CurrentWeather);
    if (!Settings) return RawTemperature;

    // 添加偏差
    float BiasedTemp = RawTemperature + Settings->TemperatureBias;

    // 更新历史 (FIFO)
    if (TemperatureHistory.Num() >= HISTORY_SIZE)
    {
        TemperatureHistory.RemoveAt(0);
    }
    TemperatureHistory.Add(BiasedTemp);

    // 移动平均
    float Sum = 0.0f;
    for (float Temp : TemperatureHistory)
    {
        Sum += Temp;
    }
    float AverageTemp = Sum / TemperatureHistory.Num();

    // 平滑
    SmoothedTemperature = FMath::Lerp(SmoothedTemperature, AverageTemp, Settings->TemperatureSmoothing * DeltaTime * SMOOTHING_FRAMERATE_FACTOR);

    // 钳制范围
    SmoothedTemperature = FMath::Clamp(SmoothedTemperature, 250.0f, 400.0f);

    return SmoothedTemperature;
}

float UShipRadCalculation::StabilizeRadiation(float RawRadiation, float DeltaTime)
{
    const FWeatherStabilitySettings* Settings = StabilitySettings.Find(CurrentWeather);
    if (!Settings) return RawRadiation;

    float BiasedRadiation = FMath::Max(0.0f, RawRadiation + Settings->RadiationBias);

    // 更新历史
    if (RadiationHistory.Num() >= HISTORY_SIZE)
    {
        RadiationHistory.RemoveAt(0);
    }
    RadiationHistory.Add(BiasedRadiation);

    // 移动平均
    float Sum = 0.0f;
    for (float Rad : RadiationHistory)
    {
        Sum += Rad;
    }
    float AverageRadiation = Sum / RadiationHistory.Num();

    // 低通滤波器
    if (Settings->bUseLowPassFilter)
    {
        AverageRadiation = AverageRadiation * 0.8f + LastCalculatedRadiation * 0.2f;
    }

    // 平滑
    SmoothedRadiation = FMath::Lerp(SmoothedRadiation, AverageRadiation, Settings->RadiationSmoothing * DeltaTime * SMOOTHING_FRAMERATE_FACTOR);

    // 范围限制
    SmoothedRadiation = FMath::Clamp(SmoothedRadiation, Settings->MinRadiation, Settings->MaxRadiation);

    // 防止剧烈变化
    float MaxChange = Settings->MaxRadiation * 0.1f;
    SmoothedRadiation = FMath::Clamp(SmoothedRadiation,
        LastCalculatedRadiation - MaxChange,
        LastCalculatedRadiation + MaxChange);

    LastCalculatedRadiation = SmoothedRadiation;

    return SmoothedRadiation;
}

// ==================== 零视距红外辐射 ====================

float UShipRadCalculation::CalculateLdetzeroview(EShipType Part, float DeltaTime)
{
    float T = 0.0f, E = 0.0f;
    const float* R = nullptr;

    switch (Part)
    {
    case EShipType::ShipBody:       T = TShipBody;       E = EShipBody;       R = RShipBody;       break;
    case EShipType::ShipCannon:     T = TShipCannon_T;   E = EShipCannon_Val; R = RShipCannon;     break;
    case EShipType::AirDefenseGun:  T = TAirDefenseGun;  E = EAirDefenseGun;  R = RAirDefenseGun;  break;
    case EShipType::ShipMissile:    T = TShipMissile;    E = EShipMissile;    R = RShipMissile;    break;
    case EShipType::Chimney:        T = TChimney_T;      E = EChimney;        R = RChimney;        break;
    case EShipType::BoilerRoom:     T = TBoilerRoom_T;   E = EBoilerRoom;     R = RBoilerRoom;     break;
    default: return 0.0f;
    }

    // 自身辐射
    float Eselfrad = FMath::Abs(Calculateapprox(1.0f / ShipRadConstants::Wave1, T, E)
                              - Calculateapprox(1.0f / ShipRadConstants::Wave2, T, E));
    Eselfrad *= CurrentIR_Tau;

    // 环境反射
    float Ereflectenvir = R[0] * (Qsundir + Qsunsca + Qsunearth) + R[1] * (Qsky + Qearth);

    // 天气衰减
    float WeatherFactor = 1.0f - GetWeatherAttenuationFactor();
    Ereflectenvir *= WeatherFactor;

    float RawRadiation = (Eselfrad + Ereflectenvir)
        * FMath::Cos(ShipRadConstants::ATR * 30.0f)
        * FMath::Cos(ShipRadConstants::ATR * 60.0f);

    return StabilizeRadiation(RawRadiation, DeltaTime);
}

// ==================== 环境辐射分量计算 ====================

float UShipRadCalculation::CalculateEabsorbenvirWithAtmosphere(const float* Absorb) const
{
    return Absorb[0] * (Qsundir + Qsunsca + Qsunearth)
         + Absorb[1] * Qsky
         + Absorb[2] * Qearth;
}

float UShipRadCalculation::CalculateQsundirWithAtmosphere(float SunH, float Day) const
{
    float R = 1.0f + 0.034f * FMath::Cos(2.0f * PI * Day / 365.0f);
    float M = 1.0f / FMath::Sin(SunH * ShipRadConstants::ATR);
    float AtmosphericAttenuation = FMath::Pow(CurrentPm, M);
    float WaterVaporAbsorption = FMath::Exp(-0.05f * CurrentRH * M);
    float AerosolAttenuation = FMath::Exp(-CurrentAOD * M);
    float PrecipitationAtten = CalculatePrecipitationAttenuation(1.0f, CurrentRainRate);

    return R * ShipRadConstants::Isc * AtmosphericAttenuation
         * WaterVaporAbsorption * AerosolAttenuation * PrecipitationAtten;
}

float UShipRadCalculation::CalculateQsunscaWithAtmosphere(float SunH, float InBeta) const
{
    float BaseScattering = ShipRadConstants::Sca1 * FMath::Pow(FMath::Sin(SunH * ShipRadConstants::ATR), ShipRadConstants::Sca2);
    float ScatterCoeff = GetScatteringCoefficient();
    float AngleFactor = (FMath::Cos(InBeta * ShipRadConstants::ATR) + 1.0f) / 2.0f;
    float WeatherScatter = GetWeatherScatteringFactor();

    return BaseScattering * ScatterCoeff * AngleFactor * WeatherScatter;
}

float UShipRadCalculation::CalculateQsunearthWithAtmosphere(float InQsundir, float InBeta) const
{
    float DirectPart = InQsundir * FMath::Sin(ShipRadConstants::SunH * ShipRadConstants::ATR);
    float ScatteringPart = ShipRadConstants::Sca1 * FMath::Pow(FMath::Sin(ShipRadConstants::SunH * ShipRadConstants::ATR), ShipRadConstants::Sca2);
    float GroundReflectivity = CurrentRhod;
    float AngleFactor = (FMath::Cos(ShipRadConstants::Beta * ShipRadConstants::ATR) + 1.0f) / 2.0f;

    return (DirectPart + ScatteringPart) * GroundReflectivity * AngleFactor;
}

float UShipRadCalculation::CalculateQskyWithAtmosphere(float Tf, float Fsi) const
{
    float Ea = (2.0f / 15.0f) * FMath::Exp(18.5916f - 3991.11f / (Tf - 273.15f + 233.84f)) * CurrentRH;
    float BaseSkyRadiation = ShipRadConstants::Sigma * FMath::Pow(Tf, 4.0f)
        * (ShipRadConstants::Aexp + ShipRadConstants::Bexp * FMath::Sqrt(Ea)) * Fsi;
    float WeatherCorrection = GetSkyRadiationCorrection();

    return BaseSkyRadiation * WeatherCorrection;
}

float UShipRadCalculation::CalculateQearthWithAtmosphere(float Tgrd, float Fsi) const
{
    return CurrentEpslgrd * ShipRadConstants::Sigma * FMath::Pow(Tgrd, 4.0f) * Fsi;
}

// ==================== 对流换热系数 ====================

float UShipRadCalculation::CalculateConvection1WithWind() const
{
    float Re_wind = ShipRadConstants::Re * (CurrentVwind / ShipRadConstants::Sunny_Vwind);
    return 2.0f + ShipRadConstants::Airc / ShipRadConstants::Dbar
        * (0.4f * FMath::Sqrt(Re_wind) + 0.06f * FMath::Pow(Re_wind, 2.0f / 3.0f))
        * FMath::Pow(ShipRadConstants::Pr, 0.4f)
        * FMath::Pow(ShipRadConstants::Uinf / ShipRadConstants::Us, 0.25f);
}

float UShipRadCalculation::CalculateConvection2WithWind() const
{
    float Re_wind = ShipRadConstants::Re * (CurrentVwind / ShipRadConstants::Sunny_Vwind);
    return ShipRadConstants::Airc / ShipRadConstants::Dcan * 0.332f
        * FMath::Sqrt(Re_wind) * FMath::Pow(ShipRadConstants::Pr, 1.0f / 3.0f);
}

// ==================== 热源计算 ====================

float UShipRadCalculation::CalculateEengine(float Power) const
{
    return (1.0f - 0.4f) * Power * 0.3f * 427000.0f / 3600.0f;
}

float UShipRadCalculation::CalculateEfire(int32 FireCount) const
{
    return 0.6f * FireCount * 0.3f * 0.5f / (1.0f * (0.4f * PI)) * 0.5f * 2.0f * FMath::Pow(20.0f, 2);
}

// ==================== 辐射计算 ====================

float UShipRadCalculation::Calculateapprox(float Wave, float T, float Emm) const
{
    return Emm / PI * ShipRadConstants::Radc1 * T / ShipRadConstants::Radc2
        * FMath::Exp(-ShipRadConstants::Radc2 / (Wave * T))
        * (FMath::Pow(Wave, 3) + 3.0f * T / ShipRadConstants::Radc2
        * (FMath::Pow(Wave, 2) + 2.0f * T / ShipRadConstants::Radc2
        * (Wave + T / ShipRadConstants::Radc2)));
}

float UShipRadCalculation::CalculateAtmosphericTransmittance(float Distance, float Wavelength) const
{
    float ExtinctionCoefficient = 3.912f / (CurrentVisibility / 1000.0f);
    float AngstromExp = GetAngstromExponent();
    float WavelengthDependence = FMath::Pow(0.55f / Wavelength, AngstromExp);
    float TotalExtinction = ExtinctionCoefficient * WavelengthDependence;

    return FMath::Exp(-TotalExtinction * Distance);
}

float UShipRadCalculation::CalculatePrecipitationAttenuation(float Distance, float PrecipitationRate) const
{
    float AttenuationCoefficient = 0.0f;

    switch (CurrentWeather)
    {
    case EWeatherType::Rainy:
        AttenuationCoefficient = ShipRadConstants::Rain_Attenuation * PrecipitationRate;
        break;
    case EWeatherType::Snowy:
        AttenuationCoefficient = ShipRadConstants::Snow_Attenuation * PrecipitationRate;
        break;
    case EWeatherType::Foggy:
        AttenuationCoefficient = ShipRadConstants::Fog_Attenuation;
        break;
    default:
        return 1.0f;
    }

    return FMath::Pow(10.0f, -AttenuationCoefficient * Distance / 10.0f);
}

// ==================== 辅助查询 ====================

float UShipRadCalculation::GetScatteringCoefficient() const
{
    switch (CurrentScattering)
    {
    case EScatteringType::Rayleigh: return ShipRadConstants::Rayleigh_Beta;
    case EScatteringType::Mie:      return ShipRadConstants::Mie_Beta;
    case EScatteringType::Cloud:    return ShipRadConstants::Cloud_Beta;
    case EScatteringType::Rain:     return ShipRadConstants::Rain_Beta;
    case EScatteringType::Fog:      return ShipRadConstants::Fog_Beta;
    case EScatteringType::Snow:     return ShipRadConstants::Snow_Beta;
    default: return 1.0f;
    }
}

float UShipRadCalculation::GetWeatherScatteringFactor() const
{
    switch (CurrentWeather)
    {
    case EWeatherType::Sunny:  return 1.0f;
    case EWeatherType::Cloudy: return 2.0f;
    case EWeatherType::Rainy:  return 1.8f;
    case EWeatherType::Foggy:  return 3.0f;
    case EWeatherType::Snowy:  return 2.5f;
    default: return 1.5f;
    }
}

float UShipRadCalculation::GetSkyRadiationCorrection() const
{
    switch (CurrentWeather)
    {
    case EWeatherType::Sunny:  return 1.0f;
    case EWeatherType::Cloudy: return 1.3f;
    case EWeatherType::Rainy:  return 1.4f;
    case EWeatherType::Foggy:  return 1.2f;
    case EWeatherType::Snowy:  return 1.1f;
    default: return 1.0f;
    }
}

float UShipRadCalculation::GetWeatherAttenuationFactor() const
{
    switch (CurrentWeather)
    {
    case EWeatherType::Sunny:  return 0.1f;
    case EWeatherType::Cloudy: return 0.5f;
    case EWeatherType::Rainy:  return 0.6f;
    case EWeatherType::Foggy:  return 0.8f;
    case EWeatherType::Snowy:  return 0.7f;
    default: return 0.3f;
    }
}

float UShipRadCalculation::GetAngstromExponent() const
{
    switch (CurrentWeather)
    {
    case EWeatherType::Sunny:  return ShipRadConstants::Sunny_Angstrom;
    case EWeatherType::Cloudy: return ShipRadConstants::Cloudy_Angstrom;
    case EWeatherType::Rainy:  return ShipRadConstants::Rainy_Angstrom;
    case EWeatherType::Foggy:  return ShipRadConstants::Foggy_Angstrom;
    case EWeatherType::Snowy:  return ShipRadConstants::Snowy_Angstrom;
    default: return 1.0f;
    }
}

// ==================== 描述信息 ====================

FString UShipRadCalculation::GetWeatherDescription() const
{
    switch (CurrentWeather)
    {
    case EWeatherType::Sunny:  return TEXT("晴天 - 高能见度，强太阳辐射");
    case EWeatherType::Cloudy: return TEXT("多云 - 中等能见度，散射增强");
    case EWeatherType::Rainy:  return TEXT("雨天 - 能见度降低，红外衰减强");
    case EWeatherType::Foggy:  return TEXT("雾天 - 能见度极低，红外探测困难");
    case EWeatherType::Snowy:  return TEXT("雪天 - 能见度低，地面反射率高");
    default: return TEXT("未知天气");
    }
}

FString UShipRadCalculation::GetWeatherImpactSummary() const
{
    float VisibilityKm = CurrentVisibility / 1000.0f;
    float TransmittancePercent = CurrentIR_Tau * 100.0f;

    return FString::Printf(TEXT("当前天气: %s\n能见度: %.1f km\n红外透过率: %.1f%%\n大气温度: %.1f°C\n相对湿度: %.0f%%\n风速: %.1f m/s"),
        *GetWeatherDescription(),
        VisibilityKm,
        TransmittancePercent,
        CurrentTf - 273.15f,
        CurrentRH * 100.0f,
        CurrentVwind);
}
