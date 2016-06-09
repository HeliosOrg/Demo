#include "SimpleDataIntegrationPCH.h"
#include "HeliosGetterProxy_Float.h"

UHeliosGetterProxy_Float::UHeliosGetterProxy_Float(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Empty.
}

void UHeliosGetterProxy_Float::SendHeliosRequest(const FName HeliosClass, const FName ServerUrl)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(ServerUrl.ToString() + HeliosClass.ToString().ToLower());
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHeliosGetterProxy_Float::OnHeliosRequestComplete);
	HttpRequest->ProcessRequest();
}

void UHeliosGetterProxy_Float::OnHeliosRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
	if (!bSucceeded) {
		UE_LOG(LogTemp, Error, TEXT("Helios > request failed: %s"), *HttpRequest->GetURL());
		OnFail.Broadcast(0);
		return;
	}
	if (!HttpResponse.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid response for request: %s"), *HttpRequest->GetURL());
		OnFail.Broadcast(0);
		return;
	}

	FString ContentAsString = HttpResponse->GetContentAsString();
	json Content = json::parse(TCHAR_TO_UTF8(*ContentAsString));
	if (Content.size() != 1) {
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid json returned"));
		OnFail.Broadcast(0);
		return;
	}
	if (!Content["value"].is_number_float()) {
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid type returned"));
		OnFail.Broadcast(0);
		return;
	}
	float Result = Content["value"];
	OnSuccess.Broadcast(Result);
}