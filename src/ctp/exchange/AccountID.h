#ifndef __FH_CTP_EXCHANGE_CTP_ACCOUNTID_H__
#define __FH_CTP_EXCHANGE_CTP_ACCOUNTID_H__

#include <string>

namespace fh
{
namespace ctp
{
namespace exchange
{
//记录账户的基本信息：用户ID，交易账号，账号密码，经纪商代码，前置机地址
class AccountID{
public:
	void setInvestorID(const std::string &investorID);
	void setPassword(const std::string &password);
	void setBrokerID(const std::string &brokerID);
	void setMarketFrontAddress(const std::string &frontAddress);
	void setExchangeFrontAddress(const std::string &frontAddress);
	void setUserID(const std::string &userID);
	void setCombOffsetFlag(const std::string &combOffsetFlag);
	void setCombHedgeFlag(const std::string &combHedgeFlag);
	void setTimeCondition(const std::string &timeCondition);
	void setIsAutoSuspend(const std::string &isAutoSuspend);
	void setExchangeID(const std::string &exchangeID);
	void setVolumeCondition(const std::string &volumeCondition);
	void setForceCloseReason(const std::string &forceCloseReason);
	void settimeout(const std::string &timeout);
	const std::string &getInvestorID() const;
	const std::string &getPassword() const;
	const std::string &getBrokerID() const;
	const std::string &getMarketFrontAddress() const;
	const std::string &getExchangeFrontAddress() const;
	const std::string &getUserID() const;
	const std::string &getCombOffsetFlag() const;
	const std::string &getCombHedgeFlag() const;
	const std::string &getTimeCondition() const;
	const std::string &getIsAutoSuspend() const;
	const std::string &getExchangeID() const;
	const std::string &getVolumeCondition() const;
	const std::string &getForceCloseReason() const;
	const std::string &getTimeout() const;
	
private:
	std::string investorID;
	std::string password;
	std::string brokerID;
	std::string marketfrontAddress;
	std::string exchangefrontAddress;
	std::string userID;
	std::string combOffsetFlag;
	std::string combHedgeFlag;
	std::string timeCondition;
	std::string isAutoSuspend;
	std::string exchangeID;
	std::string volumeCondition;
	std::string forceCloseReason;
	std::string timeout;		
};
}
}
}

#endif