#include "AccountID.h"

namespace fh
{
namespace ctp
{
namespace exchange
{
void AccountID::setInvestorID(const std::string &investorID){
	this->investorID = investorID;
}

void AccountID::setPassword(const std::string &password){
	this->password = password;
}

void AccountID::setBrokerID(const std::string &brokerID){
	this->brokerID = brokerID;
}

void AccountID::setMarketFrontAddress(const std::string &frontAddress){
	this->marketfrontAddress = frontAddress;
}

void AccountID::setExchangeFrontAddress(const std::string &frontAddress){
	this->exchangefrontAddress = frontAddress;
}

void AccountID::setUserID(const std::string &userID){
	this->userID = userID;
}

void AccountID::setCombOffsetFlag(const std::string &combOffsetFlag){
	this->combOffsetFlag = combOffsetFlag;
}

void AccountID::setCombHedgeFlag(const std::string &combHedgeFlag){
	this->combHedgeFlag = combHedgeFlag;
}

void AccountID::setTimeCondition(const std::string &timeCondition){
	this->timeCondition = timeCondition;
}

void AccountID::setIsAutoSuspend(const std::string &isAutoSuspend){
	this->isAutoSuspend = isAutoSuspend;
}

void AccountID::setExchangeID(const std::string &exchangeID){
	this->exchangeID = exchangeID;
}

void AccountID::setVolumeCondition(const std::string &volumeCondition){
	this->volumeCondition = volumeCondition;
}

void AccountID::setForceCloseReason(const std::string &forceCloseReason){
	this->forceCloseReason = forceCloseReason;
}

void AccountID::settimeout(const std::string &timeout){
	this->timeout = timeout;
}


const std::string &AccountID::getInvestorID() const{
	return investorID;
}

const std::string &AccountID::getPassword() const{
	return password;
}

const std::string &AccountID::getBrokerID() const{
	return brokerID;
}

const std::string &AccountID::getMarketFrontAddress() const{
	return marketfrontAddress;
}

const std::string &AccountID::AccountID::getExchangeFrontAddress() const{
	return exchangefrontAddress;
}
const std::string &AccountID::getUserID() const{
	return userID;
}
const std::string &AccountID::getCombOffsetFlag() const{
	return combOffsetFlag;
}
const std::string &AccountID::getCombHedgeFlag() const{
	return combHedgeFlag;
}
const std::string &AccountID::getTimeCondition() const{
	return timeCondition;
}
const std::string &AccountID::getIsAutoSuspend() const{
	return isAutoSuspend;
}
const std::string &AccountID::getExchangeID() const{
	return exchangeID;
}
const std::string &AccountID::getVolumeCondition() const{
	return volumeCondition;
}
const std::string &AccountID::getForceCloseReason() const{
	return forceCloseReason;
}
const std::string &AccountID::getTimeout() const{
	return timeout;
}

}
}
}