using UnityEngine;
using System.Collections.Generic;

public class ShipRadCalculation
{
    // 天气类型
    private Constants.WeatherType currentWeather = Constants.WeatherType.Snowy;

    // 大气传输参数
    private float currentPm;          // 当前天气的大气透明度
    private float currentTau;         // 当前天气的大气透过率
    private float currentAOD;         // 当前天气的气溶胶光学厚度
    private float currentIR_Tau;      // 当前天气的红外透过率
    private float currentVisibility;  // 当前能见度
    private float currentTf;          // 当前环境大气温度
    private float currentTgrd;        // 当前地面温度
    private float currentFsi;         // 当前辐射传递系数
    private float currentVwind;       // 当前风速
    private float currentRH;          // 当前相对湿度
    private float currentPressure;    // 当前大气压力
    private float currentCloud;       // 当前云量因子
    private float currentRainRate;    // 当前降雨率
    private float currentRhod;        // 当前地面反射率
    private float currentEpslgrd;     // 当前地面发射率

    // 散射类型
    private Constants.ScatteringType currentScattering;

    // 船舶部件吸收率
    private static readonly float[] aShipBody = { 0.85f, 0.8f, 0.65f };
    private static readonly float[] aShipCannon = { 0.7f, 0.7f, 0.7f };
    private static readonly float[] aAirDefenseGun = { 0.7f, 0.7f, 0.7f };
    private static readonly float[] aShipMissile = { 0.85f, 0.8f, 0.65f };
    private static readonly float[] aChimney = { 0.9f, 0.8f, 0.75f };
    private static readonly float[] aBoilerRoom = { 0.9f, 0.8f, 0.75f };

    // 船舶部件发射率
    private const float eShipBody = 0.75f, eShipCannon = 0.85f, eAirDefenseGun = 0.85f,
                       eShipMissile = 0.75f, eChimney = 0.9f, eBoilerRoom = 0.9f;

    // 船舶部件反射率
    private static readonly float[] rShipBody = { 0.2f, 0.25f };
    private static readonly float[] rShipCannon = { 0.15f, 0.2f };
    private static readonly float[] rAirDefenseGun = { 0.15f, 0.2f };
    private static readonly float[] rShipMissile = { 0.2f, 0.25f };
    private static readonly float[] rChimney = { 0.1f, 0.2f };
    private static readonly float[] rBoilerRoom = { 0.1f, 0.2f };

    // 状态变量
    private float Qsundir, Qsunsca, Qsunearth, Qsky, Qearth;
    private float conve1, conve2;
    private float Eabsorbenvir, Efire, Eengine;
    private float Einternal = 450f; // 内部发热量

    // 船舶部件温度
    private float TShipBody = 300f, TShipCannon = 300f, TAirDefenseGun = 300f,
                  TShipMissile = 300f, TChimney = 300f, TBoilerRoom = 300f;

    // ==================== 新增：稳定性控制 ====================
    private Dictionary<Constants.WeatherType, WeatherStabilitySettings> stabilitySettings;
    private float lastCalculatedTemperature = 300f;
    private float lastCalculatedRadiation = 0f;
    private float smoothedTemperature = 300f;
    private float smoothedRadiation = 0f;
    private Queue<float> temperatureHistory = new Queue<float>();
    private Queue<float> radiationHistory = new Queue<float>();
    private const int HISTORY_SIZE = 5;
    private bool isInitialized = false;
    // ========================================================

    public float GetAmbientTemperature()
    {
        return currentTf;
    }

    // 获取当前能见度 (m)
    public float GetVisibility()
    {
        return currentVisibility;
    }

    // 获取当前天气类型
    public Constants.WeatherType GetWeatherType()
    {
        return currentWeather;
    }

    // 设置天气类型
    public void SetWeather(Constants.WeatherType weather)
    {
        currentWeather = weather;
        UpdateWeatherParameters();
        InitializeStabilitySettings(); // 初始化稳定性设置
    }

