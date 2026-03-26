using UnityEngine;

public static class Constants
{
    public const float PI = Mathf.PI;
    public const float ATR = (PI * 2.0f) / 360.0f;
    public const float Isc = 1353.0f; // 太阳常数 W/m^2

    // 天气类型枚举 - 扩展为5种
    public enum WeatherType
    {
        Sunny,     // 晴天
        Cloudy,    // 多云
        Rainy,     // 雨天
        Foggy,     // 雾天
        Snowy      // 雪天
    }

    // 原有常量...
    public const float day = 180.0f;  // 距离一月一日的天数

    // 大气传输相关参数
    public const float sca1 = 0.275f, sca2 = 0.53f; // 散射经验系数

    // ==================== 晴天参数 ====================
    public const float Sunny_Pm = 0.8f;        // 晴天大气透明度（高）
    public const float Sunny_Tau = 0.9f;       // 晴天大气透过率（高）
    public const float Sunny_AOD = 0.1f;       // 气溶胶光学厚度（低）
    public const float Sunny_IR_Tau = 0.95f;   // 晴天红外透过率（高）
    public const float Sunny_Visibility = 30000f; // 晴天能见度 (m) - 高能见度
    public const float Sunny_Tf = 298.0f;      // 晴天气温 (25°C)
    public const float Sunny_Tgrd = 303.0f;    // 晴天地面温度 (30°C)
    public const float Sunny_fsi = 0.6f;       // 晴天辐射传递系数
    public const float Sunny_Vwind = 2.0f;     // 晴天风速 m/s
    public const float Sunny_RH = 0.4f;        // 晴天相对湿度（低）
    public const float Sunny_Pressure = 1013.25f; // hPa
    public const float Sunny_Cloud = 0.1f;     // 晴天云量因子（低）
    public const float Sunny_RainRate = 0.0f;  // 晴天降雨率 mm/hr

    // ==================== 多云参数 ====================
    public const float Cloudy_Pm = 0.5f;       // 多云大气透明度（中等）
    public const float Cloudy_Tau = 0.7f;      // 多云大气透过率（中等）
    public const float Cloudy_AOD = 0.3f;      // 气溶胶光学厚度（中等）
    public const float Cloudy_IR_Tau = 0.8f;   // 多云红外透过率（中等）
    public const float Cloudy_Visibility = 10000f; // 多云能见度 (m)
    public const float Cloudy_Tf = 293.0f;     // 多云气温 (20°C)
    public const float Cloudy_Tgrd = 295.0f;   // 多云地面温度 (22°C)
    public const float Cloudy_fsi = 0.5f;      // 多云辐射传递系数
    public const float Cloudy_Vwind = 3.0f;    // 多云风速 m/s
    public const float Cloudy_RH = 0.7f;       // 多云相对湿度（高）
    public const float Cloudy_Pressure = 1008.0f; // hPa
    public const float Cloudy_Cloud = 0.7f;    // 多云云量因子（高）
    public const float Cloudy_RainRate = 0.0f; // 多云降雨率 mm/hr

    // ==================== 雨天参数 ====================
    public const float Rainy_Pm = 0.3f;        // 雨天大气透明度（低）
    public const float Rainy_Tau = 0.4f;       // 雨天大气透过率（低）
    public const float Rainy_AOD = 0.5f;       // 气溶胶光学厚度（高）
    public const float Rainy_IR_Tau = 0.6f;    // 雨天红外透过率（低）
    public const float Rainy_Visibility = 5000f; // 雨天能见度 (m)
    public const float Rainy_Tf = 288.0f;      // 雨天气温 (15°C)
    public const float Rainy_Tgrd = 290.0f;    // 雨天地面温度 (17°C)
    public const float Rainy_fsi = 0.4f;       // 雨天辐射传递系数
    public const float Rainy_Vwind = 4.0f;     // 雨天风速 m/s
    public const float Rainy_RH = 0.95f;       // 雨天相对湿度（很高）
    public const float Rainy_Pressure = 1005.0f; // hPa
    public const float Rainy_Cloud = 0.9f;     // 雨天云量因子（很高）
    public const float Rainy_RainRate = 5.0f;  // 雨天降雨率 mm/hr（中雨）

    // ==================== 雾天参数 ====================
    public const float Foggy_Pm = 0.1f;        // 雾天大气透明度（极低）
    public const float Foggy_Tau = 0.2f;       // 雾天大气透过率（极低）
    public const float Foggy_AOD = 2.0f;       // 气溶胶光学厚度（很高）
    public const float Foggy_IR_Tau = 0.3f;    // 雾天红外透过率（低）
    public const float Foggy_Visibility = 500f; // 雾天能见度 (m) - 低能见度
    public const float Foggy_Tf = 285.0f;      // 雾天气温 (12°C)
    public const float Foggy_Tgrd = 286.0f;    // 雾天地面温度 (13°C)
    public const float Foggy_fsi = 0.3f;       // 雾天辐射传递系数
    public const float Foggy_Vwind = 1.0f;     // 雾天风速 m/s（通常无风）
    public const float Foggy_RH = 1.0f;        // 雾天相对湿度（饱和）
    public const float Foggy_Pressure = 1010.0f; // hPa
    public const float Foggy_Cloud = 0.4f;     // 雾天云量因子（不一定有云）
    public const float Foggy_RainRate = 0.0f;  // 雾天降雨率 mm/hr

