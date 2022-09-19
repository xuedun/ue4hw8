// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "../Plugins/Online/OnlineSubsystem/Source/Public/Online.h"
#include "../Plugins/Online/OnlineSubsystemUtils/Source/OnlineSubsystemUtils/Public/OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"

UMyGameInstance::UMyGameInstance()
{
	//绑定回调函数
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::
		CreateUObject(this, &UMyGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::
		CreateUObject(this, &UMyGameInstance::OnStartSessionComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::
		CreateUObject(this, &UMyGameInstance::OnFindSessionComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::
		CreateUObject(this, &UMyGameInstance::OnJoinSessionComplete);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::
		CreateUObject(this, &UMyGameInstance::OnDestroySessionComplete);
}


void UMyGameInstance::AssignPlayerController(AMyPlayerController* InController)
{
	PlayerController = InController;

	//获取OnlineSub
	OnlineSub = Online::GetSubsystem(PlayerController->GetWorld(), NAME_None);

	if (!GetLocalPlayers().Num() == 0)
		UserID = (*GetLocalPlayers()[0]->GetPreferredUniqueNetId()).AsShared();
}

void UMyGameInstance::HostSession()
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//会话设置
			FOnlineSessionSettings Settings;
			//连接数
			Settings.NumPublicConnections = 10;
			Settings.bShouldAdvertise = true;
			Settings.bAllowJoinInProgress = true;
			//使用局域网
			Settings.bIsLANMatch = true;
			Settings.bUsesPresence = true;
			Settings.bAllowJoinViaPresence = true;
			//绑定委托
			OnCreateSessionCompleteDelegateHandle = Session
				->AddOnCreateSessionCompleteDelegate_Handle
				(OnCreateSessionCompleteDelegate);
			//创建会话
			Session->CreateSession(*UserID, NAME_GameSession, Settings);
		}
	}
}

void UMyGameInstance::ClientSession()
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//实例化搜索结果指针并且设定参数
			SearchObject = MakeShareable(new FOnlineSessionSearch);
			//返回结果数
			SearchObject->MaxSearchResults = 10;
			//是否是局域网，就是IsLAN
			SearchObject->bIsLanQuery = true;
			SearchObject->QuerySettings.Set(SEARCH_PRESENCE, true,
				EOnlineComparisonOp::Equals);
			//绑定寻找会话委托
			OnFindSessionsCompleteDelegateHandle = Session->
				AddOnFindSessionsCompleteDelegate_Handle
				(OnFindSessionsCompleteDelegate);
			//进行会话寻找
			Session->FindSessions(*UserID, SearchObject.ToSharedRef());
		}
	}
}

void UMyGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//解绑创建会话完成回调函数
			Session->
				ClearOnCreateSessionCompleteDelegate_Handle
				(OnCreateSessionCompleteDelegateHandle);
			//判断创建会话是否成功
			if (bWasSuccessful)
			{
				//绑定开启会话委托
				OnStartSessionCompleteDelegateHandle = Session->
					AddOnStartSessionCompleteDelegate_Handle
					(OnStartSessionCompleteDelegate);
				Session->StartSession(NAME_GameSession);
			}
		}
	}
}

void UMyGameInstance::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//注销开启会话委托绑定
			Session->ClearOnStartSessionCompleteDelegate_Handle
			(OnStartSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				//服务端跳转场景
				UGameplayStatics::OpenLevel(PlayerController->GetWorld(),
					FName("GameMap"), true, FString("listen"));
			}
		}
	}
}

void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//取消加入对话委托绑定
			Session->ClearOnJoinSessionCompleteDelegate_Handle
			(OnJoinSessionCompleteDelegateHandle);
			//如果加入成功
			if (Result == EOnJoinSessionCompleteResult::Success)
			{
				//传送玩家到新地图
				FString ConnectString;
				if (Session->GetResolvedConnectString(NAME_GameSession, ConnectString))
				{
					//客户端切换到服务器的关卡
					PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
				}
			}
		}
	}
}



void UMyGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//取消寻找会话委托绑定
			Session->ClearOnStartSessionCompleteDelegate_Handle
			(OnStartSessionCompleteDelegateHandle);
			//如果寻找会话成功
			if (bWasSuccessful)
			{
				//如果收集的结果存在且大于1
				if (SearchObject.IsValid() && SearchObject->SearchResults.Num() > 0)
				{
					//绑定加入Session委托
					OnJoinSessionCompleteDelegateHandle = Session
						->AddOnJoinSessionCompleteDelegate_Handle
						(OnJoinSessionCompleteDelegate);
					//执行加入会话
					Session->JoinSession(*UserID, NAME_GameSession, SearchObject->SearchResults[0]);
				}
			}
		}
	}
}


void UMyGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//注销销毁会话委托
			Session->ClearOnDestroySessionCompleteDelegate_Handle
			(OnDestroySessionCompleteDelegateHandle);
			//其它逻辑。。。
		}
	}
}

void UMyGameInstance::DestroySession()
{
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			//绑定销毁会话委托
			OnDestroySessionCompleteDelegateHandle = Session->
				AddOnDestroySessionCompleteDelegate_Handle
				(OnDestroySessionCompleteDelegate);
			//执行销毁会话
			Session->DestroySession(NAME_GameSession);
		}
	}
}
