// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "VRGripMotionControllerPluginPrivatePCH.h"
#include "GripMotionControllerComponent.h"
#include "PrimitiveSceneInfo.h"

namespace {
	/** This is to prevent destruction of motion controller components while they are
	in the middle of being accessed by the render thread */
	FCriticalSection CritSect;

	// This is already declared in the original motion controller component, redeclaring it provides a reference
	// I assume there is a better way

	/** Console variable for specifying whether motion controller late update is used */
	TAutoConsoleVariable<int32> CVarEnableMotionControllerLateUpdate(
		TEXT("vr.EnableMotionControllerLateUpdate"),
		1,
		TEXT("This command allows you to specify whether the motion controller late update is applied.\n")
		TEXT(" 0: don't use late update\n")
		TEXT(" 1: use late update (default)"),
		ECVF_Cheat);
} // anonymous namespace


void UGripMotionControllerComponent::FViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	if (!MotionControllerComponent)
	{
		return;
	}
	FScopeLock ScopeLock(&CritSect);
	if (!MotionControllerComponent || MotionControllerComponent->bDisableLowLatencyUpdate || !CVarEnableMotionControllerLateUpdate.GetValueOnGameThread())
	{
		return;
	}	

	LateUpdatePrimitives.Reset();
	GatherLateUpdatePrimitives(MotionControllerComponent, LateUpdatePrimitives);

	// Loop through gripped actors
	for (FBPActorGripInformation actor : MotionControllerComponent->GrippedActors)
	{
		if (!actor.Actor)
			continue;

		// Get primitive components attached to the actor and update them with the late motion controller offset
		TInlineComponentArray<UPrimitiveComponent*> PrimComps(actor.Actor);
		for (UPrimitiveComponent * PrimitiveComponent : PrimComps)
		{
			if (PrimitiveComponent && PrimitiveComponent->SceneProxy)
			{
				FPrimitiveSceneInfo* PrimitiveSceneInfo = PrimitiveComponent->SceneProxy->GetPrimitiveSceneInfo();
				if (PrimitiveSceneInfo)
				{
					LateUpdatePrimitiveInfo PrimitiveInfo;
					PrimitiveInfo.IndexAddress = PrimitiveSceneInfo->GetIndexAddress();
					PrimitiveInfo.SceneInfo = PrimitiveSceneInfo;
					LateUpdatePrimitives.Add(PrimitiveInfo);
				}
			}
		}
	}
}


bool UGripMotionControllerComponent::GripActor(AActor* ActorToGrip, const FTransform &WorldOffset, bool bWorldOffsetIsRelative, FName OptionalSnapToSocketName, bool bSweepCollision, bool bInteractiveCollision, bool bAllowSetMobility)
{
	if (!ActorToGrip)
	{
		UE_LOG(LogTemp, Warning, TEXT("VRGripMotionController grab function was passed an invalid or already gripped actor"));
		return false;
	}

	UPrimitiveComponent *root = Cast<UPrimitiveComponent>(ActorToGrip->GetRootComponent());

	if (!root)
	{
		UE_LOG(LogTemp, Warning, TEXT("VRGripMotionController tried to grip an actor without a UPrimitiveComponent Root"));
		return false; // Need a primitive root
	}

	// Has to be movable to work
	if (root->Mobility != EComponentMobility::Movable)
	{
		if (bAllowSetMobility)
			root->SetMobility(EComponentMobility::Movable);
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("VRGripMotionController tried to grip an actor set to static mobility and bAllowSetMobility is false"));
			return false; // It is not movable, can't grip it
		}
	}

	root->IgnoreActorWhenMoving(this->GetOwner(), true);

	// Don't add the component until after the rendering thread is done with the array
	//FScopeLock ScopeLock(&CritSect);

	FBPActorGripInformation newActorGrip;
	newActorGrip.bSweepCollision = bSweepCollision;
	newActorGrip.Actor = ActorToGrip;
	newActorGrip.bInteractiveCollision = bInteractiveCollision;

	if (OptionalSnapToSocketName.IsValid() && root->DoesSocketExist(OptionalSnapToSocketName))
	{
		// I inverse it so that laying out the sockets makes sense
		newActorGrip.RelativeTransform = root->GetSocketTransform(OptionalSnapToSocketName, ERelativeTransformSpace::RTS_Component).Inverse();
	}
	else if(bWorldOffsetIsRelative)
		newActorGrip.RelativeTransform = WorldOffset;
	else
		newActorGrip.RelativeTransform = WorldOffset.GetRelativeTransform(this->GetComponentTransform());

	GrippedActors.Add(newActorGrip);
	ActorToGrip->DisableComponentsSimulatePhysics();
	ActorToGrip->AddTickPrerequisiteComponent(this);

	this->IgnoreActorWhenMoving(ActorToGrip, true);

	return true;
}

