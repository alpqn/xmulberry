#pragma once

#include "ui_Mulberry.h"
#include "Setting.hpp"
#include "MimeApp.hpp"

#include <QMainWindow>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class Mulberry; }
QT_END_NAMESPACE

class Mulberry : public QMainWindow
{
    Q_OBJECT
public:
    Mulberry(QWidget* parent = nullptr);
    ~Mulberry();

    void loadMimeApps();
    void loadSettings();
    void updateUI();
    void populateFields();
    void saveChanges();
    void writeChanges(const QList<Setting>& settingsChanged, const QList<Setting>& xresourcesChanged);
private:
    Ui::Mulberry* ui;
    QList<Setting> m_settings;
    QList<Setting> m_xresources;
    QList<MimeApp> m_mimeApps;
};

