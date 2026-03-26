using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

public class ShipInfraredVisualizer : MonoBehaviour
{
    public ShipType shipPart;           // 当前部件类型
    public int fireCount = 0;           // 开火次数
    public float enginePower = 100f;    // 发动机功率
    public Material infraredMaterial;

    [Header("稳定性设置")]
    [Range(0.1f, 0.5f)]
    public float finalSmoothing = 0.4f;
    public float minRadiationForDisplay = 0.001f;

    // --- 新增：材质控制参数 ---
    [Header("Shader 动态参数")]
    public float densityMultiplier = 1.0f; // 全局调整雾效密度
    public float noiseMultiplier = 1.0f;   // 全局调整噪点强度

    private ShipRadCalculation radCalc;
    private Renderer rend;
    private float displayedTemperature = 300f;
    private float displayedRadiation = 0f;

    void Start()
    {
        radCalc = new ShipRadCalculation();
        rend = GetComponent<Renderer>();
        if (rend == null) return;

        // 自动查找材质逻辑 (保留原有逻辑)
        if (infraredMaterial == null)
        {
            Material found = null;
#if UNITY_EDITOR
            string[] guids = AssetDatabase.FindAssets("infraredMaterial t:Material");
            if (guids == null || guids.Length == 0)
                guids = AssetDatabase.FindAssets("InfraredMaterial t:Material");
            if (guids != null && guids.Length > 0)
            {
                string path = AssetDatabase.GUIDToAssetPath(guids[0]);
                found = AssetDatabase.LoadAssetAtPath<Material>(path);
            }
#else
            found = Resources.Load<Material>("infraredMaterial");
            if (found == null) found = Resources.Load<Material>("InfraredMaterial");
#endif
            if (found != null) infraredMaterial = found;
        }

        if (infraredMaterial != null)
        {
            Material inst = new Material(infraredMaterial);
            if (rend.sharedMaterial != null && rend.sharedMaterial.mainTexture != null)
                inst.SetTexture("_MainTex", rend.sharedMaterial.mainTexture);
            // 初始设置
            inst.SetFloat("_BlendFactor", 0.05f);
            rend.material = inst;
        }

 
    }

    private float printTimer = 0f;

    void Update()
    {
        if (rend == null || rend.material == null) return;

        // 1. 获取物理计算值
        float temp = radCalc.GetTemperature(shipPart, fireCount, enginePower);
        float radiation = radCalc.CalculateLdetzeroview(shipPart);

        // 2. 平滑处理
        displayedTemperature = Mathf.Lerp(displayedTemperature, temp, finalSmoothing);
        displayedRadiation = Mathf.Lerp(displayedRadiation, radiation, finalSmoothing);

        if (displayedRadiation < minRadiationForDisplay)
            displayedRadiation = minRadiationForDisplay;

        // 3. 调试打印
        printTimer += Time.deltaTime;
        if (printTimer > 2f)
        {
            Debug.Log($"部件: {shipPart} | 天气: {radCalc.GetWeatherType()} | 温度: {displayedTemperature:F1}K");
            printTimer = 0f;
        }

        // 4. 更新基础材质参数
        rend.material.SetFloat("_Temperature", displayedTemperature);
        rend.material.SetFloat("_Radiation", displayedRadiation);

        // 5. 根据天气更新视觉特效 (新增核心逻辑)
        UpdateVisualsBasedOnWeather();

        // 6. 更新自发光颜色
        UpdateEmissionColor();
    }

