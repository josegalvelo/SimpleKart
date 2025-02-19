// Fill out your copyright notice in the Description page of Project Settings.
#include "KartDemo/Public/Kart.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/BoxComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MaterialHLSLTree.h"


// Sets default values
AKart::AKart()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCol = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = BoxCol;
	BoxCol->SetBoxExtent(FVector(100.0f, 52.0f, 10.0f));
	BoxCol->SetSimulatePhysics(true);
	BoxCol->SetLinearDamping(2.0f);
	BoxCol->SetAngularDamping(5.0f);


	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	CubeMesh->SetRelativeScale3D(FVector(2.0f, 1.0f, 0.25f));
	CubeMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	CubeMesh->SetupAttachment(BoxCol);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BoxCol);
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->SocketOffset = FVector(0.0f, 0.0f, 100.0f);
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bEnableCameraRotationLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	//SetupWheels
	FLWheel = CreateDefaultSubobject<USceneComponent>(TEXT("FLWheel"));
	FLWheel->SetupAttachment(BoxCol);
	FLWheel->SetRelativeLocation(FVector(100.0f, -50.0f, 0.0f));
	FLWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FLWheelMesh"));
	FLWheelMesh->SetupAttachment(FLWheel);
	FLWheelMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	FRWheel = CreateDefaultSubobject<USceneComponent>(TEXT("FRWheel"));
	FRWheel->SetupAttachment(BoxCol);
	FRWheel->SetRelativeLocation(FVector(100.0f, 50.0f, 0.0f));
	FRWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FRWheelMesh"));
	FRWheelMesh->SetupAttachment(FRWheel);
	FRWheelMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	BLWheel = CreateDefaultSubobject<USceneComponent>(TEXT("BLWheel"));
	BLWheel->SetupAttachment(BoxCol);
	BLWheel->SetRelativeLocation(FVector(-100.0f, -50.0f, 0.0f));
	BLWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BLWheelMesh"));
	BLWheelMesh->SetupAttachment(BLWheel);
	BLWheelMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	BRWheel = CreateDefaultSubobject<USceneComponent>(TEXT("BRWheel"));
	BRWheel->SetupAttachment(BoxCol);
	BRWheel->SetRelativeLocation(FVector(-100.0f, 50.0f, 0.0f));
	BRWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BRWheelMesh"));
	BRWheelMesh->SetupAttachment(BRWheel);
	BRWheelMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));


	SteeringSpeed = 1000000.0f;

	//Set Collisions
	BoxCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCol->SetCollisionObjectType(ECC_WorldDynamic);
	if (BoxCol)
	{
		BoxCol->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		BoxCol->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		BoxCol->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		BoxCol->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		BoxCol->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		BoxCol->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
		BoxCol->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
		BoxCol->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);
		BoxCol->SetGenerateOverlapEvents(true);
	}
	SetCollisionChannels(CubeMesh);
	SetCollisionChannels(FRWheelMesh);
	SetCollisionChannels(FLWheelMesh);
	SetCollisionChannels(BRWheelMesh);
	SetCollisionChannels(BLWheelMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		CubeMesh->SetStaticMesh(CubeMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("/Engine/EngineMeshes/Cylinder"));
	if (CylinderMeshAsset.Succeeded())
	{
		BLWheelMesh->SetStaticMesh(CylinderMeshAsset.Object);
		BRWheelMesh->SetStaticMesh(CylinderMeshAsset.Object);
		FLWheelMesh->SetStaticMesh(CylinderMeshAsset.Object);
		FRWheelMesh->SetStaticMesh(CylinderMeshAsset.Object);
	}
}

void AKart::SetCollisionChannels(UStaticMeshComponent* StaticMesh)
{
	if (StaticMesh)
	{
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		StaticMesh->SetCollisionObjectType(ECC_Vehicle);
		StaticMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
		StaticMesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
		StaticMesh->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
	}
}


// Called when the game starts or when spawned
void AKart::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SuspensionsPhys(FLWheel);
	SuspensionsPhys(FRWheel);
	SuspensionsPhys(BLWheel);
	SuspensionsPhys(BRWheel);
}

