#pragma once

#include <QVariant>

#include <functional>

class Setting
{
public:
    Setting(const QString& settingName, const QVariant& values, const QString& outputCmd);

    const QString& name() const { return m_name; }
    const QVariant& values() const { return m_values; }
    const QString& outputCmd() const { return m_outputCmd; }
    QString outputCmdFormatted() const;

    void setGetter(const std::function<QVariant()>& getterLambda) { getter = getterLambda; }
    void setSetter(const std::function<void()>& setterLambda) { setter = setterLambda; setter(); }
    void setChangedCallback(const std::function<bool()>& function) { isChanged = function; }

    std::function<QVariant()> getter;
    std::function<void()> setter;
    std::function<bool()> isChanged{ [&]{ return m_values != getter(); }};
private:
    QString m_name;
    QVariant m_values;
    QString m_outputCmd;
};
