#include "SimpleDataIntegrationPCH.h"
#include "HeliosSetterProxy_FString.h"

UHeliosSetterProxy_FString::UHeliosSetterProxy_FString(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Empty.
}

void UHeliosSetterProxy_FString::SendHeliosRequest(const FName HeliosClass, const FName ServerUrl, const FString InputValue)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("POST");
	HttpRequest->SetURL(ServerUrl.ToString() + HeliosClass.ToString().ToLower());
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));

	json Content;
	std::string ContentString(TCHAR_TO_UTF8(*InputValue));
	Content["value"] = ContentString;
	FString ContentAsString(Content.dump().c_str());
	HttpRequest->SetContentAsString(ContentAsString);

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHeliosSetterProxy_FString::OnHeliosSetterRequestComplete);
	HttpRequest->ProcessRequest();
}

void UHeliosSetterProxy_FString::OnHeliosSetterRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
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