    // 初始化稳定性设置
    private void InitializeStabilitySettings()
    {
        if (stabilitySettings == null)
        {
            stabilitySettings = new Dictionary<Constants.WeatherType, WeatherStabilitySettings>
            {
                { Constants.WeatherType.Sunny, new WeatherStabilitySettings {
                    temperatureSmoothing = 0.8f,
                    radiationSmoothing = 0.7f,
                    minRadiation = 0.01f,
                    maxRadiation = 20f,
                    temperatureBias = 0f,
                    radiationBias = 0f,
                    useLowPassFilter = false
                }},
                { Constants.WeatherType.Cloudy, new WeatherStabilitySettings {
                    temperatureSmoothing = 0.6f,
                    radiationSmoothing = 0.5f,
                    minRadiation = 0.005f,
                    maxRadiation = 15f,
                    temperatureBias = -2f,
                    radiationBias = -0.2f,
                    useLowPassFilter = false
                }},
                { Constants.WeatherType.Rainy, new WeatherStabilitySettings {
                    temperatureSmoothing = 0.3f,  // 雨天更强的平滑
                    radiationSmoothing = 0.2f,
                    minRadiation = 0.001f,
                    maxRadiation = 10f,
                    temperatureBias = -5f,
                    radiationBias = -0.5f,
                    useLowPassFilter = true
                }},
                { Constants.WeatherType.Foggy, new WeatherStabilitySettings {
                    temperatureSmoothing = 0.2f,  // 雾天最强的平滑
                    radiationSmoothing = 0.1f,
                    minRadiation = 0.0005f,
                    maxRadiation = 5f,
                    temperatureBias = -3f,
                    radiationBias = -0.8f,
                    useLowPassFilter = true
                }},
                { Constants.WeatherType.Snowy, new WeatherStabilitySettings {
                    temperatureSmoothing = 0.4f,
                    radiationSmoothing = 0.3f,
                    minRadiation = 0.002f,
                    maxRadiation = 8f,
                    temperatureBias = -10f,
                    radiationBias = -0.3f,
                    useLowPassFilter = true
                }}
            };
        }

        // 清空历史数据
        temperatureHistory.Clear();
        radiationHistory.Clear();
        for (int i = 0; i < HISTORY_SIZE; i++)
        {
            temperatureHistory.Enqueue(300f);
            radiationHistory.Enqueue(0f);
        }

        isInitialized = true;
    }

    // 更新天气参数
    private void UpdateWeatherParameters()
    {
        switch (currentWeather)
        {
            case Constants.WeatherType.Sunny:
                SetSunnyParameters();
                break;

            case Constants.WeatherType.Cloudy:
                SetCloudyParameters();
                break;

            case Constants.WeatherType.Rainy:
                SetRainyParameters();
                break;

            case Constants.WeatherType.Foggy:
                SetFoggyParameters();
                break;

            case Constants.WeatherType.Snowy:
                SetSnowyParameters();
                break;
        }
    }

    private void SetSunnyParameters()
    {
        currentPm = Constants.Sunny_Pm;
        currentTau = Constants.Sunny_Tau;
        currentAOD = Constants.Sunny_AOD;
        currentIR_Tau = Constants.Sunny_IR_Tau;
        currentVisibility = Constants.Sunny_Visibility;
        currentTf = Constants.Sunny_Tf;
        currentTgrd = Constants.Sunny_Tgrd;
        currentFsi = Constants.Sunny_fsi;
        currentVwind = Constants.Sunny_Vwind;
        currentRH = Constants.Sunny_RH;
        currentPressure = Constants.Sunny_Pressure;
        currentCloud = Constants.Sunny_Cloud;
        currentRainRate = Constants.Sunny_RainRate;
        currentRhod = Constants.Sunny_rhod;
        currentEpslgrd = Constants.Sunny_epslgrd;
        currentScattering = Constants.ScatteringType.Mie; // 晴天主要为气溶胶散射
    }

    private void SetCloudyParameters()
    {
        currentPm = Constants.Cloudy_Pm;
        currentTau = Constants.Cloudy_Tau;
        currentAOD = Constants.Cloudy_AOD;
        currentIR_Tau = Constants.Cloudy_IR_Tau;
        currentVisibility = Constants.Cloudy_Visibility;
        currentTf = Constants.Cloudy_Tf;
        currentTgrd = Constants.Cloudy_Tgrd;
        currentFsi = Constants.Cloudy_fsi;
        currentVwind = Constants.Cloudy_Vwind;
        currentRH = Constants.Cloudy_RH;
        currentPressure = Constants.Cloudy_Pressure;
        currentCloud = Constants.Cloudy_Cloud;
        currentRainRate = Constants.Cloudy_RainRate;
        currentRhod = Constants.Cloudy_rhod;
        currentEpslgrd = Constants.Cloudy_epslgrd;
        currentScattering = Constants.ScatteringType.Cloud; // 多云主要为云散射
    }

