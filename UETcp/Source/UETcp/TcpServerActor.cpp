#include "TcpServerActor.h"


// Sets default values
ATcpServerActor::ATcpServerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

bool ATcpServerActor::CreateSocket()
{
	ServerSocket =  ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
	if (!ServerSocket) {
		return false;
	}
	return true;
}

void ATcpServerActor::CloseSocket()
{
	if (ServerSocket)
	{
		//关闭，销毁
		ServerSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ServerSocket);
	}
	if (ClientSocket)
	{
		//关闭，销毁
		ClientSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
	}
}

bool ATcpServerActor::BindSocket(const FString& NewIP, const int32 NewPort)
{
	FIPv4Address::Parse(NewIP, TargetIP);
	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(TargetIP.Value);
	addr->SetPort(NewPort);
	bool bBind = ServerSocket->Bind(*addr);
	return bBind;
}

bool ATcpServerActor::ListenSocket(const int32 MaxBackLog)
{
	bool bListen = ServerSocket->Listen(MaxBackLog);
	return bListen;
}

bool ATcpServerActor::AcceptSocket(FString& IP, int32& Port)
{
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	FIPv4Address::Parse(IP, TargetIP);
	uint32 ipd;
	ClientSocket = ServerSocket->Accept(*targetAddr, "aaa");
	Port = targetAddr->GetPort();
	targetAddr->GetIp(ipd);

	char strTemp[20];
	sprintf_s(strTemp, "%d.%d.%d.%d",
		(ipd & 0xff000000) >> 24,
		(ipd & 0x00ff0000) >> 16,
		(ipd & 0x0000ff00) >> 8,
		(ipd & 0x000000ff));

	IP = FString(strTemp);

	if (!ClientSocket) {
		return false;
	}
	return true;
}

bool ATcpServerActor::SendSocket(const FString& Msg2Send)
{
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	FString serialized = Msg2Send;

	bool bsend;
	TCHAR *seriallizedChar = serialized.GetCharArray().GetData();
	int32 size = FCString::Strlen(seriallizedChar) + 1;
	int32 sent = 0;
	//注意，要用客户端这个socket
	bsend = ClientSocket->SendTo((uint8*)TCHAR_TO_UTF8(seriallizedChar), size, sent, *targetAddr);

	if (bsend)
	{
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("_____Send Succ!"));
		UE_LOG(LogTemp, Warning, TEXT("_____Send Succ!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, TEXT("_____Send failed!"));
		UE_LOG(LogTemp, Warning, TEXT("_____Send failed!"));
	}
	return bsend;
}

FString ATcpServerActor::RecvSoc()
{
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	TArray<uint8> ReceivedData;//定义一个接收器
	uint32 Size;
	//注意，要用客户端这个socket
	if (ClientSocket->HasPendingData(Size))
	{
		uint8 *Recv = new uint8[Size];
		int32 BytesRead = 0;
		//将数组调整到给定数量的元素。 新元素将被初始化。
		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
		//注意，要用客户端这个socket
		ClientSocket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *targetAddr);
		if (ReceivedData.Num() > 0)
		{
			//打印
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Bytes Read ~> %d"), ReceivedData.Num()));
			FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
			UE_LOG(LogTemp, Log, TEXT("ReceivedUE4String: %s"), *ReceivedUE4String);
			return ReceivedUE4String;
			//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));
			//判断是否发送了相对的指令，进行对应事件调用
			//if (ReceivedUE4String.Equals("jiegege"))
			//{
			//	sendSoc("server auto send ");
			//}
		}
	}
	return FString();
}

FString ATcpServerActor::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	return FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}

// Called when the game starts or when spawned
void ATcpServerActor::BeginPlay()
{
	Super::BeginPlay();
	CreateSocket();
	BindSocket(TEXT("127.0.0.1"), 2333);
	ListenSocket(1);
	FString Tmp;
	int32 TestPort;
	AcceptSocket(Tmp, TestPort);
}

void ATcpServerActor::EndPlay(const EEndPlayReason::Type EndType)
{
	CloseSocket();
	Super::EndPlay(EndType);
}

// Called every frame
void ATcpServerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FString Tmp = RecvSoc();
	if (!Tmp.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("Recv Msg: %s"), *Tmp);
	}
}
