using UnityEngine;

public enum WeatherState
{
    Clear,
    Rain,
    Snow,
    Cloudy
}

public class WeatherManager : MonoBehaviour
{
    [Header("引用设置")]
    public Transform cloudContainer;
    public GameObject snowPrefab;
    public GameObject rainPrefab;

    [Header("当前天气")]
    public WeatherState currentState = WeatherState.Clear;

    // 记录上一次的状态，用于检测面板改变
    private WeatherState _lastState;

    void Start()
    {
        UpdateWeather();
        _lastState = currentState;
    }

    void Update()
    {
        // 实时检测面板上的枚举切换
        if (currentState != _lastState)
        {
            UpdateWeather();
            _lastState = currentState;
        }
    }

    // 当你在 Inspector 面板修改值时，此方法会被调用（可选，用于非运行时的预览）
    void OnValidate()
    {
        if (cloudContainer != null && snowPrefab != null && rainPrefab != null)
        {
            UpdateWeather();
        }
    }

    public void UpdateWeather()
    {
        switch (currentState)
        {
            case WeatherState.Clear:
                SetWeatherParams(new Vector3(0, 0, 0), false, false);
                break;
            case WeatherState.Rain:
                SetWeatherParams(new Vector3(0, 0, 0), false, true);
                break;
            case WeatherState.Snow:
                SetWeatherParams(new Vector3(0, 0, 0), true, false);
                break;
            case WeatherState.Cloudy:
                SetWeatherParams(new Vector3(4000, 1000, 2000), false, false);
                break;
        }
    }

    private void SetWeatherParams(Vector3 cloudScale, bool showSnow, bool showRain)
    {
        if (cloudContainer != null) cloudContainer.localScale = cloudScale;
        if (snowPrefab != null) snowPrefab.SetActive(showSnow);
        if (rainPrefab != null) rainPrefab.SetActive(showRain);
    }
}