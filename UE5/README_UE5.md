# 船舶红外辐射仿真系统 - UE5 移植版使用文档

## 目录

- [1. 项目概述](#1-项目概述)
- [2. 文件结构说明](#2-文件结构说明)
- [3. 从 Unity 到 UE5 的主要变化](#3-从-unity-到-ue5-的主要变化)
- [4. 快速开始：将代码集成到 UE5 项目](#4-快速开始将代码集成到-ue5-项目)
- [5. 创建红外材质](#5-创建红外材质)
- [6. 场景搭建步骤](#6-场景搭建步骤)
- [7. 蓝图使用指南](#7-蓝图使用指南)
- [8. 各类详细说明](#8-各类详细说明)
- [9. 天气系统配置](#9-天气系统配置)
- [10. 物理模型与算法说明](#10-物理模型与算法说明)
- [11. 参数调优指南](#11-参数调优指南)
- [12. 常见问题与解答](#12-常见问题与解答)
- [13. API 参考](#13-api-参考)

---

## 1. 项目概述

本项目是从 Unity 平台移植到 **Unreal Engine 5 (UE5)** 的船舶红外辐射仿真系统。系统的核心功能包括：

- **红外辐射物理计算**：基于 Stefan-Boltzmann 定律、对流换热、太阳辐射等物理模型，计算船舶各部件的平衡温度
- **红外图像可视化**：将温度映射为灰度或伪彩色图像，模拟热成像仪所看到的效果
- **多天气条件支持**：晴天、多云、雨天、雾天、雪天五种天气，影响大气透过率、散射、衰减等
- **大气效应模拟**：距离衰减、雾效、热噪点等后处理效果

### 支持的船舶部件

| 部件类型 | 枚举值 | 热特性 |
|---------|--------|--------|
| 船体 | `EShipType::ShipBody` | 中等发射率(0.75)，内部发热 |
| 大炮 | `EShipType::ShipCannon` | 较高发射率(0.85)，开火产热 |
| 防空炮 | `EShipType::AirDefenseGun` | 较高发射率(0.85)，开火产热 |
| 导弹发射器 | `EShipType::ShipMissile` | 中等发射率(0.75)，开火产热 |
| 烟囱 | `EShipType::Chimney` | 高发射率(0.9)，发动机排热 |
| 锅炉舱 | `EShipType::BoilerRoom` | 高发射率(0.9)，发动机排热 |

---

## 2. 文件结构说明

```
UE5/
├── Source/ShipRadCul/
│   ├── ShipRadCul.Build.cs                    # UE5模块构建配置
│   ├── ShipRadCulModule.h / .cpp              # 模块入口
│   ├── ShipType.h                             # 船舶部件类型枚举 (UENUM)
│   ├── ShipRadConstants.h                     # 全部物理常数和天气参数
│   ├── WeatherStabilitySettings.h             # 天气稳定性设置结构体 (USTRUCT)
│   ├── ShipRadCalculation.h / .cpp            # 核心物理计算引擎 (UObject)
│   ├── ShipInfraredVisualizerComponent.h / .cpp # 红外可视化组件 (UActorComponent)
│   └── WeatherManagerActor.h / .cpp           # 天气管理Actor (AActor)
├── Shaders/
│   └── InfraredPseudoColor.usf                # 红外伪彩色HLSL代码
└── README_UE5.md                              # 本文档
```

### 与 Unity 版本的文件对应关系

| Unity 文件 | UE5 文件 | 说明 |
|------------|----------|------|
| `ShipType.cs` | `ShipType.h` | 枚举 → UENUM |
| `Constants.cs` | `ShipRadConstants.h` | 静态类 → namespace constexpr |
| `ShipRadCalculation.cs` | `ShipRadCalculation.h/.cpp` | 普通类 → UObject |
| `ShipInfraredVisualizer.cs` | `ShipInfraredVisualizerComponent.h/.cpp` | MonoBehaviour → UActorComponent |
| `WeatherManager.cs` | `WeatherManagerActor.h/.cpp` | MonoBehaviour → AActor |
| `InfraredPseudoColor.shader` | `InfraredPseudoColor.usf` | Unity Surface Shader → UE5 Custom HLSL |

---

## 3. 从 Unity 到 UE5 的主要变化

### 3.1 架构映射

| Unity 概念 | UE5 对应 |
|-----------|---------|
| `MonoBehaviour` | `UActorComponent` / `AActor` |
| `Start()` | `BeginPlay()` |
| `Update()` | `TickComponent()` / `Tick()` |
| `Mathf` | `FMath` |
| `Vector3` | `FVector` |
| `Time.deltaTime` | `DeltaTime` (Tick参数) |
| `Material.SetFloat()` | `DynMaterial->SetScalarParameterValue()` |
| `Material.SetColor()` | `DynMaterial->SetVectorParameterValue()` |
| `Debug.Log()` | `UE_LOG(LogTemp, Log, ...)` |
| `[Header]` / `[Range]` | `UPROPERTY(Category, meta)` |
| `Dictionary<K,V>` | `TMap<K,V>` |
| `Queue<T>` | `TArray<T>` (FIFO操作) |
| `GetComponent<Renderer>()` | `FindComponentByClass<UStaticMeshComponent>()` |
| Unity Shader (CG/HLSL) | UE5 Material + Custom HLSL Node |

### 3.2 关键差异

1. **DeltaTime 传递方式**：Unity 中通过 `Time.deltaTime` 全局访问，UE5 中通过 `TickComponent` 参数传入
2. **材质系统**：Unity 使用 `Material.SetFloat()`，UE5 使用 `UMaterialInstanceDynamic` 的 `SetScalarParameterValue()`
3. **Shader**：Unity 使用 Surface Shader，UE5 使用材质编辑器 + Custom 节点
4. **对象生命周期**：Unity 中 `new ShipRadCalculation()` 即可，UE5 中使用 `NewObject<UShipRadCalculation>(this)`
5. **编辑器扩展**：Unity 的 `CustomEditor` 对应 UE5 的蓝图属性面板或自定义 DetailCustomization

---

## 4. 快速开始：将代码集成到 UE5 项目

### 步骤 1：创建或打开 UE5 项目

1. 打开 **Unreal Engine 5**（建议5.1以上版本）
2. 创建一个新的 C++ 项目（或打开已有项目）
3. 选择 **Basic Code** 或 **Blank** 模板

### 步骤 2：添加模块源码

1. 将 `UE5/Source/ShipRadCul/` 目录下的所有文件复制到你的项目 `Source/` 目录下：

```
你的项目/
└── Source/
    └── ShipRadCul/
        ├── ShipRadCul.Build.cs
        ├── ShipRadCulModule.h
        ├── ShipRadCulModule.cpp
        ├── ShipType.h
        ├── ShipRadConstants.h
        ├── WeatherStabilitySettings.h
        ├── ShipRadCalculation.h
        ├── ShipRadCalculation.cpp
        ├── ShipInfraredVisualizerComponent.h
        ├── ShipInfraredVisualizerComponent.cpp
        ├── WeatherManagerActor.h
        └── WeatherManagerActor.cpp
```

2. 在你的项目的 `.uproject` 文件中添加模块引用：

```json
{
    "Modules": [
        {
            "Name": "YourProjectName",
            "Type": "Runtime",
            "LoadingPhase": "Default"
        },
        {
            "Name": "ShipRadCul",
            "Type": "Runtime",
            "LoadingPhase": "Default"
        }
    ]
}
```

3. 如果你希望把所有代码直接放在主模块中而非独立模块，可将所有 `.h` / `.cpp` 文件复制到主模块的 `Source/你的项目名/` 目录下，并将 `SHIPRADCUL_API` 替换为你自己的模块API宏（如 `YOURPROJECT_API`）。

### 步骤 3：重新生成项目文件

- **Windows**: 右键 `.uproject` 文件 → Generate Visual Studio Project Files
- **命令行**: `UnrealBuildTool -projectfiles -project="你的项目.uproject" -game -engine`

### 步骤 4：编译项目

在 Visual Studio 或 UE5 编辑器中编译项目。确保编译成功无错误。

---

## 5. 创建红外材质

### 方法一：使用 Custom 节点（推荐）

1. 在 **Content Browser** 中右键 → **Material** → 创建新材质，命名为 `M_InfraredPseudoColor`

2. 打开材质编辑器，设置材质属性：
   - **Blend Mode**: Opaque
   - **Shading Model**: Default Lit

3. 创建以下 **Scalar Parameter** 节点（右键 → Material Expression → ScalarParameter）：

   | 参数名 | 默认值 | 说明 |
   |--------|--------|------|
   | `Temperature` | 310 | 物体温度(K) |
   | `Radiation` | 0 | 辐射亮度 |
   | `EmissionIntensity` | 4 | 发光强度 |
   | `MinTemp` | 290 | 最低温度 |
   | `MaxTemp` | 380 | 最高温度 |
   | `Contrast` | 1.5 | 对比度 |
   | `Brightness` | 1.7 | 亮度 |
   | `AmbientTemp` | 295 | 环境温度 |
   | `AtmosphereDensity` | 0.001 | 大气密度 |
   | `NoiseIntensity` | 0.1 | 噪点强度 |
   | `BlendFactor` | 0.8 | 纹理混合 |
   | `UsePseudoColor` | 0 | 伪彩色开关(0/1) |
   | `PseudoColorIntensity` | 0.5 | 伪彩色强度 |

4. 创建 **Vector Parameter** 节点：

   | 参数名 | 默认值 | 说明 |
   |--------|--------|------|
   | `EmissionColor` | (1,1,1,1) | 自发光颜色 |

5. 添加 **Custom** 节点：
   - 右键 → Custom
   - 将 `UE5/Shaders/InfraredPseudoColor.usf` 中 **第5部分"完整红外渲染主逻辑"** 下方的代码复制到 Custom 节点的 **Code** 字段
   - 设置 Custom 节点的输入引脚（名称需与代码中的变量名一致）
   - 设置输出类型为 `CMOT Float3`
   - 添加额外输出引脚 `Emissive`

6. 将 Custom 节点的输出连接到材质输出：
   - `Albedo` → **Base Color**
   - `Emissive` → **Emissive Color**
   - 设置 **Metallic** = 0, **Roughness** = 0.9 (Smoothness 0.1的对应值)

7. 添加 **Texture Sample** 和 **Camera Position WS** 节点作为 Custom 节点的输入。

### 方法二：使用材质节点图（无需自定义HLSL）

如果不想使用 Custom 节点，也可以使用 UE5 的材质节点手动搭建：

1. **温度归一化**: 使用 `Subtract` → `Divide` → `Saturate` 节点
2. **Pow** 节点实现对比度调节
3. **Lerp** 节点实现颜色混合
4. **距离衰减**: `Distance` → `Multiply` → `Exp` (取负)
5. **噪声**: 使用 `Noise` 或 `Random` 节点

### 方法三：后处理材质方式

创建 `PostProcessMaterial` 用于全屏红外效果：

1. 创建新材质，设置 **Material Domain** = Post Process
2. 在材质中读取 `SceneTexture:WorldPosition` 获取像素位置
3. 使用 Custom 节点进行完整的温度-颜色映射
4. 通过 `PostProcessVolume` 应用到场景

---

## 6. 场景搭建步骤

### 6.1 准备船舶模型

1. 导入船舶3D模型到 UE5（支持 FBX/OBJ 等格式）
2. 将模型拆分为独立部件（船体、大炮、烟囱等），每个部件作为独立的 `StaticMeshActor` 或 `SkeletalMeshActor`
3. 为每个部件赋予前面创建的红外材质 `M_InfraredPseudoColor`

### 6.2 添加红外可视化组件

对每个船舶部件 Actor：

1. 在 **Details** 面板中点击 **Add Component**
2. 搜索并添加 **ShipInfraredVisualizerComponent**
3. 配置组件属性：
   - **Ship Part**: 选择对应的部件类型（如 `ShipBody`、`Chimney` 等）
   - **Fire Count**: 如果是武器部件，设置开火次数
   - **Engine Power**: 如果是动力部件，设置发动机功率
   - **Infrared Material**: 指定红外材质

### 6.3 添加天气管理器

1. 在场景中放置一个空 Actor
2. 添加 **WeatherManagerActor** 类（或将其拖入场景）
3. 配置可选的天气效果 Actor 引用：
   - **Cloud Container**: 云层 Actor（可选）
   - **Snow Effect**: 雪粒子系统 Actor（可选）
   - **Rain Effect**: 雨粒子系统 Actor（可选）
4. 设置 **Current State** 来初始化天气

### 6.4 运行测试

1. 点击 **Play** 运行场景
2. 观察船舶部件的红外渲染效果
3. 在 **Details** 面板中切换 `WeatherManagerActor` 的 `CurrentState` 来测试不同天气
4. 查看 **Output Log** 中的调试信息（每2秒打印一次温度和天气信息）

---

## 7. 蓝图使用指南

### 7.1 通过蓝图切换天气

在任意蓝图（如 Level Blueprint 或自定义 Actor Blueprint）中：

```
1. 获取场景中的 WeatherManagerActor 引用
2. 调用以下函数之一：
   - Set Clear()    → 晴天
   - Set Rain()     → 雨天
   - Set Snow()     → 雪天
   - Set Cloudy()   → 多云
```

或者直接在 ShipInfraredVisualizerComponent 上调用：

```
1. 获取目标 Actor 上的 ShipInfraredVisualizerComponent
2. 调用 Set Weather(EWeatherType) 函数
   可选值：Sunny / Cloudy / Rainy / Foggy / Snowy
```

### 7.2 运行时动态调整参数

通过蓝图可以实时修改：

```
ShipInfraredVisualizerComponent:
  - Fire Count       → 模拟开火加热
  - Engine Power     → 调整发动机功率
  - Final Smoothing  → 调整显示平滑度
  - Density Multiplier → 调整雾效强度
  - Noise Multiplier   → 调整噪点强度
```

### 7.3 获取调试信息

```
ShipRadCalculation:
  - Get Weather Description()       → 获取天气描述文字
  - Get Weather Impact Summary()    → 获取天气影响详细信息
  - Get Ambient Temperature()       → 获取环境温度
  - Get Visibility()                → 获取当前能见度

ShipInfraredVisualizerComponent:
  - Get Displayed Temperature()     → 获取当前显示温度
  - Get Displayed Radiation()       → 获取当前显示辐射
```

---

## 8. 各类详细说明

### 8.1 EShipType (ShipType.h)

船舶部件类型枚举，标记为 `UENUM(BlueprintType)` 以便在蓝图中使用。

### 8.2 ShipRadConstants (ShipRadConstants.h)

所有物理常数以 `constexpr float` 定义在 `ShipRadConstants` 命名空间中。包括：

- 5种天气的完整参数集（透明度、透过率、温度、风速、湿度等）
- 散射系数
- 衰减系数
- Ångström指数
- Stefan-Boltzmann常数、Planck常数等

### 8.3 FWeatherStabilitySettings (WeatherStabilitySettings.h)

`USTRUCT(BlueprintType)` 结构体，控制温度和辐射计算的平滑参数。可在编辑器中调整。

### 8.4 UShipRadCalculation (ShipRadCalculation.h/.cpp)

核心物理引擎（`UObject` 派生类），包含：

- **天气系统**：5种天气切换及参数更新
- **热平衡求解**：Newton-Raphson法 + 阻尼
- **辐射分量计算**：太阳直接/散射/地反、大气长波、地面辐射
- **零视距红外辐射**：Planck定律积分近似
- **稳定性处理**：移动平均 + 低通滤波 + 范围钳制

### 8.5 UShipInfraredVisualizerComponent (ShipInfraredVisualizerComponent.h/.cpp)

`UActorComponent` 派生类，负责：

- 创建并管理动态材质实例(MID)
- 每帧获取物理计算值并传入材质参数
- 根据天气调整视觉效果参数
- 计算热发光颜色

### 8.6 AWeatherManagerActor (WeatherManagerActor.h/.cpp)

`AActor` 派生类，场景级天气管理器：

- 控制云/雨/雪粒子效果的显隐
- 自动通知场景中所有红外可视化组件同步天气

---

## 9. 天气系统配置

### 5种天气条件对比

| 参数 | 晴天 | 多云 | 雨天 | 雾天 | 雪天 |
|------|------|------|------|------|------|
| 大气透明度 | 0.8 | 0.5 | 0.3 | 0.1 | 0.2 |
| 红外透过率 | 95% | 80% | 60% | 30% | 50% |
| 能见度(m) | 30000 | 10000 | 5000 | 500 | 2000 |
| 环境温度(K) | 298 | 293 | 288 | 285 | 273 |
| 风速(m/s) | 2.0 | 3.0 | 4.0 | 1.0 | 3.0 |
| 相对湿度 | 40% | 70% | 95% | 100% | 80% |
| 地面反射率 | 0.3 | 0.2 | 0.15 | 0.1 | 0.8 |

### 视觉效果参数

| 天气 | 噪点强度 | 对比度 | 亮度 | 雾密度修正 |
|------|---------|--------|------|-----------|
| 晴天 | 0.02 | 2.0 | 20 | ×0.8 |
| 多云 | 0.05 | 1.5 | 10 | ×1.0 |
| 雨天 | 0.25 | 1.2 | 3 | ×1.0 |
| 雾天 | 0.15 | 0.8 | 1.5 | ×1.2 |
| 雪天 | 0.30 | 1.6 | 5 | ×1.0 |

---

## 10. 物理模型与算法说明

### 10.1 热平衡方程

系统通过求解以下热平衡方程来确定每个部件的平衡温度：

```
ε·σ·(T⁴ - T_env⁴)             [辐射换热 - Stefan-Boltzmann]
+ h_conv·(T - T_env)           [对流换热]
+ E_internal                   [内部发热]
- E_absorbed_env               [环境辐射吸收]
- E_fire/engine                [武器/发动机排热]
= 0
```

使用 **牛顿迭代法(Newton-Raphson)** 求解，带有：
- 阻尼因子 0.1（防止温度跳变）
- 最大变化率 5K/s（物理约束）
- 每帧钳制：`ΔT_max = 5 × DeltaTime`

### 10.2 太阳辐射计算

总太阳辐射 = 直接辐射 + 散射辐射 + 地面反射辐射

**直接辐射公式**：
```
Q_dir = r × Isc × Pm^m × exp(-0.05·RH·m) × exp(-AOD·m) × τ_precip

其中：
  r = 1 + 0.034·cos(2π·day/365)    [地球轨道修正]
  m = 1/sin(太阳高度角)              [大气质量]
```

### 10.3 零视距红外辐射

```
L_det = (E_self + E_reflect) × cos(30°) × cos(60°)

E_self = |M(λ1,T) - M(λ2,T)| × τ_IR    [自身热辐射]
E_reflect = r·(Q_sun + Q_sky) × f_weather  [环境反射辐射]
```

### 10.4 普朗克定律积分近似

在3-5μm红外波段的积分近似：

```
M(ν, T, ε) = (ε/π) × c₁ × (T/c₂) × exp(-c₂/(ν·T))
             × [ν³ + 3T/c₂·(ν² + 2T/c₂·(ν + T/c₂))]
```

---

## 11. 参数调优指南

### 11.1 温度显示不正确

- 检查 `MinTemp` 和 `MaxTemp` 材质参数是否合理
- 调整 `Contrast` 和 `Brightness` 值
- 检查 `TemperatureBias` 偏差补偿

### 11.2 画面闪烁或不稳定

- 增大 `FinalSmoothing` 值（推荐 0.3-0.5）
- 减小对应天气的 `TemperatureSmoothing` 值（更强的平滑）
- 启用 `bUseLowPassFilter`

### 11.3 雾天效果不明显

- 增大 `DensityMultiplier`
- 检查 `AtmosphereDensity` 材质参数
- 确认场景单位与能见度计算一致（UE5默认单位为厘米，可能需要调整密度系数）

### 11.4 伪彩色不显示

- 确保材质中 `UsePseudoColor` 参数设为 1
- 调整 `PseudoColorIntensity` 到 0.5-1.0

### 11.5 UE5 场景单位适配

UE5 使用 **厘米** 作为默认单位（1 UE单位 = 1cm），而物理公式中使用米。在 `ShipInfraredVisualizerComponent` 中，大气密度计算已考虑这一点。如果您的场景使用不同的单位比例，需要调整以下参数：

- `AtmosphereDensity`：除以100以适配厘米单位
- 或在材质 Custom 节点中的 `distance` 计算时除以100

---

## 12. 常见问题与解答

### Q1: 编译时提示 `SHIPRADCUL_API` 未定义？

**A**: 确保在 `.uproject` 文件中正确添加了 `ShipRadCul` 模块，或者将 `SHIPRADCUL_API` 替换为你项目的 API 宏。

### Q2: 材质没有效果，全黑或全白？

**A**: 
1. 确认 `ShipInfraredVisualizerComponent` 已添加到 Actor 上
2. 确认 Actor 上有 `StaticMeshComponent` 或 `SkeletalMeshComponent`
3. 确认材质参数名称与代码中的 `FName("xxx")` 完全一致
4. 检查 `InfraredMaterial` 引用是否已设置

### Q3: 如何在运行时通过键盘切换天气？

**A**: 在 Level Blueprint 中：
1. 添加 `Keyboard Event` (如数字键 1-5)
2. 获取 `WeatherManagerActor` 引用
3. 分别调用 `SetClear()` / `SetCloudy()` / `SetRain()` 等

### Q4: 如何添加新的船舶部件类型？

**A**: 
1. 在 `ShipType.h` 的 `EShipType` 枚举中添加新值
2. 在 `ShipRadCalculation.h` 中添加对应的材料属性数组
3. 在 `ShipRadCalculation.cpp` 的 `Expression()` / `DerExpression()` / `GetTemperature()` / `CalculateLdetzeroview()` 中添加对应的 case

### Q5: 如何修改天气参数？

**A**: 直接修改 `ShipRadConstants.h` 中的 `constexpr float` 常量值。例如修改晴天温度：
```cpp
constexpr float Sunny_Tf = 300.0f;  // 改为27°C
```

### Q6: 性能问题？

**A**: 
- `UShipRadCalculation` 的计算量很小（每帧仅几个浮点运算）
- 主要性能开销在材质渲染端
- 如果场景有大量部件，考虑降低 `TickComponent` 的频率

---

## 13. API 参考

### UShipRadCalculation

```cpp
// 设置天气
void SetWeather(EWeatherType Weather);

// 获取温度 (需传入DeltaTime)
float GetTemperature(EShipType Part, int32 FireCount, float EnginePower, float DeltaTime);

// 计算零视距红外辐射
float CalculateLdetzeroview(EShipType Part, float DeltaTime);

// 查询
EWeatherType GetWeatherType() const;
float GetAmbientTemperature() const;
float GetVisibility() const;
FString GetWeatherDescription() const;
FString GetWeatherImpactSummary() const;
```

### UShipInfraredVisualizerComponent

```cpp
// 属性
EShipType ShipPart;           // 部件类型
int32 FireCount;              // 开火次数
float EnginePower;            // 发动机功率
float FinalSmoothing;         // 最终平滑
float DensityMultiplier;      // 雾效乘数
float NoiseMultiplier;        // 噪点乘数

// 方法
void SetWeather(EWeatherType Weather);
float GetDisplayedTemperature() const;
float GetDisplayedRadiation() const;
```

### AWeatherManagerActor

```cpp
// 属性
EWeatherState CurrentState;   // 当前天气状态
AActor* CloudContainer;       // 云效果
AActor* SnowEffect;          // 雪效果
AActor* RainEffect;          // 雨效果

// 方法
void SetClear();
void SetRain();
void SetSnow();
void SetCloudy();
```

---

## 版权说明

本项目由 Unity 版本转换而来，保留了所有原始物理模型和算法。UE5 版本的代码结构已针对 Unreal Engine 的架构进行了适配优化。