bool UGripMotionControllerComponent::DropActor(AActor* ActorToDrop, bool bSimulate)
{
	if (!ActorToDrop)
	{
		UE_LOG(LogTemp, Warning, TEXT("VRGripMotionController drop function was passed an invalid actor"));
		return false;
	}

	bool bFoundActor = false;

	for (int i = GrippedActors.Num() - 1; i >= 0; --i)
	{
		if (GrippedActors[i].Actor == ActorToDrop)
		{
			GrippedActors.RemoveAt(i);
			bFoundActor = true;
			break;
		}
	}				

	if (!bFoundActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("VRGripMotionController grab function was passed an actor that isn't gripped"));
		return false;
	}

	// Don't drop the component until after the rendering thread is done with it
	//FScopeLock ScopeLock(&CritSect);

	ActorToDrop->RemoveTickPrerequisiteComponent(this);

	UPrimitiveComponent *root = Cast<UPrimitiveComponent>(ActorToDrop->GetRootComponent());

	this->IgnoreActorWhenMoving(ActorToDrop, false);

	if (root)
	{
		root->IgnoreActorWhenMoving(this->GetOwner(), false);

		if (bSimulate)
		{
			root->SetSimulatePhysics(true);
		}
	}

	return true;
}


void UGripMotionControllerComponent::PostTeleportMoveGrippedActors()
{
	if (!GrippedActors.Num())
		return;

	FTransform WorldTransform;
	FTransform InverseTransform = this->GetComponentTransform().Inverse();
	for (int i = GrippedActors.Num() - 1; i >= 0; --i)
	{
		if (GrippedActors[i].Actor)
		{
			// GetRelativeTransformReverse had some serious fucking floating point errors associated with it that was fucking everything up
			// Not sure whats wrong with the function but I might want to push a patch out eventually
			WorldTransform = GrippedActors[i].RelativeTransform.GetRelativeTransform(InverseTransform);

			// Need to use WITH teleport for this function so that the velocity isn't updated and without sweep so that they don't collide
			GrippedActors[i].Actor->SetActorTransform(WorldTransform, false, NULL, ETeleportType::TeleportPhysics);
		}
		else
			GrippedActors.RemoveAt(i); // If it got garbage collected then just remove the pointer, won't happen with new uproperty use, but keeping it here anyway
	}

}

void UGripMotionControllerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector Position;
	FRotator Orientation;
	bTracked = PollControllerState(Position, Orientation);

	if (bTracked)
	{
		SetRelativeLocationAndRotation(Position, Orientation);
	}

	if (!ViewExtension.IsValid() && GEngine)
	{
		TSharedPtr< FViewExtension, ESPMode::ThreadSafe > NewViewExtension(new FViewExtension(this));
		ViewExtension = NewViewExtension;
		GEngine->ViewExtensions.Add(ViewExtension);
	}

	if (!bTracked && !bUseWithoutTracking)
		return; // Don't update anything including location

	if (GrippedActors.Num())
	{
		FTransform WorldTransform;
		FTransform InverseTransform = this->GetComponentTransform().Inverse();

		for (int i = GrippedActors.Num() - 1; i >= 0; --i)
		{
			if (GrippedActors[i].Actor)
			{
				// GetRelativeTransformReverse had some serious fucking floating point errors associated with it that was fucking everything up
				// Not sure whats wrong with the function but I might want to push a patch out eventually
				WorldTransform = GrippedActors[i].RelativeTransform.GetRelativeTransform(InverseTransform);

				if (GrippedActors[i].bInteractiveCollision)
				{
					// Need to use without teleport so that the physics velocity is updated for when the actor is released to throw
					GrippedActors[i].Actor->SetActorTransform(WorldTransform, GrippedActors[i].bSweepCollision);
				}
				else if(GrippedActors[i].bSweepCollision)
				{
					FVector OriginalPosition(GrippedActors[i].Actor->GetActorLocation());
					FRotator OriginalOrientation(GrippedActors[i].Actor->GetActorRotation());

					FVector NewPosition(WorldTransform.GetTranslation());
					FRotator NewOrientation(WorldTransform.GetRotation());

					// Now sweep collision separately so we can get hits but not have the location altered
					if (bUseWithoutTracking || NewPosition != OriginalPosition || NewOrientation != OriginalOrientation)
					{
						FVector move = NewPosition - OriginalPosition;

						// ComponentSweepMulti does nothing if moving < KINDA_SMALL_NUMBER in distance, so it's important to not try to sweep distances smaller than that. 
						const float MinMovementDistSq = (FMath::Square(4.f*KINDA_SMALL_NUMBER));

						if (bUseWithoutTracking || move.SizeSquared() > MinMovementDistSq || NewOrientation == OriginalOrientation)
						{
							if (CheckActorWithSweep(GrippedActors[i].Actor, move, NewOrientation))
							{

							}
						}
					}

					// Move the actor, we are not offsetting by the hit result anyway
					GrippedActors[i].Actor->SetActorTransform(WorldTransform, false);
				}
				else
				{
					// Move the actor, we are not offsetting by the hit result anyway
					GrippedActors[i].Actor->SetActorTransform(WorldTransform, false);
				}
			}
			else
				GrippedActors.RemoveAt(i); // If it got garbage collected then just remove the pointer, won't happen with new uproperty use, but keeping it here anyway
		}
	}
}

