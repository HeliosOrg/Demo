#pragma once

//#include "AITypes.h"
//#include "AI/Navigation/NavLinkDefinition.h"
//#include "Navigation/PathFollowingComponent.h"
#include "HeliosGetterProxy_Bool.generated.h"

//class UWorld;
//class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeliosGetterBoolRequestDelegate, bool, OutputValue);

UCLASS(MinimalAPI)
class UHeliosGetterProxy_Bool : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FHeliosGetterBoolRequestDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FHeliosGetterBoolRequestDelegate OnFail;

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
