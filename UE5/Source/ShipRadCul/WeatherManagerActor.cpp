// WeatherManagerActor.cpp
// 天气管理Actor实现 - UE5版本
// 对应Unity版本: WeatherManager.cs

#include "WeatherManagerActor.h"
#include "ShipInfraredVisualizerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

AWeatherManagerActor::AWeatherManagerActor()
{
    PrimaryActorTick.bCanEverTick = true;
    LastState = CurrentState;
}

void AWeatherManagerActor::BeginPlay()
{
    Super::BeginPlay();
    UpdateWeather();
    LastState = CurrentState;
}

void AWeatherManagerActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 检测天气状态变化
    if (CurrentState != LastState)
    {
        UpdateWeather();
        LastState = CurrentState;
    }
}

void AWeatherManagerActor::SetClear()
{
    CurrentState = EWeatherState::Clear;
}

void AWeatherManagerActor::SetRain()
{
    CurrentState = EWeatherState::Rain;
}

void AWeatherManagerActor::SetSnow()
{
    CurrentState = EWeatherState::Snow;
}

void AWeatherManagerActor::SetCloudy()
{
    CurrentState = EWeatherState::Cloudy;
}

void AWeatherManagerActor::UpdateWeather()
{
    switch (CurrentState)
    {
    case EWeatherState::Clear:
        SetWeatherParams(FVector::ZeroVector, false, false);
        NotifyVisualizerComponents(EWeatherType::Sunny);
        break;

    case EWeatherState::Rain:
        SetWeatherParams(FVector::ZeroVector, false, true);
        NotifyVisualizerComponents(EWeatherType::Rainy);
        break;

    case EWeatherState::Snow:
        SetWeatherParams(FVector::ZeroVector, true, false);
        NotifyVisualizerComponents(EWeatherType::Snowy);
        break;

    case EWeatherState::Cloudy:
        SetWeatherParams(FVector(4000.0f, 1000.0f, 2000.0f), false, false);
        NotifyVisualizerComponents(EWeatherType::Cloudy);
        break;
    }
}

void AWeatherManagerActor::SetWeatherParams(FVector CloudScale, bool bShowSnow, bool bShowRain)
{
    if (CloudContainer)
    {
        CloudContainer->SetActorScale3D(CloudScale);
    }

    if (SnowEffect)
    {
        SnowEffect->SetActorHiddenInGame(!bShowSnow);
        SnowEffect->SetActorEnableCollision(bShowSnow);
        SnowEffect->SetActorTickEnabled(bShowSnow);
    }

    if (RainEffect)
    {
        RainEffect->SetActorHiddenInGame(!bShowRain);
        RainEffect->SetActorEnableCollision(bShowRain);
        RainEffect->SetActorTickEnabled(bShowRain);
    }
}

void AWeatherManagerActor::NotifyVisualizerComponents(EWeatherType InfraredWeather)
{
    // 查找场景中所有包含 ShipInfraredVisualizerComponent 的 Actor
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

    for (AActor* Actor : AllActors)
    {
        if (!Actor) continue;

        UShipInfraredVisualizerComponent* VisComp =
            Actor->FindComponentByClass<UShipInfraredVisualizerComponent>();

        if (VisComp)
        {
            VisComp->SetWeather(InfraredWeather);
        }
    }
}
