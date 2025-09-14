#pragma once

#include "ui_Mulberry.h"
#include "Setting.hpp"
#include "MimeApp.hpp"

#include <QMainWindow>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class Mulberry; }
QT_END_NAMESPACE

class Mulberry : public QMainWindow {
    Q_OBJECT
public:
    explicit Mulberry( QWidget* parent = nullptr );
    ~Mulberry();

    void loadMimeApps();
    void loadSettings();
    void updateUI();
    void populateFields();
    void saveChanges();
    void writeChanges( const QList<Setting>& settingsChanged );
private:
    Ui::Mulberry* ui;
    QList<Setting> m_settings;
    QList<MimeApp> m_mimeApps;
};