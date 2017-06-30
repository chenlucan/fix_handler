#include "MDAccountID.h"

namespace fh
{
namespace ctp
{
namespace market
{
void MDAccountID::setInvestorID(const std::string &investorID){
	this->investorID = investorID;
}

void MDAccountID::setPassword(const std::string &password){
	this->password = password;
}

void MDAccountID::setBrokerID(const std::string &brokerID){
	this->brokerID = brokerID;
}

void MDAccountID::setMarketFrontAddress(const std::string &frontAddress){
	this->marketfrontAddress = frontAddress;
}

void MDAccountID::setExchangeFrontAddress(const std::string &frontAddress){
	this->exchangefrontAddress = frontAddress;
}

void MDAccountID::setUserID(const std::string &userID){
	this->userID = userID;
}

void MDAccountID::setCombOffsetFlag(const std::string &combOffsetFlag){
	this->combOffsetFlag = combOffsetFlag;
}

void MDAccountID::setCombHedgeFlag(const std::string &combHedgeFlag){
	this->combHedgeFlag = combHedgeFlag;
}

void MDAccountID::setTimeCondition(const std::string &timeCondition){
	this->timeCondition = timeCondition;
}

void MDAccountID::setIsAutoSuspend(const std::string &isAutoSuspend){
	this->isAutoSuspend = isAutoSuspend;
}

void MDAccountID::setExchangeID(const std::string &exchangeID){
	this->exchangeID = exchangeID;
}

void MDAccountID::setVolumeCondition(const std::string &volumeCondition){
	this->volumeCondition = volumeCondition;
}

void MDAccountID::setForceCloseReason(const std::string &forceCloseReason){
	this->forceCloseReason = forceCloseReason;
}

void MDAccountID::settimeout(const std::string &timeout){
	this->timeout = timeout;
}


const std::string &MDAccountID::getInvestorID() const{
	return investorID;
}

const std::string &MDAccountID::getPassword() const{
	return password;
}

const std::string &MDAccountID::getBrokerID() const{
	return brokerID;
}

const std::string &MDAccountID::getMarketFrontAddress() const{
	return marketfrontAddress;
}

const std::string &MDAccountID::MDAccountID::getExchangeFrontAddress() const{
	return exchangefrontAddress;
}
const std::string &MDAccountID::getUserID() const{
	return userID;
}
const std::string &MDAccountID::getCombOffsetFlag() const{
	return combOffsetFlag;
}
const std::string &MDAccountID::getCombHedgeFlag() const{
	return combHedgeFlag;
}
const std::string &MDAccountID::getTimeCondition() const{
	return timeCondition;
}
const std::string &MDAccountID::getIsAutoSuspend() const{
	return isAutoSuspend;
}
const std::string &MDAccountID::getExchangeID() const{
	return exchangeID;
}
const std::string &MDAccountID::getVolumeCondition() const{
	return volumeCondition;
}
const std::string &MDAccountID::getForceCloseReason() const{
	return forceCloseReason;
}
const std::string &MDAccountID::getTimeout() const{
	return timeout;
}

}
}
}