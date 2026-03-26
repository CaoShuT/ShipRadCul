Shader "Custom/InfraredGrayscale"
{
    Properties
    {
        _Temperature ("Temperature", Float) = 310
        _Radiation ("Radiation", Float) = 0
        _EmissionIntensity ("Emission Intensity", Range(0, 5)) = 4
        _MinTemp ("Minimum Temperature", Float) = 290
        _MaxTemp ("Maximum Temperature", Float) = 380
        _Contrast ("Contrast", Range(0.5, 3)) = 1.5
        _Brightness ("Brightness", Range(0, 2)) = 1.7
 
        _MainTex ("Base Texture", 2D) = "white" {}
        _BlendFactor ("Thermal Blend", Range(0, 1)) = 0.8
        [Toggle(USE_PSEUDOCOLOR)] _UsePseudoColor ("Use Pseudo Color", Float) = 0
        _PseudoColorIntensity ("Pseudo Color Intensity", Range(0, 1)) = 0.5
        
        // --- 新增视觉属性 ---
        _AmbientTemp ("Ambient Temperature", Float) = 295   // 环境温度(用于远处背景融合)
        _AtmosphereDensity ("Atmosphere Density", Range(0, 0.1)) = 0.001 // 大气密度/雾效
        _NoiseIntensity ("Noise Intensity", Range(0, 1)) = 0.1 // 传感器噪点
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 200

        CGPROGRAM
   
        #pragma surface surf Standard fullforwardshadows
        #pragma shader_feature USE_PSEUDOCOLOR

        float _Temperature;
        float _Radiation;
        float _EmissionIntensity;
        float _MinTemp;
        float _MaxTemp;
        float _Contrast;
        float _Brightness;
        float _BlendFactor;
        float _PseudoColorIntensity;
        
        // --- 新增变量 ---
        float _AmbientTemp;
        float _AtmosphereDensity;
        float _NoiseIntensity;
        
        sampler2D _MainTex;

        struct Input
        {
            float2 uv_MainTex;
            float3 worldPos; //用于计算距离
        };

        // 简单的伪随机噪声函数
        float rand(float2 co) {
            return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
        }

        // 将温度转换为灰度值 (0-1)
        float TempToGrayscale(float temp)
        {
            // 将温度归一化到0-1范围
            float normalizedTemp = saturate((temp - _MinTemp) / (_MaxTemp - _MinTemp));
            // 应用对比度和亮度调整
            float grayscale = pow(normalizedTemp, _Contrast) * _Brightness;
            return saturate(grayscale);
        }


// 将温度灰度值 (0-1) 映射到伪彩色
float3 GrayscaleToPseudoColor(float grayscale)
{
    // 确保值在 [0, 1] 范围内
    grayscale = saturate(grayscale); 
    
    float3 color = float3(0, 0, 0);

    // 0.0-0.2: 蓝紫色 -> 蓝色 (冷)
    if (grayscale < 0.2)
        color = lerp(float3(0.1, 0.0, 0.5), float3(0.0, 0.2, 1.0), grayscale * 5.0);
    // 0.2-0.4: 蓝色 -> 青色
    else if (grayscale < 0.4)
        color = lerp(float3(0.0, 0.2, 1.0), float3(0.0, 1.0, 1.0), (grayscale - 0.2) * 5.0);
    // 0.4-0.6: 青色 -> 黄色
    else if (grayscale < 0.6)
        color = lerp(float3(0.0, 1.0, 1.0), float3(1.0, 1.0, 0.0), (grayscale - 0.4) * 5.0);
    // 0.6-0.8: 黄色 -> 红色
    else if (grayscale < 0.8)
        color = lerp(float3(1.0, 1.0, 0.0), float3(1.0, 0.0, 0.0), (grayscale - 0.6) * 5.0);
    // 0.8-1.0: 红色 -> 白色 (热)
    else 
        color = lerp(float3(1.0, 0.0, 0.0), float3(1.0, 1.0, 1.0), (grayscale - 0.8) * 5.0);
     
    return color;

}

        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            // 1. 基础物体温度灰度
            float grayscale = TempToGrayscale(_Temperature);
            
            // 2. 环境背景温度灰度 (用于远处融合)
            float ambientGray = TempToGrayscale(_AmbientTemp);

            // 3. 计算距离和大气衰减 (模拟红外大气传输窗口)
            float dist = distance(_WorldSpaceCameraPos, IN.worldPos);
            // 使用指数衰减模拟比尔-朗伯定律
            float attenuation = exp(-_AtmosphereDensity * dist);
            
            // 将物体温度向环境温度融合 (距离越远，越接近环境温)
            grayscale = lerp(ambientGray, grayscale, attenuation);
            float2 noiseUV = IN.uv_MainTex;
    
    // 让噪声在Y轴方向流动 (模拟雨/雪下落的干扰)，速度适中
    // 如果是晴天，NoiseIntensity很低，这个流动几乎看不见，不影响
    noiseUV.y += _Time.y * 5.0; 
    noiseUV.x += _Time.y * 0.5; //稍微加一点横向漂移

    // 2. 计算噪声
    // rand函数保持不变，但输入坐标变化变得平滑
    float noise = (rand(noiseUV) - 0.5) * _NoiseIntensity;
    
    // 3. 叠加噪声
    grayscale = saturate(grayscale + noise);
    
    // ============ 修改结束 ============

  
    float3 textured = tex2D(_MainTex, IN.uv_MainTex).rgb;
    float3 baseColor = float3(grayscale, grayscale, grayscale);
    
    #ifdef USE_PSEUDOCOLOR
        float3 pseudoColor = GrayscaleToPseudoColor(grayscale);
        baseColor = lerp(baseColor, pseudoColor, _PseudoColorIntensity);
    #endif

    float3 finalColor = lerp(baseColor, textured, _BlendFactor);
    float radIntensity = saturate(_Radiation / 1000.0);

    o.Albedo = finalColor * 0.9;
    o.Emission = finalColor * radIntensity * _EmissionIntensity * attenuation;
    o.Metallic = 0.0;
    o.Smoothness = 0.1;
   
        }
        ENDCG
    }
    FallBack "Diffuse"
}