    private void SetRainyParameters()
    {
        currentPm = Constants.Rainy_Pm;
        currentTau = Constants.Rainy_Tau;
        currentAOD = Constants.Rainy_AOD;
        currentIR_Tau = Constants.Rainy_IR_Tau;
        currentVisibility = Constants.Rainy_Visibility;
        currentTf = Constants.Rainy_Tf;
        currentTgrd = Constants.Rainy_Tgrd;
        currentFsi = Constants.Rainy_fsi;
        currentVwind = Constants.Rainy_Vwind;
        currentRH = Constants.Rainy_RH;
        currentPressure = Constants.Rainy_Pressure;
        currentCloud = Constants.Rainy_Cloud;
        currentRainRate = Constants.Rainy_RainRate;
        currentRhod = Constants.Rainy_rhod;
        currentEpslgrd = Constants.Rainy_epslgrd;
        currentScattering = Constants.ScatteringType.Rain; // 雨天为雨滴散射
    }

    private void SetFoggyParameters()
    {
        currentPm = Constants.Foggy_Pm;
        currentTau = Constants.Foggy_Tau;
        currentAOD = Constants.Foggy_AOD;
        currentIR_Tau = Constants.Foggy_IR_Tau;
        currentVisibility = Constants.Foggy_Visibility;
        currentTf = Constants.Foggy_Tf;
        currentTgrd = Constants.Foggy_Tgrd;
        currentFsi = Constants.Foggy_fsi;
        currentVwind = Constants.Foggy_Vwind;
        currentRH = Constants.Foggy_RH;
        currentPressure = Constants.Foggy_Pressure;
        currentCloud = Constants.Foggy_Cloud;
        currentRainRate = Constants.Foggy_RainRate;
        currentRhod = Constants.Foggy_rhod;
        currentEpslgrd = Constants.Foggy_epslgrd;
        currentScattering = Constants.ScatteringType.Fog; // 雾天为雾散射
    }

    private void SetSnowyParameters()
    {
        currentPm = Constants.Snowy_Pm;
        currentTau = Constants.Snowy_Tau;
        currentAOD = Constants.Snowy_AOD;
        currentIR_Tau = Constants.Snowy_IR_Tau;
        currentVisibility = Constants.Snowy_Visibility;
        currentTf = Constants.Snowy_Tf;
        currentTgrd = Constants.Snowy_Tgrd;
        currentFsi = Constants.Snowy_fsi;
        currentVwind = Constants.Snowy_Vwind;
        currentRH = Constants.Snowy_RH;
        currentPressure = Constants.Snowy_Pressure;
        currentCloud = Constants.Snowy_Cloud;
        currentRainRate = Constants.Snowy_RainRate;
        currentRhod = Constants.Snowy_rhod;
        currentEpslgrd = Constants.Snowy_epslgrd;
        currentScattering = Constants.ScatteringType.Snow; // 雪天为雪花散射
    }

    // 初始化
    private void Initialization()
    {
        UpdateWeatherParameters();

        // 计算考虑大气传输的辐射分量
        Qsundir = CalculateQsundirWithAtmosphere(Constants.sunh, Constants.day);
        Qsunsca = CalculateQsunscaWithAtmosphere(Constants.sunh, Constants.beta);
        Qsunearth = CalculateQsunearthWithAtmosphere(Qsundir, Constants.beta);
        Qsky = CalculateQskyWithAtmosphere(currentTf, currentFsi);
        Qearth = CalculateQearthWithAtmosphere(currentTgrd, currentFsi);

        // 计算对流换热系数
        conve1 = CalculateConvection1WithWind();
        conve2 = CalculateConvection2WithWind();
    }

