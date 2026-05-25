// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldspaceWidget.h"
#include "EngineUtils.h"

// Sets default values
AWorldspaceWidget::AWorldspaceWidget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	NameplateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameplateWidgetComponent"));
	NameplateWidgetComponent->SetupAttachment(GetRootComponent());
	NameplateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // always faces camera
	NameplateWidgetComponent->SetDrawSize(FVector2D(200.f, 50.f));
	NameplateWidgetComponent->SetVisibility(false); // hidden until proven visible
}

// Called when the game starts or when spawned
void AWorldspaceWidget::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
	if (!LocalPC)
	{
		Destroy();
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	LocalPC->GetPlayerViewPoint(CameraLocation, CameraRotation);
	const FVector CameraForward = CameraRotation.Vector();

	const FVector MyLocation = GetActorLocation();
	const FVector ToSelf = MyLocation - CameraLocation;

	// Check 1 — Distance
	if (ToSelf.SizeSquared() > NameplateMaxDistance * NameplateMaxDistance)
	{
		Destroy();
		return;
	}

	// Check 2 — Behind camera
	if (FVector::DotProduct(CameraForward, ToSelf.GetSafeNormal()) <= 0.f)
	{
		Destroy();
		return;
	}

	// Check 3 — Wall occlusion
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (HitActor) Params.AddIgnoredActor(HitActor);

	const bool bBlocked = GetWorld()->LineTraceSingleByChannel(
		Hit, CameraLocation, MyLocation, ECC_Visibility, Params
	);

	if (bBlocked)
	{
		Destroy();
		return;
	}

	// All checks passed
	NameplateWidgetComponent->SetVisibility(true);
	OnVisibilityConfirmed(); // BP triggers the animation here

	// Self-destruct after animation finishes
	GetWorldTimerManager().SetTimer(
		NameplateCheckTimerHandle, // reusing your existing handle
		[this]() { Destroy(); },
		LifetimeDuration,
		false
	);
}

// Called every frame
void AWorldspaceWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorldspaceWidget::StartNameplateTimers()
{

}


void AWorldspaceWidget::RefreshPlayerCache()
{

}

void AWorldspaceWidget::UpdateNameplateVisibility()
{

}
