// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "UmgWindowComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMGWINDOWSAMPLE_API UUmgWindowComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUmgWindowComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Opens the given UMG user widget as/in a new, separate window.
	 *
	 * !!! THIS SIMPLE SAMPLE METHOD ONLY SUPPORTS PROPERLY CREATING **ONE** WINDOW !!!
	 * If you want to create multiple windows with the same method, you need to add proper handling of them.
	 *
	 * @param Widget The UMG UserWidget to use as content for the new window
	 * @param WindowTitle Title caption of the new window
	 * @param WindowPosition Where on screen should the new window be placed
	 * @param WindowSize Size of the new window
	 * @param bUseOsBorder Use the OS native window border (=true) or the UE own one (=false)
	 * @param bHasTitleBar Show the window title bar; window will have no title bar if set to "false"
	 * @param bDragEverywhere Can the window be dragged by clicking and dragging anywhere in the window area (=true) or only via the tile bar (=false)
	 */
	UFUNCTION(BlueprintCallable) void OpenAsWindow(UUserWidget* Widget, FString WindowTitle, FVector2D WindowPosition, FVector2D WindowSize,  bool bUseOsBorder, bool bHasTitleBar, bool bDragEverywhere);

	UFUNCTION(BlueprintCallable) void SetManualDpiScaling(bool bEnableManualScaling);
	UFUNCTION(BlueprintCallable) void OverrideManualDpiScalingWindowSize(FVector2D OverrideWindowSize);

protected:
	virtual void BeginPlay() override;
	UPROPERTY() UUserWidget* UmgWidget;
	TSharedPtr<SWindow> Window;
	FOnWindowClosed WindowClosedDelegate;
	FVector2D WindowOriginalSize;
	bool bUseManualDPIScaling;

	void OnWindowClose(const TSharedRef<SWindow>& Window);
};