    // 获取温度（添加稳定性处理）
    public float GetTemperature(ShipType part, int fireCount, float enginePower)
    {
        Initialization();

        if (!isInitialized) InitializeStabilitySettings();

        float rawTemp = 0f;
        switch (part)
        {
            case ShipType.ShipBody:
                Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(aShipBody);
                TShipBody = CalculateTemperature(ShipType.ShipBody, TShipBody, 0.1f, 100);
                rawTemp = TShipBody;
                break;
            case ShipType.ShipCannon:
                Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(aShipCannon);
                Efire = CalculateEfire(fireCount);
                TShipCannon = CalculateTemperature(ShipType.ShipCannon, TShipCannon, 0.1f, 100);
                rawTemp = TShipCannon;
                break;
            case ShipType.AirDefenseGun:
                Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(aAirDefenseGun);
                Efire = CalculateEfire(fireCount);
                TAirDefenseGun = CalculateTemperature(ShipType.AirDefenseGun, TAirDefenseGun, 0.1f, 100);
                rawTemp = TAirDefenseGun;
                break;
            case ShipType.ShipMissile:
                Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(aShipMissile);
                Efire = CalculateEfire(fireCount);
                TShipMissile = CalculateTemperature(ShipType.ShipMissile, TShipMissile, 0.1f, 100);
                rawTemp = TShipMissile;
                break;
            case ShipType.Chimney:
                Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(aChimney);
                Eengine = CalculateEengine(enginePower / 50f);
                TChimney = CalculateTemperature(ShipType.Chimney, TChimney, 0.1f, 100);
                rawTemp = TChimney;
                break;
            case ShipType.BoilerRoom:
                Eabsorbenvir = CalculateEabsorbenvirWithAtmosphere(aBoilerRoom);
                Eengine = CalculateEengine(enginePower / 50f);
                TBoilerRoom = CalculateTemperature(ShipType.BoilerRoom, TBoilerRoom, 0.1f, 100);
                rawTemp = TBoilerRoom;
                break;
            default:
                rawTemp = 300f;
                break;
        }

        // 应用稳定性处理
        return StabilizeTemperature(rawTemp);
    }

    // 稳定性处理：温度
    private float StabilizeTemperature(float rawTemperature)
    {
        if (!stabilitySettings.ContainsKey(currentWeather))
            return rawTemperature;

        var settings = stabilitySettings[currentWeather];

        // 添加偏差
        float biasedTemp = rawTemperature + settings.temperatureBias;

        // 记录历史
        temperatureHistory.Dequeue();
        temperatureHistory.Enqueue(biasedTemp);

        // 计算移动平均值
        float sum = 0f;
        foreach (var temp in temperatureHistory)
        {
            sum += temp;
        }
        float averageTemp = sum / HISTORY_SIZE;

        // 应用平滑
        smoothedTemperature = Mathf.Lerp(smoothedTemperature, averageTemp, settings.temperatureSmoothing * Time.deltaTime * 30f);

        // 限制温度范围
        smoothedTemperature = Mathf.Clamp(smoothedTemperature, 250f, 400f);

        return smoothedTemperature;
    }

    // 热平衡方程
    private float Expression(ShipType part, float T)
    {
        float envTemp = currentTf;

        switch (part)
        {
            case ShipType.ShipBody:
                return eShipBody * Constants.sigma * (Mathf.Pow(T, 4) - Mathf.Pow(envTemp, 4))
                    + (0.7331f * (T - envTemp) + 1.9f * currentVwind + 1.8f) * (T - envTemp)
                    + Einternal - Eabsorbenvir;
            case ShipType.ShipCannon:
                return eShipCannon * Constants.sigma * (Mathf.Pow(T, 4) - Mathf.Pow(envTemp, 4))
                    + conve1 * (T - envTemp) + Einternal - Eabsorbenvir - Efire;
            case ShipType.AirDefenseGun:
                return eAirDefenseGun * Constants.sigma * (Mathf.Pow(T, 4) - Mathf.Pow(envTemp, 4))
                    + conve2 * (T - envTemp) + Einternal - Eabsorbenvir - Efire;
            case ShipType.ShipMissile:
                return eShipMissile * Constants.sigma * (Mathf.Pow(T, 4) - Mathf.Pow(envTemp, 4))
                    + conve2 * (T - envTemp) + Einternal - Eabsorbenvir - Efire;
            case ShipType.Chimney:
                return eChimney * Constants.sigma * (Mathf.Pow(T, 4) - Mathf.Pow(envTemp, 4))
                    + (0.7331f * (T - envTemp) + 1.9f * currentVwind + 1.8f) * (T - envTemp)
                    + Einternal - Eabsorbenvir - Eengine;
            case ShipType.BoilerRoom:
                return eBoilerRoom * Constants.sigma * (Mathf.Pow(T, 4) - Mathf.Pow(envTemp, 4))
                    + (0.7331f * (T - envTemp) + 1.9f * currentVwind + 1.8f) * (T - envTemp)
                    + Einternal - Eabsorbenvir - Eengine;
            default:
                return 0f;
        }
    }

