#pragma once

#include "HeliosSetterProxy_Int.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeliosSetterIntRequestDelegate, bool, OutputValue);

UCLASS(MinimalAPI)
class UHeliosSetterProxy_Int : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FHeliosSetterIntRequestDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FHeliosSetterIntRequestDelegate OnFail;

public:
	//UFUNCTION()
	//	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult);

	void SendHeliosRequest(const FName HeliosClass, const FName ServerUrl, const int InputValue);
	void OnHeliosSetterRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	//void OnNoPath();

	////~ Begin UObject Interface
	//virtual void BeginDestroy() override;
	////~ End UObject Interface

	//TWeakObjectPtr<AAIController> AIController;
	//FAIRequestID MoveRequestId;
	//TWeakObjectPtr<UWorld> MyWorld;

	//FTimerHandle TimerHandle_OnNoPath;
};
