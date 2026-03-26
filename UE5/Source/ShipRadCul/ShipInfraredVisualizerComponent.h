// ShipInfraredVisualizerComponent.h
// 船舶红外可视化组件 - UE5版本
// 对应Unity版本: ShipInfraredVisualizer.cs
//
// 本组件负责：
// - 将物理计算的温度和辐射值传递给材质参数
// - 根据天气类型更新视觉效果（雾效、噪点、对比度等）
// - 计算热发光颜色
// - 提供蓝图可调参数

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShipType.h"
#include "ShipRadConstants.h"
#include "ShipRadCalculation.h"
#include "ShipInfraredVisualizerComponent.generated.h"

/**
 * UShipInfraredVisualizerComponent
 * 
 * 挂载在具有 MeshComponent 的 Actor 上，自动创建动态材质实例(MID)，
 * 每帧将红外辐射物理计算结果传入材质参数，实现红外伪彩色渲染。
 */
UCLASS(ClassGroup = (ShipRadCul), meta = (BlueprintSpawnableComponent))
class SHIPRADCUL_API UShipInfraredVisualizerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UShipInfraredVisualizerComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ==================== 可编辑属性 ====================

    /** 当前部件类型 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShipRad|Part")
    EShipType ShipPart = EShipType::ShipBody;

    /** 开火次数 (仅武器部件) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShipRad|Part", meta = (ClampMin = "0"))
    int32 FireCount = 0;

    /** 发动机功率 (仅动力部件) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShipRad|Part", meta = (ClampMin = "0"))
    float EnginePower = 100.0f;

    /** 红外材质引用 (需使用 InfraredPseudoColor 材质) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShipRad|Material")
    UMaterialInterface* InfraredMaterial = nullptr;

    /** 最终平滑系数 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShipRad|Stability", meta = (ClampMin = "0.1", ClampMax = "0.5"))
    float FinalSmoothing = 0.4f;

    /** 最小显示辐射值 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShipRad|Stability")
    float MinRadiationForDisplay = 0.001f;

    /** 雾效密度全局乘数 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShipRad|Visual", meta = (ClampMin = "0.0"))
    float DensityMultiplier = 1.0f;

    /** 噪点强度全局乘数 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShipRad|Visual", meta = (ClampMin = "0.0"))
    float NoiseMultiplier = 1.0f;

    // ==================== 公共接口 ====================

    /** 设置天气类型 */
    UFUNCTION(BlueprintCallable, Category = "ShipRad|Weather")
    void SetWeather(EWeatherType Weather);

    /** 获取当前显示温度 */
    UFUNCTION(BlueprintPure, Category = "ShipRad|Debug")
    float GetDisplayedTemperature() const { return DisplayedTemperature; }

    /** 获取当前显示辐射 */
    UFUNCTION(BlueprintPure, Category = "ShipRad|Debug")
    float GetDisplayedRadiation() const { return DisplayedRadiation; }

private:
    // 物理计算引擎
    UPROPERTY()
    UShipRadCalculation* RadCalc = nullptr;

    // 动态材质实例
    UPROPERTY()
    UMaterialInstanceDynamic* DynMaterial = nullptr;

    // Mesh组件引用
    UPROPERTY()
    UPrimitiveComponent* MeshComp = nullptr;

    /** 初始热混合系数 (对应材质中的BlendFactor) */
    static constexpr float DefaultBlendFactor = 0.05f;

    // 显示状态
    float DisplayedTemperature = 300.0f;
    float DisplayedRadiation = 0.0f;
    float PrintTimer = 0.0f;

    // 内部方法
    void UpdateVisualsBasedOnWeather();
    void UpdateEmissionColor();
    FLinearColor CalculateThermalColor(float Temperature) const;
};