    // 热平衡方程导数
    private float DerExpression(ShipType part, float T)
    {
        float envTemp = currentTf;

        switch (part)
        {
            case ShipType.ShipBody:
                return 4 * eShipBody * Constants.sigma * Mathf.Pow(T, 3)
                    + 2 * 0.7331f * (T - envTemp) + 1.9f * currentVwind + 1.8f;
            case ShipType.ShipCannon:
                return 4 * eShipCannon * Constants.sigma * Mathf.Pow(T, 3) + conve1;
            case ShipType.AirDefenseGun:
                return 4 * eAirDefenseGun * Constants.sigma * Mathf.Pow(T, 3) + conve2;
            case ShipType.ShipMissile:
                return 4 * eShipMissile * Constants.sigma * Mathf.Pow(T, 3) + conve2;
            case ShipType.Chimney:
                return 4 * eChimney * Constants.sigma * Mathf.Pow(T, 3)
                    + 2 * 0.7331f * (T - envTemp) + 1.9f * currentVwind + 1.8f;
            case ShipType.BoilerRoom:
                return 4 * eBoilerRoom * Constants.sigma * Mathf.Pow(T, 3)
                    + 2 * 0.7331f * (T - envTemp) + 1.9f * currentVwind + 1.8f;
            default:
                return 0f;
        }
    }

    // 牛顿迭代法求解温度
    // ShipRadCalculation.cs (在 CalculateTemperature 方法内部)

    private float CalculateTemperature(ShipType part, float T, float precision, int maxcyc)
    {
        float T1 = T, T0 = T;

        // 引入阻尼因子 (防止一步跳到稳态解)
        float dampingFactor = 0.1f;

        float expr = Expression(part, T0);
        float dexpr = DerExpression(part, T0);

        // 使用阻尼步
        T1 = T0 - (expr / dexpr) * dampingFactor;

        // === 核心修正：限制每秒的最大温度变化率 ===

        // 对于船体这样热容大的物体，每秒变化 0.5K 是一个比较合理的物理上限。
        float maxRateOfChange = 5f; // 每秒最大变化 0.5 K/s

        // 结合 deltaTime 计算每帧允许的最大变化量
        float maxDeltaT = maxRateOfChange * Time.deltaTime;

        // 将计算出的新温度 T1 钳制在 T0 正负 maxDeltaT 范围内
        T1 = Mathf.Clamp(T1, T0 - maxDeltaT, T0 + maxDeltaT);

        return T1;
    }
    // 零视距红外辐射（添加稳定性处理）
    public float CalculateLdetzeroview(ShipType part)
    {
        float T = 0, e = 0;
        float[] r = null;
        switch (part)
        {
            case ShipType.ShipBody: T = TShipBody; e = eShipBody; r = rShipBody; break;
            case ShipType.ShipCannon: T = TShipCannon; e = eShipCannon; r = rShipCannon; break;
            case ShipType.AirDefenseGun: T = TAirDefenseGun; e = eAirDefenseGun; r = rAirDefenseGun; break;
            case ShipType.ShipMissile: T = TShipMissile; e = eShipMissile; r = rShipMissile; break;
            case ShipType.Chimney: T = TChimney; e = eChimney; r = rChimney; break;
            case ShipType.BoilerRoom: T = TBoilerRoom; e = eBoilerRoom; r = rBoilerRoom; break;
            default: return 0f;
        }

        // 计算目标自身辐射
        float Eselfrad = Mathf.Abs(Calculateapprox(1 / Constants.wave1, T, e) -
                                   Calculateapprox(1 / Constants.wave2, T, e));
        Eselfrad *= currentIR_Tau; // 乘以红外波段大气透过率

        // 计算环境反射辐射
        float Ereflectenvir = r[0] * (Qsundir + Qsunsca + Qsunearth) + r[1] * (Qsky + Qearth);

        // 考虑云量和雾的影响
        float weatherFactor = 1.0f - GetWeatherAttenuationFactor();
        Ereflectenvir *= weatherFactor;

        float rawRadiation = (Eselfrad + Ereflectenvir) * Mathf.Cos(Constants.ATR * 30f) * Mathf.Cos(Constants.ATR * 60f);

        // 应用稳定性处理
        return StabilizeRadiation(rawRadiation);
    }

