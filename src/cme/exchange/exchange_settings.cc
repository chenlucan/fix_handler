
#include "cme/exchange/exchange_settings.h"

namespace fh
{
namespace cme
{
namespace exchange
{

    ExchangeSettings::ExchangeSettings(const std::string &app_setting_file) : m_settings(app_setting_file)
    {
        // noop
    }

    ExchangeSettings::~ExchangeSettings()
    {
        // noop
    }

    const std::pair<std::string, std::string> ExchangeSettings::Get_strategy_url() const
    {
        return {m_settings.Get("strategy.send_url"), m_settings.Get("strategy.receive_url")};
    }

    const std::pair<std::string, std::string> ExchangeSettings::Get_target_id() const
    {
        return {m_settings.Get("session.target_comp_id"), m_settings.Get("session.target_sub_id")};
    }

    const std::tuple<std::string, std::string, std::string> ExchangeSettings::Get_sender_id() const
    {
        return  std::make_tuple(m_settings.Get("session.sender_comp_id"), m_settings.Get("session.sender_sub_id"), m_settings.Get("session.sender_location_id"));
    }

    const std::pair<std::string, std::string> ExchangeSettings::Get_password() const
    {
        return {m_settings.Get("session.password"), m_settings.Get("session.password_length")};
    }

    const std::tuple<std::string, std::string, std::string> ExchangeSettings::Get_app_info() const
    {
        return  std::make_tuple(m_settings.Get("session.app_sys_name"), m_settings.Get("session.trading_sys_ver"), m_settings.Get("session.app_sys_vendor"));
    }

    const std::string ExchangeSettings::Get_account() const
    {
        return m_settings.Get("order.account");
    }

    const std::string ExchangeSettings::Get_manual_flag() const
    {
        return m_settings.Get("order.manual_order_indicator");
    }

    const std::string ExchangeSettings::Get_security_type() const
    {
        return m_settings.Get("order.security_type");
    }

    const std::string ExchangeSettings::Get_customer_flag() const
    {
        return m_settings.Get("order.customer_or_firm");
    }

    const std::string ExchangeSettings::Get_cti_code() const
    {
        return m_settings.Get("order.cti_code");
    }

} // namespace exchange
} // namespace cme
} // namespace fh
