// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Networking/Public/Networking.h"
#include "TcpServerActor.generated.h"

UCLASS()
class UETCP_API ATcpServerActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATcpServerActor();

	UFUNCTION(BlueprintCallable, Category = "TCPSocket")
	bool CreateSocket();
	UFUNCTION(BlueprintCallable, Category = "TCPSocket")
	void CloseSocket();
	UFUNCTION(BlueprintCallable, Category = "TCPSocket")
	bool BindSocket(const FString &NewIP, const int32 NewPort);
	UFUNCTION(BlueprintCallable, Category = "TCPSocket")
	bool ListenSocket(const int32 MaxBackLog);
	UFUNCTION(BlueprintCallable, Category = "TCPSocket")
	bool AcceptSocket(FString &IP, int32 &Port);
	UFUNCTION(BlueprintCallable, Category = "TCPSocket")
	bool SendSocket(const FString &Msg2Send);
	UFUNCTION(BlueprintCallable, Category = "TCPSocket")
	FString RecvSoc();
	static FString StringFromBinaryArray(const TArray<uint8> &BinaryArray);

protected:
	FSocket* ServerSocket;
	FSocket* ClientSocket;
	FIPv4Address TargetIP;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndType) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
