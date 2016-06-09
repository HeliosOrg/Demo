#pragma once
// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

/**
* This kismet library is used for helper functions primarily used in the kismet compiler for AI related nodes
* NOTE: Do not change the signatures for any of these functions as it can break the kismet compiler and/or the nodes referencing them
*/

#pragma once
#include "HeliosHelperLibrary.generated.h"

//class UBehaviorTree;
//class UAnimInstance;
//class APawn;
//class AAIController;
//class UBlackboardComponent;
class UHeliosGetterProxy_FString;
class UHeliosGetterProxy_Bool;
class UHeliosGetterProxy_Int;
class UHeliosGetterProxy_Float;

class UHeliosSetterProxy_FString;
class UHeliosSetterProxy_Bool;
class UHeliosSetterProxy_Int;
class UHeliosSetterProxy_Float;

UCLASS()
class SIMPLEDATAINTEGRATION_API UHeliosHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	// Getters

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "TRUE"))
	static UHeliosGetterProxy_FString* CreateHeliosGetterFStringProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "TRUE"))
	static UHeliosGetterProxy_Bool* CreateHeliosGetterBoolProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "TRUE"))
	static UHeliosGetterProxy_Int* CreateHeliosGetterIntProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "TRUE"))
	static UHeliosGetterProxy_Float* CreateHeliosGetterFloatProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl);

	// Setters

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "TRUE"))
	static UHeliosSetterProxy_FString* CreateHeliosSetterFStringProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl, FString InputValue);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "TRUE"))
	static UHeliosSetterProxy_Bool* CreateHeliosSetterBoolProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl, bool InputValue);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "TRUE"))
	static UHeliosSetterProxy_Int* CreateHeliosSetterIntProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl, int InputValue);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "TRUE"))
	static UHeliosSetterProxy_Float* CreateHeliosSetterFloatProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl, float InputValue);

	//UFUNCTION(BlueprintCallable, Category = "AI", meta = (DefaultToSelf = "MessageSource"))
	//static void SendAIMessage(APawn* Target, FName Message, UObject* MessageSource, bool bSuccess = true);

	//UFUNCTION(BlueprintCallable, Category = "AI", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	//static APawn* SpawnAIFromClass(UObject* WorldContextObject, TSubclassOf<APawn> PawnClass, UBehaviorTree* BehaviorTree, FVector Location, FRotator Rotation = FRotator::ZeroRotator, bool bNoCollisionFail = false);

	///** The way it works exactly is if the actor passed in is a pawn, then the function retrieves
	//*	pawn's controller cast to AIController. Otherwise the function returns actor cast to AIController. */
	//UFUNCTION(BlueprintCallable, Category = "AI", meta = (DefaultToSelf = "ControlledObject"))
	//static AAIController* GetAIController(AActor* ControlledActor);

	//UFUNCTION(BlueprintPure, Category = "AI", meta = (DefaultToSelf = "Target"))
	//static UBlackboardComponent* GetBlackboard(AActor* Target);

	///** locks indicated AI resources of animated pawn */
	//UFUNCTION(BlueprintCallable, Category = "Animation", BlueprintAuthorityOnly, meta = (DefaultToSelf = "AnimInstance"))
	//static void LockAIResourcesWithAnimation(UAnimInstance* AnimInstance, bool bLockMovement, bool LockAILogic);

	///** unlocks indicated AI resources of animated pawn. Will unlock only animation-locked resources */
	//UFUNCTION(BlueprintCallable, Category = "Animation", BlueprintAuthorityOnly, meta = (DefaultToSelf = "AnimInstance"))
	//static void UnlockAIResourcesWithAnimation(UAnimInstance* AnimInstance, bool bUnlockMovement, bool UnlockAILogic);

	//UFUNCTION(BlueprintPure, Category = "AI")
	//static bool IsValidAILocation(FVector Location);

	//UFUNCTION(BlueprintPure, Category = "AI")
	//static bool IsValidAIDirection(FVector DirectionVector);

	//UFUNCTION(BlueprintPure, Category = "AI")
	//static bool IsValidAIRotation(FRotator Rotation);
};
