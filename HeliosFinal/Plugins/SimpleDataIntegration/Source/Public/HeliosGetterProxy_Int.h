#pragma once

//#include "AITypes.h"
//#include "AI/Navigation/NavLinkDefinition.h"
//#include "Navigation/PathFollowingComponent.h"
#include "HeliosGetterProxy_Int.generated.h"

//class UWorld;
//class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeliosGetterIntRequestDelegate, int, OutputValue);

UCLASS(MinimalAPI)
class UHeliosGetterProxy_Int : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FHeliosGetterIntRequestDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FHeliosGetterIntRequestDelegate OnFail;

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