bool UGripMotionControllerComponent::CheckActorWithSweep(AActor * ActorToCheck, FVector Move, FRotator newOrientation)
{
	TArray<FHitResult> Hits;
	// WARNING: HitResult is only partially initialized in some paths. All data is valid only if bFilledHitResult is true.
	FHitResult BlockingHit(NoInit);
	BlockingHit.bBlockingHit = false;
	BlockingHit.Time = 1.f;
	bool bFilledHitResult = false;
	bool bMoved = false;
	bool bIncludesOverlapsAtEnd = false;
	bool bRotationOnly = false;

	UPrimitiveComponent *root = Cast<UPrimitiveComponent>(ActorToCheck->GetRootComponent());

	if (!root || !root->IsQueryCollisionEnabled())
		return false;

	FVector start(root->GetComponentLocation());

	const bool bCollisionEnabled = root->IsQueryCollisionEnabled();

	if (bCollisionEnabled)
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (!root->IsRegistered())
		{
			if (ActorToCheck)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s MovedComponent %s not initialized deleteme %d in grip motion controller"), *ActorToCheck->GetName(), *root->GetName(), ActorToCheck->IsPendingKill());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("MovedComponent %s not initialized in grip motion controller"), *root->GetFullName());
			}
		}
#endif

		UWorld* const MyWorld = GetWorld();
		FComponentQueryParams Params(TEXT("sweep_params"), ActorToCheck);

		FCollisionResponseParams ResponseParam;
		root->InitSweepCollisionParams(Params, ResponseParam);

		bool const bHadBlockingHit = MyWorld->ComponentSweepMulti(Hits, root, start, start + Move, newOrientation, Params);

		if (bHadBlockingHit)
		{
			int32 BlockingHitIndex = INDEX_NONE;
			float BlockingHitNormalDotDelta = BIG_NUMBER;
			for (int32 HitIdx = 0; HitIdx < Hits.Num(); HitIdx++)
			{
				const FHitResult& TestHit = Hits[HitIdx];

				// Ignore the owning actor to the motion controller
				if (TestHit.Actor == this->GetOwner())
				{
					if (Hits.Num() == 1)
						return false;
					else
						continue;
				}

				if (TestHit.bBlockingHit)
				{
					if (TestHit.Time == 0.f)
					{
						// We may have multiple initial hits, and want to choose the one with the normal most opposed to our movement.
						const float NormalDotDelta = (TestHit.ImpactNormal | Move);
						if (NormalDotDelta < BlockingHitNormalDotDelta)
						{
							BlockingHitNormalDotDelta = NormalDotDelta;
							BlockingHitIndex = HitIdx;
						}
					}
					else if (BlockingHitIndex == INDEX_NONE)
					{
						// First non-overlapping blocking hit should be used, if an overlapping hit was not.
						// This should be the only non-overlapping blocking hit, and last in the results.
						BlockingHitIndex = HitIdx;
						break;
					}
					//}
				}
			}

			// Update blocking hit, if there was a valid one.
			if (BlockingHitIndex >= 0)
			{
				BlockingHit = Hits[BlockingHitIndex];
				bFilledHitResult = true;
			}
		}
	}

	// Handle blocking hit notifications. Avoid if pending kill (which could happen after overlaps).
	if (BlockingHit.bBlockingHit && !root->IsPendingKill())
	{
		check(bFilledHitResult);
		if (root->IsDeferringMovementUpdates())
		{
			FScopedMovementUpdate* ScopedUpdate = root->GetCurrentScopedMovement();
			ScopedUpdate->AppendBlockingHitAfterMove(BlockingHit);
		}
		else
		{
			root->DispatchBlockingHit(*ActorToCheck, BlockingHit);
		}
	}

	return true;

}

//=============================================================================
UGripMotionControllerComponent::UGripMotionControllerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;

	PlayerIndex = 0;
	Hand = EControllerHand::Left;
	bDisableLowLatencyUpdate = false;
	bHasAuthority = false;
	bUseWithoutTracking = false;
}

