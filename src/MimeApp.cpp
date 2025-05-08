#include "MimeApp.hpp"

#include <QTextStream>
#include <QSettings>
#include <QRegularExpression>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include <iostream>

MimeApp::MimeApp(const QFileInfo& fileInfo)
:m_filename{ fileInfo.fileName() }
{
    QFile desktopFile{ fileInfo.absoluteFilePath() };
    if(!desktopFile.open(QIODevice::ReadOnly)) { std::cerr << "Cannot open " << desktopFile.fileName().toStdString() << " check its permissions.\n"; return; }

    QSettings desktopSettings{ desktopFile.fileName(), QSettings::NativeFormat };
    desktopSettings.beginGroup("Desktop Entry");
    m_name = desktopSettings.value("Name").toString();
    m_icon = QIcon::fromTheme(desktopSettings.value("Icon").toString());
    desktopSettings.endGroup();

    if(m_name.isEmpty()) return;

    QTextStream stream{ &desktopFile };
    QString line;
    QMimeDatabase db;
    while(!stream.atEnd())
    {
        line = stream.readLine();
        if(line.contains(QRegularExpression("^MimeType=")))
        {
            m_mimeTypes = line.remove("MimeType=").split(';', Qt::SkipEmptyParts);
            for(auto& type : m_mimeTypes)
            {
                QMimeType mType{ db.mimeTypeForName(type) };
                type += mType.comment().isEmpty() ? " " : "  |  " + mType.comment();
                type += mType.suffixes().isEmpty() ? "" : " (" + mType.suffixes().replaceInStrings(QRegularExpression("^"), ".").join(" ") + ')';
            }
            m_mimeTypes.sort();
        }
    }

    if(m_mimeTypes.empty()) return;

    QFile configFile{ QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/mimeapps.list" };
    if(!configFile.open(QIODevice::ReadWrite)) { std::cerr << "Cannot open " << configFile.fileName().toStdString() << " check its permissions.\n"; return; }
    else
    {
        QSettings configSettings{ configFile.fileName(), QSettings::NativeFormat };
        configSettings.beginGroup("Default Applications");
        for(auto& key : configSettings.allKeys())
        {
            if(configSettings.value(key) == m_filename)
            {
                QMimeType mKey{ db.mimeTypeForName(key) };
                key += mKey.comment().isEmpty() ? "" :  "  |  " + mKey.comment();
                key += mKey.suffixes().empty() ? "" : " (" + mKey.suffixes().replaceInStrings(QRegularExpression("^"), ".").join(" ") + ')';
                m_activeMimeTypes.append(key);
            }
        }
        configSettings.endGroup();
        configFile.close();
    }
    for(const auto& type : m_activeMimeTypes) { m_mimeTypes.removeAll(type); } // Remove mimetypes that are already set
}
