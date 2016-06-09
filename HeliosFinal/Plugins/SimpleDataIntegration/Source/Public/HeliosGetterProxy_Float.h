#pragma once

#include "HeliosGetterProxy_Float.generated.h"

//class UWorld;
//class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeliosGetterFloatRequestDelegate, float, OutputValue);

UCLASS(MinimalAPI)
class UHeliosGetterProxy_Float : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FHeliosGetterFloatRequestDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FHeliosGetterFloatRequestDelegate OnFail;

public:
	//UFUNCTION()
	//	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult);

	void SendHeliosRequest(const FName HeliosClass, const FName ServerUrl);
	void OnHeliosRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	//void OnNoPath();

	////~ Begin UObject Interface
	//virtual void BeginDestroy() override;
	////~ End UObject Interface

	//TWeakObjectPtr<AAIController> AIController;
	//FAIRequestID MoveRequestId;
	//TWeakObjectPtr<UWorld> MyWorld;

	//FTimerHandle TimerHandle_OnNoPath;
};