    // 稳定性处理：辐射
    private float StabilizeRadiation(float rawRadiation)
    {
        if (!stabilitySettings.ContainsKey(currentWeather))
            return rawRadiation;

        var settings = stabilitySettings[currentWeather];

        // 添加偏差
        float biasedRadiation = Mathf.Max(0f, rawRadiation + settings.radiationBias);

        // 记录历史
        radiationHistory.Dequeue();
        radiationHistory.Enqueue(biasedRadiation);

        // 计算移动平均值
        float sum = 0f;
        foreach (var rad in radiationHistory)
        {
            sum += rad;
        }
        float averageRadiation = sum / HISTORY_SIZE;

        // 低通滤波器（用于恶劣天气）
        if (settings.useLowPassFilter)
        {
            // 更严格的平滑
            averageRadiation = averageRadiation * 0.8f + lastCalculatedRadiation * 0.2f;
        }

        // 应用平滑
        smoothedRadiation = Mathf.Lerp(smoothedRadiation, averageRadiation, settings.radiationSmoothing * Time.deltaTime * 30f);

        // 限制范围
        smoothedRadiation = Mathf.Clamp(smoothedRadiation, settings.minRadiation, settings.maxRadiation);

        // 防止剧烈变化
        float maxChange = settings.maxRadiation * 0.1f;
        smoothedRadiation = Mathf.Clamp(smoothedRadiation,
            lastCalculatedRadiation - maxChange,
            lastCalculatedRadiation + maxChange);

        // 更新最后计算值
        lastCalculatedRadiation = smoothedRadiation;

        return smoothedRadiation;
    }

    // 获取天气衰减因子
    private float GetWeatherAttenuationFactor()
    {
        switch (currentWeather)
        {
            case Constants.WeatherType.Sunny:
                return 0.1f;
            case Constants.WeatherType.Cloudy:
                return 0.5f;
            case Constants.WeatherType.Rainy:
                return 0.6f;
            case Constants.WeatherType.Foggy:
                return 0.8f; // 雾天衰减最强
            case Constants.WeatherType.Snowy:
                return 0.7f;
            default:
                return 0.3f;
        }
    }

    // 计算吸收环境辐射
    private float CalculateEabsorbenvirWithAtmosphere(float[] absorb)
    {
        return absorb[0] * (Qsundir + Qsunsca + Qsunearth) +
               absorb[1] * Qsky +
               absorb[2] * Qearth;
    }

    // 计算太阳直接辐射
    private float CalculateQsundirWithAtmosphere(float sunh, float day)
    {
        // 地球轨道修正
        float r = 1 + 0.034f * Mathf.Cos(2 * Constants.PI * day / 365f);

        // 大气质量
        float m = 1f / Mathf.Sin(sunh * Constants.ATR);

        // 大气衰减
        float atmosphericAttenuation = Mathf.Pow(currentPm, m);

        // 水汽吸收
        float waterVaporAbsorption = Mathf.Exp(-0.05f * currentRH * m);

        // 气溶胶衰减
        float aerosolAttenuation = Mathf.Exp(-currentAOD * m);

        // 雨雪衰减
        float precipitationAttenuation = CalculatePrecipitationAttenuation(1.0f, currentRainRate);

        // 总太阳直接辐射
        return r * Constants.Isc * atmosphericAttenuation * waterVaporAbsorption *
               aerosolAttenuation * precipitationAttenuation;
    }

    // 计算太阳散射辐射
    private float CalculateQsunscaWithAtmosphere(float sunh, float beta)
    {
        // 基础散射
        float baseScattering = Constants.sca1 * Mathf.Pow(Mathf.Sin(sunh * Constants.ATR), Constants.sca2);

        // 散射系数
        float scatteringCoefficient = GetScatteringCoefficient();

        // 角度修正
        float angleFactor = ((Mathf.Cos(beta * Constants.ATR) + 1f) / 2f);

        // 考虑云量和天气影响
        float weatherScatteringFactor = GetWeatherScatteringFactor();

        return baseScattering * scatteringCoefficient * angleFactor * weatherScatteringFactor;
    }

    // 获取散射系数
    private float GetScatteringCoefficient()
    {
        switch (currentScattering)
        {
            case Constants.ScatteringType.Rayleigh:
                return Constants.Rayleigh_Beta;
            case Constants.ScatteringType.Mie:
                return Constants.Mie_Beta;
            case Constants.ScatteringType.Cloud:
                return Constants.Cloud_Beta;
            case Constants.ScatteringType.Rain:
                return Constants.Rain_Beta;
            case Constants.ScatteringType.Fog:
                return Constants.Fog_Beta;
            case Constants.ScatteringType.Snow:
                return Constants.Snow_Beta;
            default:
                return 1.0f;
        }
    }

