#pragma once

namespace Debug
{
	static void Print(const FString& Msg, float TimeToDisplay = 6.f,  const FColor& Color = FColor::MakeRandomColor(), int32 Inkey = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(Inkey, TimeToDisplay, Color, Msg);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		}
	}

	static void Print(const FString& FloatTitle, float FloatValueToPrint, int32 InKey = -1, const FColor& Color = FColor::MakeRandomColor())
	{

		const FString FinalMsg = FloatTitle + TEXT(": ") + FString::SanitizeFloat(FloatValueToPrint);


		Print(FinalMsg,6.f, Color, InKey);
	}

	static void DrawSphere(const UObject* WorldContextObject, const FVector& Center, float Radius = 50.f, const FColor& Color = FColor::Red, float Duration = 3.f)
	{
		if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			DrawDebugSphere(World, Center, Radius, 12, Color, false, Duration);
		}
	}
}