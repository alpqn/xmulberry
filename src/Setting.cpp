#include "Setting.hpp"

Setting::Setting(const QString& settingName, const QVariant& values, const QString& outputCmd)
:m_name{ settingName }, m_values{ values }, m_outputCmd{ outputCmd + '\n' }
{
}

QString Setting::outputCmdFormatted() const
{
    QString outputCmdFormatted{ m_outputCmd };
    for(const auto& value : getter().toStringList()) { outputCmdFormatted = outputCmdFormatted.arg(value); }
    return outputCmdFormatted;
}


