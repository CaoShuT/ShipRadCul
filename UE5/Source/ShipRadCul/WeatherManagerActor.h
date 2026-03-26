// WeatherManagerActor.h
// 天气管理Actor - UE5版本
// 对应Unity版本: WeatherManager.cs
//
// 负责管理场景中的天气状态和视觉效果（云、雨、雪粒子），
// 并同步更新场景中所有 ShipInfraredVisualizerComponent 的天气类型。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShipRadConstants.h"
#include "WeatherManagerActor.generated.h"

/**
 * 天气状态枚举 (场景级)
 */
UENUM(BlueprintType)
enum class EWeatherState : uint8
{
    Clear   UMETA(DisplayName = "晴天"),
    Rain    UMETA(DisplayName = "下雨"),
    Snow    UMETA(DisplayName = "下雪"),
    Cloudy  UMETA(DisplayName = "多云")
};

/**
 * AWeatherManagerActor
 * 
 * 放置于场景中，通过蓝图或代码切换天气状态。
 * 自动查找场景中的所有 ShipInfraredVisualizerComponent 并同步天气。
 */
UCLASS()
class SHIPRADCUL_API AWeatherManagerActor : public AActor
{
    GENERATED_BODY()

public:
    AWeatherManagerActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // ==================== 可编辑属性 ====================

    /** 云容器 Actor (可选，用于缩放模拟云层) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Effects")
    AActor* CloudContainer = nullptr;

    /** 雪粒子系统 Actor (可选) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Effects")
    AActor* SnowEffect = nullptr;

    /** 雨粒子系统 Actor (可选) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather|Effects")
    AActor* RainEffect = nullptr;

    /** 当前天气状态 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    EWeatherState CurrentState = EWeatherState::Clear;

    // ==================== 蓝图接口 ====================

    /** 设置天气为晴天 */
    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetClear();

    /** 设置天气为雨天 */
    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetRain();

    /** 设置天气为雪天 */
    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetSnow();

    /** 设置天气为多云 */
    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetCloudy();

private:
    EWeatherState LastState;

    void UpdateWeather();
    void SetWeatherParams(FVector CloudScale, bool bShowSnow, bool bShowRain);
    void NotifyVisualizerComponents(EWeatherType InfraredWeather);
};