    // 获取天气散射因子
    private float GetWeatherScatteringFactor()
    {
        switch (currentWeather)
        {
            case Constants.WeatherType.Sunny:
                return 1.0f;
            case Constants.WeatherType.Cloudy:
                return 2.0f; // 多云天散射增强
            case Constants.WeatherType.Rainy:
                return 1.8f;
            case Constants.WeatherType.Foggy:
                return 3.0f; // 雾天散射最强
            case Constants.WeatherType.Snowy:
                return 2.5f;
            default:
                return 1.5f;
        }
    }

    // 计算地面反射太阳辐射
    private float CalculateQsunearthWithAtmosphere(float Qsundir, float beta)
    {
        // 直接辐射部分
        float directPart = Qsundir * Mathf.Sin(Constants.sunh * Constants.ATR);

        // 散射部分
        float scatteringPart = Constants.sca1 * Mathf.Pow(Mathf.Sin(Constants.sunh * Constants.ATR), Constants.sca2);

        // 使用当前天气的地面反射率
        float groundReflectivity = currentRhod;

        // 角度修正
        float angleFactor = ((Mathf.Cos(Constants.beta * Constants.ATR) + 1f) / 2f);

        return (directPart + scatteringPart) * groundReflectivity * angleFactor;
    }

    // 计算大气长波辐射
    private float CalculateQskyWithAtmosphere(float Tf, float fsi)
    {
        // 水汽压计算
        float Ea = (2f / 15f) * Mathf.Exp(18.5916f - 3991.11f / (Tf - 273.15f + 233.84f)) * currentRH;

        // 基础天空辐射
        float baseSkyRadiation = Constants.sigma * Mathf.Pow(Tf, 4f) *
                                (Constants.aexp + Constants.bexp * Mathf.Sqrt(Ea)) * fsi;

        // 天气修正因子
        float weatherCorrection = GetSkyRadiationCorrection();

        return baseSkyRadiation * weatherCorrection;
    }

    // 获取天空辐射修正因子
    private float GetSkyRadiationCorrection()
    {
        switch (currentWeather)
        {
            case Constants.WeatherType.Sunny:
                return 1.0f;
            case Constants.WeatherType.Cloudy:
                return 1.3f; // 多云增强向下长波辐射
            case Constants.WeatherType.Rainy:
                return 1.4f;
            case Constants.WeatherType.Foggy:
                return 1.2f;
            case Constants.WeatherType.Snowy:
                return 1.1f;
            default:
                return 1.0f;
        }
    }

    // 计算地面辐射
    private float CalculateQearthWithAtmosphere(float Tgrd, float fsi)
    {
        // 使用当前天气的地面发射率
        float groundEmissivity = currentEpslgrd;

        return groundEmissivity * Constants.sigma * Mathf.Pow(Tgrd, 4f) * fsi;
    }

    // 平板对流系数
    private float CalculateConvection1WithWind()
    {
        // 使用当前天气的风速
        float Re_wind = Constants.Re * (currentVwind / Constants.Sunny_Vwind);

        return 2 + Constants.airc / Constants.dbar *
               (0.4f * Mathf.Sqrt(Re_wind) + 0.06f * Mathf.Pow(Re_wind, 2f / 3f)) *
               Mathf.Pow(Constants.Pr, 0.4f) * Mathf.Pow(Constants.uinf / Constants.us, 0.25f);
    }

    // 管道对流系数
    private float CalculateConvection2WithWind()
    {
        float Re_wind = Constants.Re * (currentVwind / Constants.Sunny_Vwind);

        return Constants.airc / Constants.dcan * 0.332f *
               Mathf.Sqrt(Re_wind) * Mathf.Pow(Constants.Pr, 1f / 3f);
    }

    // 发动机热能
    private float CalculateEengine(float power)
    {
        return (1 - 0.4f) * power * 0.3f * 427000f / 3600f;
    }

    // 开火后增加热能
    private float CalculateEfire(int fireCount)
    {
        return 0.6f * fireCount * 0.3f * 0.5f / (1f * (0.4f * Constants.PI)) * 0.5f * 2f * Mathf.Pow(20f, 2);
    }