    // ==================== 雪天参数 ====================
    public const float Snowy_Pm = 0.2f;        // 雪天大气透明度（低）
    public const float Snowy_Tau = 0.3f;       // 雪天大气透过率（低）
    public const float Snowy_AOD = 0.6f;       // 气溶胶光学厚度（高）
    public const float Snowy_IR_Tau = 0.5f;    // 雪天红外透过率（中等）
    public const float Snowy_Visibility = 2000f; // 雪天能见度 (m)
    public const float Snowy_Tf = 273.0f;      // 雪天气温 (0°C)
    public const float Snowy_Tgrd = 273.0f;    // 雪天地面温度 (0°C)
    public const float Snowy_fsi = 0.5f;       // 雪天辐射传递系数
    public const float Snowy_Vwind = 3.0f;     // 雪天风速 m/s
    public const float Snowy_RH = 0.8f;        // 雪天相对湿度（高）
    public const float Snowy_Pressure = 1015.0f; // hPa
    public const float Snowy_Cloud = 0.8f;     // 雪天云量因子（高）
    public const float Snowy_RainRate = 0.5f;  // 雪天降雪率 mm/hr（等效水当量）

    // 地面反射率（根据不同天气调整）
    public const float Sunny_rhod = 0.3f;      // 晴天地面反射率
    public const float Cloudy_rhod = 0.2f;     // 多云地面反射率
    public const float Rainy_rhod = 0.15f;     // 雨天地面反射率（湿地面）
    public const float Foggy_rhod = 0.1f;      // 雾天地面反射率
    public const float Snowy_rhod = 0.8f;      // 雪天地面反射率（高）

    // 地面发射率
    public const float Sunny_epslgrd = 0.95f;  // 晴天地面发射率
    public const float Cloudy_epslgrd = 0.98f; // 多云地面发射率
    public const float Rainy_epslgrd = 0.98f;  // 雨天地面发射率
    public const float Foggy_epslgrd = 0.98f;  // 雾天地面发射率
    public const float Snowy_epslgrd = 0.85f;  // 雪天地面发射率（较低）

    // 散射类型枚举
    public enum ScatteringType
    {
        Rayleigh,    // 瑞利散射（分子散射）
        Mie,         // 米氏散射（气溶胶散射）
        Cloud,       // 云散射
        Rain,        // 雨滴散射
        Fog,         // 雾散射
        Snow         // 雪花散射
    }

    // 散射系数比例
    public const float Rayleigh_Beta = 0.1f;   // 瑞利散射系数
    public const float Mie_Beta = 0.5f;        // 米氏散射系数
    public const float Cloud_Beta = 2.0f;      // 云散射系数
    public const float Rain_Beta = 1.5f;       // 雨滴散射系数
    public const float Fog_Beta = 3.0f;        // 雾散射系数（很高）
    public const float Snow_Beta = 2.5f;       // 雪花散射系数

    // 通用常数
    public const float beta = 60.0f;           // 斜面倾角
    public const float sigma = 5.67e-8f;       // 斯蒂芬玻尔兹曼常数
    public const float aexp = 0.51f, bexp = 0.208f; // 地面反射太阳辐射经验常数

    // 对流换热相关
    public const float Re = 10000.0f;          // 雷诺数
    public const float Pr = 0.75f;             // 普朗特数
    public const float us = 1.0f, uinf = 1.0f;
    public const float dbar = 1.0f, dcan = 0.4f;
    public const float airc = 0.0242f;
    public const float Tamb = 295.0f;

    // 辐射常数
    public const float radc1 = 3.742e-16f;
    public const float radc2 = 1.439e-2f;
    public const float wave1 = 3e-6f, wave2 = 5e-6f;
    public const float Rn = 1.0f;              // 经验值
    public const float air_density = 1.225f;   // 空气密度 kg/m^3
    public const float air_ratio = 1.0f;       // 空气比热比
    public const float Ch = 1.0f;              // 经验常数
    public const float sunh = 30.0f;

    // 雨雪衰减系数（dB/km）
    public const float Rain_Attenuation = 0.5f;  // 降雨衰减系数 dB/km per mm/hr
    public const float Snow_Attenuation = 1.0f;  // 降雪衰减系数 dB/km per mm/hr
    public const float Fog_Attenuation = 5.0f;   // 雾衰减系数 dB/km（高衰减）

    // 不同天气的Ångström指数（波长依赖性）
    public const float Sunny_Angstrom = 1.3f;    // 晴天（细粒子为主）
    public const float Cloudy_Angstrom = 1.0f;   // 多云
    public const float Rainy_Angstrom = 0.5f;    // 雨天（粗粒子/雨滴）
    public const float Foggy_Angstrom = 0.2f;    // 雾天（很大粒子）
    public const float Snowy_Angstrom = 0.3f;    // 雪天（大粒子）
}