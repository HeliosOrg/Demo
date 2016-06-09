#include "SimpleDataIntegrationPCH.h"
#include "HeliosGetterProxy_Bool.h"

UHeliosGetterProxy_Bool::UHeliosGetterProxy_Bool(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Empty.
}

void UHeliosGetterProxy_Bool::SendHeliosRequest(const FName HeliosClass, const FName ServerUrl)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(ServerUrl.ToString() + HeliosClass.ToString().ToLower());
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHeliosGetterProxy_Bool::OnHeliosRequestComplete);
	HttpRequest->ProcessRequest();
}

void UHeliosGetterProxy_Bool::OnHeliosRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
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

	FString ContentAsString = HttpResponse->GetContentAsString();
	json Content = json::parse(TCHAR_TO_UTF8(*ContentAsString));
	if (Content.size() != 1) {
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid json returned"));
		OnFail.Broadcast(false);
		return;
	}
	if (!Content["value"].is_boolean()) {
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid type returned"));
		OnFail.Broadcast(false);
		return;
	}
	bool Result = Content["value"];
	OnSuccess.Broadcast(Result);
}