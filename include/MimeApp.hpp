#pragma once

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <qicon.h>

class MimeApp
{
public:
    MimeApp(const QFileInfo& file);

    const QString& getName() const { return m_name; }
    const QString& getFilename() const { return m_filename; }
    const QStringList& getMimeTypes() const { return m_mimeTypes; }
    const QIcon& getIcon() const { return m_icon; }
    const QStringList& getActiveMimeTypes() const { return m_activeMimeTypes; }
    const QStringList& getMimeTypesAdded() const { return m_mimeTypesAdded; }

    void setName(const QString& name) { m_name = name; }
    void setFileName(const QString& filename) { m_filename = filename; }
    void setMimeTypes(const QStringList& types) { m_mimeTypes = types; }
    void setIcon(const QIcon& icon) { m_icon = icon; }
    void setActiveMimeTypes(const QStringList& types) { m_activeMimeTypes = types; }
    void setMimeTypesAdded(const QString& type) { m_mimeTypesAdded.append(type); }
    void setMimeTypesAdded(const QStringList& types) { m_mimeTypesAdded = types; }
private:
    QString m_name;
    QString m_filename;
    QStringList m_mimeTypes;
    QStringList m_activeMimeTypes;
    QStringList m_mimeTypesAdded;
    QIcon m_icon;
};
