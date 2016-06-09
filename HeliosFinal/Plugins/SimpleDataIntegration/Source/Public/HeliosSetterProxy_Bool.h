#pragma once

#include "HeliosSetterProxy_Bool.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeliosSetterBoolRequestDelegate, bool, OutputValue);

UCLASS(MinimalAPI)
class UHeliosSetterProxy_Bool : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FHeliosSetterBoolRequestDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FHeliosSetterBoolRequestDelegate OnFail;

public:
	//UFUNCTION()
	//	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult);

	void SendHeliosRequest(const FName HeliosClass, const FName ServerUrl, const bool InputValue);
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