    // 新增：根据天气控制 Shader 视觉效果
    private void UpdateVisualsBasedOnWeather()
    {
        if (radCalc == null) return;

        Constants.WeatherType wType = radCalc.GetWeatherType();
        float visibility = radCalc.GetVisibility(); // 获取物理能见度 (m)
        float ambientTemp = radCalc.GetAmbientTemperature(); // 获取环境温度 (K)

        // 计算基础大气密度 (基于物理能见度公式: Extinction = 3.912 / Visibility)
        // 这里的系数 0.5 是为了在 Shader 的指数衰减中视觉效果更好，需根据场景单位调整
        float visualDensity = (3.912f / Mathf.Max(visibility, 10f)) * 0.5f;

        float noiseIntensity = 0f;
        float contrast = 1.5f;
        float brightness = 1f;

        // 针对不同天气的艺术微调
        switch (wType)
        {
            case Constants.WeatherType.Sunny:
                // 晴天：清晰，高对比，低噪点，极低雾
                noiseIntensity = 0.02f;
                contrast = 2.0f;
                visualDensity *= 0.8f; // 晴天稍微让空气更透彻
                brightness = 20f;
                break;

            case Constants.WeatherType.Cloudy:
                // 多云：中等对比
                noiseIntensity = 0.05f;
                contrast = 1.5f;
             brightness = 10f;
                break;

            case Constants.WeatherType.Rainy:
                // 雨天：低对比，高噪点，整体偏暗
                noiseIntensity = 0.25f;
                contrast = 1.2f;
                brightness = 3f;
                break;

            case Constants.WeatherType.Foggy:
                // 雾天：极低对比，中等噪点，强烈的深度衰减
                noiseIntensity = 0.15f;
                contrast = 0.8f; // 降低对比度模拟散射
                visualDensity *= 1.2f; // 增强雾感
               brightness = 1.5f;
                break;

            case Constants.WeatherType.Snowy:
                // 雪天：高反差噪点（模拟雪花干扰）
                noiseIntensity = 0.3f;
                contrast = 1.6f;
                brightness = 5f;
                break;
        }

        // 应用参数到 Shader
        rend.material.SetFloat("_AmbientTemp", ambientTemp);
        rend.material.SetFloat("_AtmosphereDensity", visualDensity * densityMultiplier);
        rend.material.SetFloat("_NoiseIntensity", noiseIntensity * noiseMultiplier);
        rend.material.SetFloat("_Contrast", contrast);
        rend.material.SetFloat("_Brightness", brightness);
    }

    private void UpdateEmissionColor()
    {
        if (!rend.material.HasProperty("_EmissionColor")) return;
        Color thermalColor = CalculateThermalColor(displayedTemperature);
        float emissionIntensity = Mathf.Lerp(0.1f, 1.0f, Mathf.InverseLerp(minRadiationForDisplay, 1.0f, displayedRadiation));
        rend.material.SetColor("_EmissionColor", thermalColor * emissionIntensity);
        if (rend.material.HasProperty("_EMISSION")) rend.material.EnableKeyword("_EMISSION");
    }

    private Color CalculateThermalColor(float temperature)
    {
        float normalizedTemp = Mathf.InverseLerp(250f, 400f, temperature);
        if (normalizedTemp < 0.2f) return Color.Lerp(Color.black, Color.blue, normalizedTemp * 5f);
        else if (normalizedTemp < 0.4f) return Color.Lerp(Color.blue, Color.cyan, (normalizedTemp - 0.2f) * 5f);
        else if (normalizedTemp < 0.6f) return Color.Lerp(Color.cyan, Color.green, (normalizedTemp - 0.4f) * 5f);
        else if (normalizedTemp < 0.8f) return Color.Lerp(Color.green, Color.yellow, (normalizedTemp - 0.6f) * 5f);
        else return Color.Lerp(Color.yellow, Color.red, (normalizedTemp - 0.8f) * 5f);
    }

    public void SetWeather(Constants.WeatherType weather)
    {
        if (radCalc != null)
        {
            radCalc.SetWeather(weather);
            // 可以在这里立即调用一次 UpdateVisuals，但放在 Update 中更平滑
            //Debug.Log($"设置天气为: {weather}");
        }
    }
}

#if UNITY_EDITOR
[CustomEditor(typeof(ShipInfraredVisualizer))]
public class ShipInfraredVisualizerEditor : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();
        ShipInfraredVisualizer visualizer = (ShipInfraredVisualizer)target;
        
        EditorGUILayout.Space();
        EditorGUILayout.LabelField("天气控制", EditorStyles.boldLabel);
        
        // 第一行按钮
        EditorGUILayout.BeginHorizontal();
        if (GUILayout.Button("晴天")) visualizer.SetWeather(Constants.WeatherType.Sunny);
        if (GUILayout.Button("多云")) visualizer.SetWeather(Constants.WeatherType.Cloudy);
        if (GUILayout.Button("雨天")) visualizer.SetWeather(Constants.WeatherType.Rainy);
        EditorGUILayout.EndHorizontal();
        
        // 第二行按钮
        EditorGUILayout.BeginHorizontal();
        if (GUILayout.Button("雾天")) visualizer.SetWeather(Constants.WeatherType.Foggy);
        if (GUILayout.Button("雪天")) visualizer.SetWeather(Constants.WeatherType.Snowy);
        EditorGUILayout.EndHorizontal();
    }
}
#endif