    // 普朗克定律积分近似公式
    private float Calculateapprox(float wave, float T, float emm)
    {
        return emm / Constants.PI * Constants.radc1 * T / Constants.radc2 *
               Mathf.Exp(-Constants.radc2 / (wave * T)) *
               (Mathf.Pow(wave, 3) + 3 * T / Constants.radc2 *
               (Mathf.Pow(wave, 2) + 2 * T / Constants.radc2 *
               (wave + T / Constants.radc2)));
    }

    // 计算大气透过率
    private float CalculateAtmosphericTransmittance(float distance, float wavelength)
    {
        // 使用能见度估算气溶胶消光系数
        float extinctionCoefficient = 3.912f / (currentVisibility / 1000f); // 转换为km

        // Ångström公式
        float angstromExponent = GetAngstromExponent();
        float wavelengthDependence = Mathf.Pow(0.55f / wavelength, angstromExponent);

        // 总消光
        float totalExtinction = extinctionCoefficient * wavelengthDependence;

        // 比尔-朗伯定律
        return Mathf.Exp(-totalExtinction * distance);
    }

    // 获取Ångström指数
    private float GetAngstromExponent()
    {
        switch (currentWeather)
        {
            case Constants.WeatherType.Sunny:
                return Constants.Sunny_Angstrom;
            case Constants.WeatherType.Cloudy:
                return Constants.Cloudy_Angstrom;
            case Constants.WeatherType.Rainy:
                return Constants.Rainy_Angstrom;
            case Constants.WeatherType.Foggy:
                return Constants.Foggy_Angstrom;
            case Constants.WeatherType.Snowy:
                return Constants.Snowy_Angstrom;
            default:
                return 1.0f;
        }
    }

    // 计算雨雪雾衰减
    private float CalculatePrecipitationAttenuation(float distance, float precipitationRate)
    {
        float attenuationCoefficient = 0f;

        switch (currentWeather)
        {
            case Constants.WeatherType.Rainy:
                attenuationCoefficient = Constants.Rain_Attenuation * precipitationRate;
                break;
            case Constants.WeatherType.Snowy:
                attenuationCoefficient = Constants.Snow_Attenuation * precipitationRate;
                break;
            case Constants.WeatherType.Foggy:
                attenuationCoefficient = Constants.Fog_Attenuation;
                break;
            default:
                return 1.0f; // 无衰减
        }

        // 转换为透过率：τ = 10^(-attenuation * distance / 10)
        return Mathf.Pow(10f, -attenuationCoefficient * distance / 10f);
    }

    // 新增：获取当前天气的描述信息
    public string GetWeatherDescription()
    {
        switch (currentWeather)
        {
            case Constants.WeatherType.Sunny:
                return "晴天 - 高能见度，强太阳辐射";
            case Constants.WeatherType.Cloudy:
                return "多云 - 中等能见度，散射增强";
            case Constants.WeatherType.Rainy:
                return "雨天 - 能见度降低，红外衰减强";
            case Constants.WeatherType.Foggy:
                return "雾天 - 能见度极低，红外探测困难";
            case Constants.WeatherType.Snowy:
                return "雪天 - 能见度低，地面反射率高";
            default:
                return "未知天气";
        }
    }

    // 新增：获取天气影响摘要
    public string GetWeatherImpactSummary()
    {
        float visibilityKm = currentVisibility / 1000f;
        float transmittancePercent = currentIR_Tau * 100f;

        return string.Format(
            "当前天气: {0}\n" +
            "能见度: {1:F1} km\n" +
            "红外透过率: {2:F1}%\n" +
            "大气温度: {3:F1}°C\n" +
            "相对湿度: {4:F0}%\n" +
            "风速: {5:F1} m/s",
            GetWeatherDescription(),
            visibilityKm,
            transmittancePercent,
            currentTf - 273.15f,
            currentRH * 100f,
            currentVwind
        );
    }
}

// 新增：稳定性设置类
public class WeatherStabilitySettings
{
    public float temperatureSmoothing = 0.5f;    // 温度平滑强度
    public float radiationSmoothing = 0.3f;      // 辐射平滑强度
    public float minRadiation = 0.001f;          // 最小辐射值
    public float maxRadiation = 10f;             // 最大辐射值
    public float temperatureBias = 0f;           // 温度偏差补偿
    public float radiationBias = 0f;             // 辐射偏差补偿
    public bool useLowPassFilter = true;        // 是否使用低通滤波器
}