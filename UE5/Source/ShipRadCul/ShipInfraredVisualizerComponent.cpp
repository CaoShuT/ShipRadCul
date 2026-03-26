// ShipInfraredVisualizerComponent.cpp
// 船舶红外可视化组件实现 - UE5版本
// 对应Unity版本: ShipInfraredVisualizer.cs

#include "ShipInfraredVisualizerComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Actor.h"

UShipInfraredVisualizerComponent::UShipInfraredVisualizerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UShipInfraredVisualizerComponent::BeginPlay()
{
    Super::BeginPlay();

    // 创建物理计算引擎
    RadCalc = NewObject<UShipRadCalculation>(this);
    RadCalc->SetWeather(EWeatherType::Sunny);

    // 获取Mesh组件 (支持 StaticMesh 和 SkeletalMesh)
    AActor* Owner = GetOwner();
    if (!Owner) return;

    MeshComp = Owner->FindComponentByClass<UStaticMeshComponent>();
    if (!MeshComp)
    {
        MeshComp = Owner->FindComponentByClass<USkeletalMeshComponent>();
    }

    if (!MeshComp) return;

    // 创建动态材质实例
    if (InfraredMaterial)
    {
        DynMaterial = UMaterialInstanceDynamic::Create(InfraredMaterial, this);
        if (DynMaterial)
        {
            DynMaterial->SetScalarParameterValue(FName("BlendFactor"), 0.05f);
            MeshComp->SetMaterial(0, DynMaterial);
        }
    }
    else
    {
        // 尝试获取当前材质并创建动态实例
        UMaterialInterface* CurrentMat = MeshComp->GetMaterial(0);
        if (CurrentMat)
        {
            DynMaterial = UMaterialInstanceDynamic::Create(CurrentMat, this);
            DynMaterial->SetScalarParameterValue(FName("BlendFactor"), 0.05f);
            MeshComp->SetMaterial(0, DynMaterial);
        }
    }
}

void UShipInfraredVisualizerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!DynMaterial || !RadCalc) return;

    // 1. 获取物理计算值
    float Temp = RadCalc->GetTemperature(ShipPart, FireCount, EnginePower, DeltaTime);
    float Radiation = RadCalc->CalculateLdetzeroview(ShipPart, DeltaTime);

    // 2. 平滑处理
    DisplayedTemperature = FMath::Lerp(DisplayedTemperature, Temp, FinalSmoothing);
    DisplayedRadiation = FMath::Lerp(DisplayedRadiation, Radiation, FinalSmoothing);

    if (DisplayedRadiation < MinRadiationForDisplay)
    {
        DisplayedRadiation = MinRadiationForDisplay;
    }

    // 3. 调试打印
    PrintTimer += DeltaTime;
    if (PrintTimer > 2.0f)
    {
        UE_LOG(LogTemp, Log, TEXT("部件: %d | 天气: %s | 温度: %.1fK"),
            static_cast<int32>(ShipPart),
            *RadCalc->GetWeatherDescription(),
            DisplayedTemperature);
        PrintTimer = 0.0f;
    }

    // 4. 更新基础材质参数
    DynMaterial->SetScalarParameterValue(FName("Temperature"), DisplayedTemperature);
    DynMaterial->SetScalarParameterValue(FName("Radiation"), DisplayedRadiation);

    // 5. 根据天气更新视觉特效
    UpdateVisualsBasedOnWeather();

    // 6. 更新自发光颜色
    UpdateEmissionColor();
}

void UShipInfraredVisualizerComponent::UpdateVisualsBasedOnWeather()
{
    if (!RadCalc || !DynMaterial) return;

    EWeatherType WType = RadCalc->GetWeatherType();
    float Visibility = RadCalc->GetVisibility();
    float AmbientTemp = RadCalc->GetAmbientTemperature();

    // 基础大气密度 (基于能见度: Extinction = 3.912 / Visibility)
    float VisualDensity = (3.912f / FMath::Max(Visibility, 10.0f)) * 0.5f;

    float NoiseIntensity = 0.0f;
    float Contrast = 1.5f;
    float Brightness = 1.0f;

    switch (WType)
    {
    case EWeatherType::Sunny:
        NoiseIntensity = 0.02f;
        Contrast = 2.0f;
        VisualDensity *= 0.8f;
        Brightness = 20.0f;
        break;

    case EWeatherType::Cloudy:
        NoiseIntensity = 0.05f;
        Contrast = 1.5f;
        Brightness = 10.0f;
        break;

    case EWeatherType::Rainy:
        NoiseIntensity = 0.25f;
        Contrast = 1.2f;
        Brightness = 3.0f;
        break;

    case EWeatherType::Foggy:
        NoiseIntensity = 0.15f;
        Contrast = 0.8f;
        VisualDensity *= 1.2f;
        Brightness = 1.5f;
        break;

    case EWeatherType::Snowy:
        NoiseIntensity = 0.3f;
        Contrast = 1.6f;
        Brightness = 5.0f;
        break;
    }

    // 应用参数到材质
    DynMaterial->SetScalarParameterValue(FName("AmbientTemp"), AmbientTemp);
    DynMaterial->SetScalarParameterValue(FName("AtmosphereDensity"), VisualDensity * DensityMultiplier);
    DynMaterial->SetScalarParameterValue(FName("NoiseIntensity"), NoiseIntensity * NoiseMultiplier);
    DynMaterial->SetScalarParameterValue(FName("Contrast"), Contrast);
    DynMaterial->SetScalarParameterValue(FName("Brightness"), Brightness);
}

void UShipInfraredVisualizerComponent::UpdateEmissionColor()
{
    if (!DynMaterial) return;

    FLinearColor ThermalColor = CalculateThermalColor(DisplayedTemperature);
    float EmissionIntensity = FMath::GetMappedRangeValueClamped(
        FVector2D(MinRadiationForDisplay, 1.0f),
        FVector2D(0.1f, 1.0f),
        DisplayedRadiation);

    DynMaterial->SetVectorParameterValue(FName("EmissionColor"), ThermalColor * EmissionIntensity);
}

FLinearColor UShipInfraredVisualizerComponent::CalculateThermalColor(float Temperature) const
{
    float NormalizedTemp = FMath::GetMappedRangeValueClamped(
        FVector2D(250.0f, 400.0f), FVector2D(0.0f, 1.0f), Temperature);

    if (NormalizedTemp < 0.2f)
        return FMath::Lerp(FLinearColor::Black, FLinearColor::Blue, NormalizedTemp * 5.0f);
    else if (NormalizedTemp < 0.4f)
        return FMath::Lerp(FLinearColor::Blue, FLinearColor(0, 1, 1), (NormalizedTemp - 0.2f) * 5.0f);
    else if (NormalizedTemp < 0.6f)
        return FMath::Lerp(FLinearColor(0, 1, 1), FLinearColor::Green, (NormalizedTemp - 0.4f) * 5.0f);
    else if (NormalizedTemp < 0.8f)
        return FMath::Lerp(FLinearColor::Green, FLinearColor::Yellow, (NormalizedTemp - 0.6f) * 5.0f);
    else
        return FMath::Lerp(FLinearColor::Yellow, FLinearColor::Red, (NormalizedTemp - 0.8f) * 5.0f);
}

void UShipInfraredVisualizerComponent::SetWeather(EWeatherType Weather)
{
    if (RadCalc)
    {
        RadCalc->SetWeather(Weather);
    }
}
