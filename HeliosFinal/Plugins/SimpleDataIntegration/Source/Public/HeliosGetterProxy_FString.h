#pragma once

//#include "AITypes.h"
//#include "AI/Navigation/NavLinkDefinition.h"
//#include "Navigation/PathFollowingComponent.h"
#include "HeliosGetterProxy_FString.generated.h"

//class UWorld;
//class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeliosGetterFStringRequestDelegate, FString, OutputValue);

UCLASS(MinimalAPI)
class UHeliosGetterProxy_FString : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FHeliosGetterFStringRequestDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FHeliosGetterFStringRequestDelegate OnFail;

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
