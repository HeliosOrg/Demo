#include "SimpleDataIntegrationPCH.h"
#include "HeliosSetterProxy_Bool.h"

UHeliosSetterProxy_Bool::UHeliosSetterProxy_Bool(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Empty.
}

void UHeliosSetterProxy_Bool::SendHeliosRequest(const FName HeliosClass, const FName ServerUrl, const bool InputValue)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("POST");
	HttpRequest->SetURL(ServerUrl.ToString() + HeliosClass.ToString().ToLower());
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));

	json Content;
	Content["value"] = InputValue;
	FString ContentAsString(Content.dump().c_str());
	HttpRequest->SetContentAsString(ContentAsString);

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHeliosSetterProxy_Bool::OnHeliosSetterRequestComplete);
	HttpRequest->ProcessRequest();
}

void UHeliosSetterProxy_Bool::OnHeliosSetterRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
	if (!bSucceeded) {
		UE_LOG(LogTemp, Error, TEXT("Helios > request failed: %s"), *HttpRequest->GetURL());
		OnFail.Broadcast(false);
		return;
	}
	if (!HttpResponse.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid response for request: %s"), *HttpRequest->GetURL());
		OnFail.Broadcast(false);
		return;
	}

	OnSuccess.Broadcast(true);
}