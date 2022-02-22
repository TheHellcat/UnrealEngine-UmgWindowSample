// Fill out your copyright notice in the Description page of Project Settings.

#include "UmgWindowComponent.h"
#include "Engine/UserInterfaceSettings.h"

UUmgWindowComponent::UUmgWindowComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Window = nullptr;
	bUseManualDPIScaling = false;
}

void UUmgWindowComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UUmgWindowComponent::SetManualDpiScaling(bool bEnableManualScaling)
{
	bUseManualDPIScaling = bEnableManualScaling;
}

void UUmgWindowComponent::OverrideManualDpiScalingWindowSize(FVector2D OverrideWindowSize)
{
	WindowOriginalSize = OverrideWindowSize;
}

void UUmgWindowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FGeometry WindowGeometry;
	FVector2D Scale;
	float DPIFactor;

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*
	 * Apply DPI scaling to the window, so the UI contents scale proportionally with the window size.
	 * This might be a bad spot to do this, this would be better placed in some OnResized event,
	 * but hey, this is only an example ;-)
	 */
	if( Window )
	{
		// Fetch the current window geometry (size, position, etc.)
		WindowGeometry = Window->GetWindowGeometryInScreen();

		if( bUseManualDPIScaling )
		{
			// calculate the DPI factor the window contents manually, sometimes this gives a nicer UI scaling
			Scale.X = WindowGeometry.GetAbsoluteSize().X / WindowOriginalSize.X;
			Scale.Y = WindowGeometry.GetAbsoluteSize().Y / WindowOriginalSize.Y;
			DPIFactor = Scale.X; // or .Y, whatever you want the dominant edge to be
		}
		else
		{
			// fetch the DPI scale factor for the window contents from the projects configured DPI curve
			DPIFactor = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())
		   ->GetDPIScaleBasedOnSize(FIntPoint(WindowGeometry.GetAbsoluteSize().X,WindowGeometry.GetAbsoluteSize().Y));
		}

		// Set the new DPI scale factor for the window
		Window->SetDPIScaleFactor(DPIFactor);
	}
}

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
void UUmgWindowComponent::OpenAsWindow(
	UUserWidget* Widget,
	FString WindowTitle,
	FVector2D WindowPosition,
	FVector2D WindowSize,
	bool bUseOsBorder,
	bool bHasTitleBar,
	bool bDragEverywhere
	)
{
	// Remember the reference to the UMG user widget, for later use
	UmgWidget = Widget;

	// Remember original window size for the manual DPI scaling
	WindowOriginalSize = WindowSize;


	/*
	 * SET UP THE NEW WINDOW
	 */

	// Create Slate Window
	Window = SNew(SWindow)
	/* set all properties we got passed with the call */
		.Title(FText::FromString(WindowTitle))
		.ScreenPosition(WindowPosition)
		.ClientSize(WindowSize)
		.UseOSWindowBorder(bUseOsBorder)
		.bDragAnywhere(bDragEverywhere)
		.CreateTitleBar(bHasTitleBar)

	/* set a few more interesting ones, just to show they exist */
		.AutoCenter(EAutoCenter::None)
		.SaneWindowPlacement(true)
		.SizingRule(ESizingRule::UserSized)
		.Type(EWindowType::GameWindow)
		.InitialOpacity(1.0f)
		.HasCloseButton(true)
		.MaxHeight(9999)
		.MaxWidth(9999)
		.SupportsTransparency(EWindowTransparency::PerPixel)
		.LayoutBorder(FMargin {2, 2})
		.AdjustInitialSizeAndPositionForDPIScale(false)
	;

	// Add our new window to the Slate subsystem (which essentially opens it)
	FSlateApplication::Get().AddWindow(Window.ToSharedRef());

	// Make sure our new window doesn't hide behind some others
	Window.Get()->BringToFront(true);

	// Bind a method to the OnClosed event, to clean up should the user, well, close the window
	WindowClosedDelegate.BindUObject(this, &UUmgWindowComponent::OnWindowClose);
	Window->SetOnWindowClosed(WindowClosedDelegate);


	/*
	 * SET UP THE UMG USER WIDGET AS WINDOW CONTENT
	 */

	// Finally fetch the Slate widget from the UMG one....
	TSharedRef<SWidget> SlateWidget = Widget->TakeWidget();

	// ....and set it as content for our newly created/opened window
	Window->SetContent(SlateWidget);
}

/**
 * Handler/callback for the "On Closed" event of a window
 *
 * @param EventWindow The window for which the event has been thrown
 */
void UUmgWindowComponent::OnWindowClose(const TSharedRef<SWindow>& EventWindow)
{
	// Clean up the UMG widget, i.e. remove it from the window and invalidate our reference
	UmgWidget->RemoveFromParent();
	UmgWidget = nullptr;

	// Also clean up the window reference
	Window.Reset();
	Window = nullptr;
}
