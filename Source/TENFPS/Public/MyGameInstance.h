// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
//#include "IDelegateInstance.h"
#include "MyPlayerController.h"
#include "MyGameInstance.generated.h"

class IOnlineSubsystem;
class APlayerController;
/**
 * 
 */
UCLASS()
class TENFPS_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UMyGameInstance();

	//指定玩家控制器
	void AssignPlayerController(AMyPlayerController* InController);

	//创建会话
	void HostSession();

	//加入会话
	void ClientSession();

	//摧毁会话
	void DestroySession();

	UFUNCTION(BlueprintImplementableEvent)
		void StartGame();

protected:

	//当创建会话结束后，调用这个函数
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	//当开始会话结束后，调用这个函数
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

	//加入服务器（会话Session）回调函数
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	//销毁会话回调函数
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

protected:

	AMyPlayerController* PlayerController;

	//开启服务器委托
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	//开启服务器委托句柄
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	//加入服务器委托
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	//加入服务器委托句柄
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	//销毁会话委托与句柄
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	FDelegateHandle OnDestroySessionCompleteDelegateHandle;


	IOnlineSubsystem* OnlineSub;

	TSharedPtr<const FUniqueNetId> UserID;

	//保存寻找到的Sessions
	TSharedPtr<FOnlineSessionSearch> SearchObject;
};
