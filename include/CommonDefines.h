#pragma once

#include <string>
#include <memory>

// 用户信息
typedef struct UserInfo
{
	std::string	strUserName;
	bool bOnline;
	int iUserid;
	std::string	strAvatarpath;

	UserInfo()
	{
		strUserName = "";
		bOnline = false;
		iUserid = 0;
		strAvatarpath = "";
	}
}UserInfo_t;

// 聊天消息
typedef struct MessageInfo
{
	std::string strMsgTime;
	int iSenderId;				// 发送者id
	std::string strMsgContent;

	MessageInfo()
	{
		strMsgTime = "";
		iSenderId = 0;
		strMsgContent = "";
	}

	MessageInfo(std::string strTime, int iId, std::string strMsg)
	{
		strMsgTime = strTime;
		iSenderId = iId;
		strMsgContent = strMsg;
	}
}MessageInfo_t;

using MessageInfoPtr = std::shared_ptr<MessageInfo>;