//=============================================================================
UGripMotionControllerComponent::~UGripMotionControllerComponent()
{
	if (ViewExtension.IsValid())
	{
		{
			// This component could be getting accessed from the render thread so it needs to wait
			// before clearing MotionControllerComponent and allowing the destructor to continue
			FScopeLock ScopeLock(&CritSect);
			ViewExtension->MotionControllerComponent = NULL;
		}

		if (GEngine)
		{
			GEngine->ViewExtensions.Remove(ViewExtension);
		}
	}
	ViewExtension.Reset();
}

//=============================================================================
bool UGripMotionControllerComponent::PollControllerState(FVector& Position, FRotator& Orientation)
{
	if (IsInGameThread())
	{
		// Cache state from the game thread for use on the render thread
		const APlayerController* Actor = Cast<APlayerController>(GetOwner());
		bHasAuthority = !Actor || Actor->IsLocalPlayerController();
	}

	if ((PlayerIndex != INDEX_NONE) && bHasAuthority)
	{
		for (auto MotionController : GEngine->MotionControllerDevices)
		{
			if ((MotionController != nullptr) && MotionController->GetControllerOrientationAndPosition(PlayerIndex, Hand, Orientation, Position))
			{
				CurrentTrackingStatus = MotionController->GetControllerTrackingStatus(PlayerIndex, Hand);
				return true;
			}
		}
	}
	return false;
}

//=============================================================================
void UGripMotionControllerComponent::FViewExtension::PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily)
{
	if (!MotionControllerComponent)
	{
		return;
	}
	FScopeLock ScopeLock(&CritSect);
	if (!MotionControllerComponent || MotionControllerComponent->bDisableLowLatencyUpdate || !CVarEnableMotionControllerLateUpdate.GetValueOnRenderThread())
	{
		return;
	}

	// Poll state for the most recent controller transform
	FVector Position;
	FRotator Orientation;
	if (!MotionControllerComponent->PollControllerState(Position, Orientation))
	{
		return;
	}

	if (LateUpdatePrimitives.Num())
	{
		// Calculate the late update transform that will rebase all children proxies within the frame of reference
		const FTransform OldLocalToWorldTransform = MotionControllerComponent->CalcNewComponentToWorld(MotionControllerComponent->GetRelativeTransform());
		const FTransform NewLocalToWorldTransform = MotionControllerComponent->CalcNewComponentToWorld(FTransform(Orientation, Position));
		const FMatrix LateUpdateTransform = (OldLocalToWorldTransform.Inverse() * NewLocalToWorldTransform).ToMatrixWithScale();

		// Apply delta to the affected scene proxies
		for (auto PrimitiveInfo : LateUpdatePrimitives)
		{
			FPrimitiveSceneInfo* RetrievedSceneInfo = InViewFamily.Scene->GetPrimitiveSceneInfo(*PrimitiveInfo.IndexAddress);
			FPrimitiveSceneInfo* CachedSceneInfo = PrimitiveInfo.SceneInfo;
			// If the retrieved scene info is different than our cached scene info then the primitive was removed from the scene
			if (CachedSceneInfo == RetrievedSceneInfo && CachedSceneInfo->Proxy)
			{
				CachedSceneInfo->Proxy->ApplyLateUpdateTransform(LateUpdateTransform);
			}
		}
		LateUpdatePrimitives.Reset();
	}
}

void UGripMotionControllerComponent::FViewExtension::GatherLateUpdatePrimitives(USceneComponent* Component, TArray<LateUpdatePrimitiveInfo>& Primitives)
{
	// If a scene proxy is present, cache it
	UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component);
	if (PrimitiveComponent && PrimitiveComponent->SceneProxy)
	{
		FPrimitiveSceneInfo* PrimitiveSceneInfo = PrimitiveComponent->SceneProxy->GetPrimitiveSceneInfo();
		if (PrimitiveSceneInfo)
		{
			LateUpdatePrimitiveInfo PrimitiveInfo;
			PrimitiveInfo.IndexAddress = PrimitiveSceneInfo->GetIndexAddress();
			PrimitiveInfo.SceneInfo = PrimitiveSceneInfo;
			Primitives.Add(PrimitiveInfo);
		}
	}

	// Gather children proxies
	const int32 ChildCount = Component->GetNumChildrenComponents();
	for (int32 ChildIndex = 0; ChildIndex < ChildCount; ++ChildIndex)
	{
		USceneComponent* ChildComponent = Component->GetChildComponent(ChildIndex);
		if (!ChildComponent)
		{
			continue;
		}

		GatherLateUpdatePrimitives(ChildComponent, Primitives);
	}
}