void AKart::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


// Called to bind functionality to input
void AKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::Triggered, this, &AKart::Accelerate);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AKart::Steering);
	}
}

void AKart::SuspensionsPhys(USceneComponent* Wheel)
{
	if (!IsValid(Wheel) || !IsValid(BoxCol)) return;

	FVector WheelLocation = Wheel->GetComponentLocation();
	FVector temp = Wheel->GetUpVector() * 60.0f;
	FVector TraceEnd = WheelLocation + Wheel->GetUpVector() * -60.0f;;

	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // Ignore the kart itself

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, WheelLocation, TraceEnd, ECC_Visibility, TraceParams);
	DrawDebugLine(GetWorld(), WheelLocation, TraceEnd, FColor::Green, false, 0.1f, 0, 2.0f);

	if (bHit)
	{
		//Gravity force
		float Distance = HitResult.Distance; // Distance from wheel to hit point
		float NormalizedDistance = FMath::Clamp((Distance - 0.0f) / (60.0f - 0.0f), 0.0f, 1.0f);

		FVector Direction = (WheelLocation - HitResult.Location).GetSafeNormal();
		FVector Force = Direction * (1.0f - NormalizedDistance) * 90000.0f;
		BoxCol->AddForceAtLocation(Force, WheelLocation);

		//movement Force
		FVector movementeForce = BoxCol->GetForwardVector() * 1000.0f * Acceleration * BoxCol->GetMass();
		BoxCol->AddForceAtLocation(movementeForce, WheelLocation);
		BoxCol->SetCenterOfMass(FVector(50.0f, 0.0f, 0.0f) * Acceleration);

		UStaticMeshComponent* TempWheel = Cast<UStaticMeshComponent>(Wheel->GetChildComponent(0));
		float halfExtend = TempWheel->GetStaticMesh()->GetBoundingBox().GetExtent().Z;
		float tempWheelLocation = FMath::FInterpTo(TempWheel->GetRelativeLocation().Z, (Distance * -1) + halfExtend,
		                                           GetWorld()->GetDeltaSeconds(), 3.0f);
		TempWheel->SetRelativeLocation(FVector(0.0f, 0.0f, tempWheelLocation));
		TempWheel->AddLocalRotation(FRotator((Acceleration * -10.0f), 0.0f, 0.0f));
	}
	else
	{
		FVector movementeForce = BoxCol->GetForwardVector() * 500.0f * Acceleration * BoxCol->GetMass();
		BoxCol->AddForceAtLocation(movementeForce, WheelLocation);
	}
}

void AKart::Accelerate(const FInputActionValue& Value)
{
	if (IsOnGround())
	{
		Acceleration = FMath::FInterpTo(Acceleration, Value.Get<float>(), GetWorld()->GetDeltaSeconds(), 0.3f);
	}
	else
	{
		Acceleration = FMath::FInterpTo(Acceleration, 0.0f, GetWorld()->GetDeltaSeconds(), 0.3f);
	}
}

void AKart::Steering(const FInputActionValue& Value)
{
	BoxCol->AddTorqueInRadians(FVector(0.0f, 0.0f, Value.Get<float>() * SteeringSpeed * Acceleration));
	RotateWheels(FLWheel, Value.Get<float>());
	RotateWheels(FRWheel, Value.Get<float>());
}

void AKart::RotateWheels(USceneComponent* Wheel, float TargetRotation)
{
	if (IsValid(Wheel))
	{
		float NewRotation = FMath::FInterpTo(Wheel->GetRelativeRotation().Yaw, TargetRotation,
		                                     GetWorld()->GetDeltaSeconds(), 3.0f);
		Wheel->SetRelativeRotation(FRotator(0.0f, NewRotation, 0.0f));
	}
}

bool AKart::IsOnGround()
{
	FVector temp = GetActorLocation();
	FVector TraceEnd = temp - FVector(0.0f, 0.0f, 35.0f);

	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // Ignore the kart itself

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, temp, TraceEnd, ECC_Visibility, TraceParams);

	return bHit;
}
