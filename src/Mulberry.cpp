#include "Mulberry.hpp"
#include "Utils.hpp"
#include "ui_Mulberry.h"
#include "Utils.hpp"

#include <QFileDialog>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <qdialogbuttonbox.h>
#include <QMessageBox>
#include <QTextStream>
#include <QSettings>

#include <iostream>

Mulberry::Mulberry(QWidget* parent)
:QMainWindow{ parent } , ui{ new Ui::Mulberry }
{
    ui->setupUi(this);

    auto save = new QPushButton("Save");
    ui->statusbar->addPermanentWidget(save);

    connect(save, &QPushButton::clicked, this, &Mulberry::saveChanges);
    connect(ui->selectWallpaperPushButton, &QPushButton::clicked, this, [&]{ ui->wallpaper->setText(QFileDialog::getOpenFileName(this, "Choosing the wallpaper", "/home", "Images (*.png *.xpm *.jpg)")); });
    connect(ui->dpmsState, &QCheckBox::toggled, this, [&]{ ui->DPMSValues->setEnabled(ui->dpmsState->isChecked()); });
    connect(ui->autoRepeatState, &QCheckBox::toggled, this, [&]{ ui->autoRepeatValues->setEnabled(ui->autoRepeatState->isChecked()); });
    connect(ui->appComboBox, &QComboBox::currentTextChanged, this,
       [&]{ ui->mimeTypeComboBox->clear(); ui->mimeTypeComboBox->addItems(m_mimeApps[ui->appComboBox->currentIndex()].getMimeTypes());
            ui->defaultMimeTypesList->clear(); ui->defaultMimeTypesList->addItems(m_mimeApps[ui->appComboBox->currentIndex()].getActiveMimeTypes()); });
    connect(ui->setAsDefaultPushButton, &QPushButton::clicked, this,
       [&]{ ui->statusbar->showMessage("Changes will take effect after the save", 1500);
            m_mimeApps[ui->appComboBox->currentIndex()].setMimeTypesAdded(ui->mimeTypeComboBox->currentText().split('|').first().trimmed()); });
    populateFields();
    updateUI();
}

Mulberry::~Mulberry()
{
    delete ui;
}

void Mulberry::updateUI()
{
    ui->DPMSValues->setEnabled(ui->dpmsState->isChecked());
    ui->autoRepeatValues->setEnabled(ui->autoRepeatState->isChecked());
    ui->mimeTypeComboBox->clear();
    ui->mimeTypeComboBox->addItems(m_mimeApps[ui->appComboBox->currentIndex()].getMimeTypes());
}

void Mulberry::populateFields()
{
    ui->tabWidget->setStyleSheet("QTabBar::tab { font-size: 20px; font-style: italic; font-weight: bold; }");
    ui->tabWidget->tabBar()->setExpanding(true);

    ui->statusbar->setStyleSheet("QStatusBar{ border-top: 1px outset black;}");
    ui->statusbar->setContentsMargins(0, 5, 0, 5);

    ui->resolution->insertItems(0, SH("xrandr | awk '{print $1}' | sed -n '/^[0-9]/p' | head -c -1").split('\n'));
    ui->resolution->setStyleSheet("QComboBox { combobox-popup: 0; }");
    ui->resolution->setFont(this->font());

    loadSettings();
    loadMimeApps();
}

void Mulberry::loadSettings()
{
    Setting browser{ "Browser", qgetenv("BROWSER"), "export BROWSER=%1" };
    browser.setGetter([this]{ return ui->browser->text(); });
    browser.setSetter([=, this]{ ui->browser->setText(browser.values().toString()); });
    browser.setChangedCallback([=]{ return browser.values() != browser.getter(); });
    m_settings.push_back(std::move(browser));

    Setting editor{ "Editor", qgetenv("EDITOR"), "export EDITOR=%1" };
    editor.setGetter([this]{ return ui->editor->text(); });
    editor.setSetter([=, this]{ ui->editor->setText(editor.values().toString()); });
    editor.setChangedCallback([=]{ return editor.values() != editor.getter(); });
    m_settings.push_back(std::move(editor));

    Setting visual{ "Visual", qgetenv("VISUAL"), "export VISUAL=%1" };
    visual.setGetter([this]{ return ui->visual->text(); });
    visual.setSetter([=, this]{ ui->visual->setText(visual.values().toString()); });
    visual.setChangedCallback([=]{ return visual.values() != visual.getter(); });
    m_settings.push_back(std::move(visual));

    Setting pager{ "Pager", qgetenv("PAGER"), "export PAGER=%1" };
    pager.setGetter([this]{ return ui->pager->text(); });
    pager.setSetter([=, this]{ ui->pager->setText(pager.values().toString()); });
    pager.setChangedCallback([=]{ return pager.values() != pager.getter(); });
    m_settings.push_back(std::move(pager));

    Setting terminal{ "Terminal", qgetenv("TERMINAL"), "export TERMINAL=%1" };
    terminal.setGetter([this]{ return ui->terminal->text(); });
    terminal.setSetter([=, this]{ ui->terminal->setText(terminal.values().toString()); });
    terminal.setChangedCallback([=]{ return terminal.values() != terminal.getter(); } );
    m_settings.push_back(std::move(terminal));

    QStringList xrandrOutput{ SH("xrandr").split('\n') };

    Setting resolution{ "Resolution", xrandrOutput.filter("*")[0].simplified().section(" ", 0, 0), "xrandr --output " + xrandrOutput.filter("connected")[0].simplified().section(" ", 0, 0) + " --mode %1" };
    resolution.setGetter([this]{ return ui->resolution->currentText(); });
    resolution.setSetter([=, this]{ ui->resolution->setCurrentText(resolution.values().toString()); });
    resolution.setChangedCallback([=]{ return resolution.values() != resolution.getter(); });
    m_settings.push_back(std::move(resolution));

    Setting wallpaper{ "Wallpaper", "", "feh --bg-fill %1" };
    wallpaper.setGetter([this]{ return ui->wallpaper->text(); });
    wallpaper.setChangedCallback([=]{ return wallpaper.values() != wallpaper.getter(); });
    m_settings.push_back(std::move(wallpaper));

    QStringList xsetOutput{ SH("xset q").split('\n') };

    Setting autoRState{ "Auto Repeat State", xsetOutput.filter("auto repeat:")[0].simplified().section(" ", 2, 2) == "on" ? true : false, "xset r %1" };
    autoRState.setGetter([this]{ return ui->autoRepeatState->isChecked() ? "on" : "off"; });
    autoRState.setSetter([=, this]{ ui->autoRepeatState->setChecked(autoRState.values().toBool()); });
    autoRState.setChangedCallback([=, this]{ return autoRState.values() != ui->autoRepeatState->isChecked(); });
    m_settings.push_back(std::move(autoRState));

    QString autoRValuesLine{ xsetOutput.filter("auto repeat delay:")[0].simplified() };
    Setting autoRValues{ "Auto Repeat Values (Delay, Rate)", QStringList{ autoRValuesLine.section(" ", 3, 3), autoRValuesLine.section(" ", 6, 6) }, "xset r rate %1 %2" };
    autoRValues.setGetter([this]{ return QStringList{ ui->autoRepeatDelay->cleanText(), ui->autoRepeatRate->cleanText() }; });
    autoRValues.setSetter([=, this]{ auto list = autoRValues.values().toStringList(); ui->autoRepeatDelay->setValue(list[0].toInt()); ui->autoRepeatRate->setValue(list[1].toInt()); });
    autoRValues.setChangedCallback([=]{ return autoRValues.values() != autoRValues.getter(); });
    m_settings.push_back(std::move(autoRValues));

    Setting dpmsState{ "DPMS State", xsetOutput.filter("DPMS is")[0].simplified().section(" ", 2, 2) == "Enabled" ? true : false, "xset %1dpms" };
    dpmsState.setGetter([this]{ return ui->dpmsState->isChecked() ? "+" : "-"; });
    dpmsState.setSetter( [=, this]{ ui->dpmsState->setChecked(dpmsState.values().toBool()); });
    dpmsState.setChangedCallback([=, this]{ return dpmsState.values() != ui->dpmsState->isChecked(); });
    m_settings.push_back(std::move(dpmsState));

    QString dpmsLine{ xsetOutput.filter("Standby")[0].simplified() };
    Setting dpmsValues{ "DPMS Values (Standby, Suspend, Off)", QStringList{ dpmsLine.section(" ", 1, 1), dpmsLine.section(" ", 3, 3), dpmsLine.section(" ", 5, 5) }, "xset dpms %1 %2 %3" };
    dpmsValues.setGetter([=, this]{ return QStringList{ ui->dpmsStandby->cleanText(), ui->dpmsSuspend->cleanText(), ui->dpmsOff->cleanText() }; });
    dpmsValues.setSetter([=, this]{ auto list = dpmsValues.values().toStringList(); ui->dpmsStandby->setValue(list[0].toInt()); ui->dpmsSuspend->setValue(list[1].toInt()); ui->dpmsOff->setValue(list[2].toInt()); });
    dpmsValues.setChangedCallback([=]{ return dpmsValues.values() != dpmsValues.getter(); });
    m_settings.push_back(std::move(dpmsValues));

    Setting screensaver{ "Screensaver", xsetOutput.filter("timeout:")[0].simplified().section(" ", 1, 1), "xset s %1" };
    screensaver.setGetter([this]{ return ui->screensaverTimeout->cleanText(); });
    screensaver.setSetter([=, this]{ ui->screensaverTimeout->setValue(screensaver.values().toInt()); });
    screensaver.setChangedCallback([=]{ return screensaver.values() != screensaver.getter(); });
    m_settings.push_back(std::move(screensaver));

    QHash<QString, QVariant> lefthandedValues;
    QHash<QString, QVariant> rScrollingValues;
    QHash<QString, QVariant> sensitivityValues;
    QStringList xinputOutput{ SH("xinput").split('\n') };
    for(const auto& device : xinputOutput.filter(QRegularExpression("^⎜.*↳.*")))
    {
        QString xinputFormatted{ device.mid(device.indexOf("↳") + 1, device.indexOf("id=")).simplified() };
        QString name{ xinputFormatted.split("id=").first().trimmed() };
        QString id{ xinputFormatted.split("id=").last().split('=').last().trimmed() };
        QStringList listPropsOutput{ SH("xinput list-props pointer:'" + name + "'").split('\n') };

        if(QStringList line{ listPropsOutput.filter("libinput Accel Speed (") }; !line.isEmpty())
        { ui->mouseSensitivityComboBox->addItem(name); sensitivityValues[name] = line[0].split(':').last().trimmed(); }

        if(QStringList line{ listPropsOutput.filter("libinput Natural Scrolling Enabled (") }; !line.isEmpty())
        { ui->rScrollingComboBox->addItem(name); rScrollingValues[name] = line[0].split(':').last().trimmed(); }

        if(QStringList line{ listPropsOutput.filter("libinput Left Handed Enabled (") }; !line.isEmpty())
        { ui->lefthandedComboBox->addItem(name); lefthandedValues[name] = line[0].split(':').last().trimmed(); }
    }

    Setting lefthanded{ "Left-Handed Enabled", lefthandedValues, "xinput set-prop pointer:'%1' 'libinput Left Handed Enabled' %2" };
    lefthanded.setGetter([this]{ return QStringList{ ui->lefthandedComboBox->currentText(), ui->lefthanded->isChecked() ? "1" : "0" }; });
    lefthanded.setSetter([=, this]{ ui->lefthanded->setChecked(lefthanded.values().toHash()[ui->lefthandedComboBox->currentText()].toBool()); });
    lefthanded.setChangedCallback([=, this]{ return lefthanded.values().toHash()[ui->lefthandedComboBox->currentText()].toBool() != ui->lefthanded->isChecked(); });
    connect(ui->lefthandedComboBox, &QComboBox::currentTextChanged, this, [=, this]{ ui->lefthanded->setChecked(lefthanded.values().toHash()[ui->lefthandedComboBox->currentText()].toBool()); });
    m_settings.push_back(std::move(lefthanded));
    ui->lefthanded->setChecked(lefthanded.values().toHash()[ui->lefthandedComboBox->currentText()].toBool());

    Setting rScrolling{ "Reverse Scrolling Enabled", rScrollingValues, "xinput set-prop pointer:'%1' 'libinput Natural Scrolling Enabled' %2" };
    rScrolling.setGetter([this]{ return QStringList{ ui->rScrollingComboBox->currentText(), ui->rScrolling->isChecked() ? "1" : "0" }; });
    rScrolling.setSetter([=, this]{ ui->rScrolling->setChecked(rScrolling.values().toHash()[ui->rScrollingComboBox->currentText()].toBool()); });
    rScrolling.setChangedCallback([=, this]{ return rScrolling.values().toHash()[ui->rScrollingComboBox->currentText()].toBool() != ui->rScrolling->isChecked(); });
    connect(ui->rScrollingComboBox, &QComboBox::currentTextChanged, this, [=, this]{ ui->rScrolling->setChecked(rScrolling.values().toHash()[ui->rScrollingComboBox->currentText()].toBool()); });
    m_settings.push_back(std::move(rScrolling));
    ui->rScrolling->setChecked(rScrolling.values().toHash()[ui->rScrollingComboBox->currentText()].toBool());

    Setting sensitivity{ "Mouse Sensitivity", sensitivityValues, "xinput set-prop pointer:'%1' 'libinput Accel Speed' %2" };
    sensitivity.setGetter([this]{ return QStringList{ ui->mouseSensitivityComboBox->currentText(), ui->mouseSensitivity->text() }; });
    sensitivity.setSetter([=, this]{ ui->mouseSensitivity->setValue(sensitivity.values().toDouble()); });
    sensitivity.setChangedCallback([=, this]{ return sensitivity.values().toHash()[ui->mouseSensitivityComboBox->currentText()].toDouble() != ui->mouseSensitivity->text().toDouble(); });
    connect(ui->mouseSensitivityComboBox, &QComboBox::currentTextChanged, this, [=, this]{ ui->mouseSensitivity->setValue(sensitivity.values().toHash()[ui->mouseSensitivityComboBox->currentText()].toDouble()); });
    m_settings.push_back(std::move(sensitivity));
    ui->mouseSensitivity->setValue(sensitivity.values().toHash()[ui->mouseSensitivityComboBox->currentText()].toDouble());

    QString xresourcesCmd{ SH("xrdb -n " + QStandardPaths::locate(QStandardPaths::HomeLocation, "/.Xresources")).remove(QRegularExpression("^xrdb:*")) };
    if(!xresourcesCmd.isEmpty())
    {
        QTextStream xresourcesStream{ &xresourcesCmd };
        QString line;
        size_t lineNumber{ 0 };
        while(!xresourcesStream.atEnd())
        {
            line = xresourcesStream.readLine();
            ++lineNumber;
            QStringList values{ line.remove('\t').split(':') };
            Setting resource{ values[0], values, values[0] + ": %1" };
            resource.setGetter([=, this]{ return ui->centralwidget->findChild<QLineEdit*>(resource.values().toStringList().first())->text(); });
            resource.setSetter(
            [=, this]
            {
                auto resName = new QLabel(resource.values().toStringList().first() + ':');
                resName->setFont(this->font());
                resName->setFixedWidth(450);

                auto resValue = new QLineEdit(resource.values().toStringList().last());
                resValue->setObjectName(resource.values().toStringList().first().toUtf8());
                resValue->setFont(this->font());
                resValue->setFixedWidth(200);

                auto resHBox = new QHBoxLayout();
                lineNumber % 2 == 0 ? ui->xresourcesEntriesRight->addLayout(resHBox) : ui->xresourcesEntriesLeft->addLayout(resHBox);

                resHBox->addWidget(resName);
                resHBox->addWidget(resValue);
            });
            resource.setChangedCallback([=]{ return resource.values().toStringList().last() != resource.getter(); });
            m_xresources.push_back(std::move(resource));
        }
    }
    else
    {
        auto xresourcesEmptyLabel = new QLabel("No Xresources entry found");
        xresourcesEmptyLabel->setFont(this->font());
        ui->xresources->layout()->addWidget(xresourcesEmptyLabel);
    }
}

void Mulberry::saveChanges()
{
    auto msg = new QDialog();
    auto resVBox = new QVBoxLayout();
    msg->setLayout(resVBox);
    QList<Setting> settingsChanged;
    QList<Setting> xresourcesChanged;

    for(const auto& setting : m_settings)
    {
        if(setting.isChanged())
        {
            settingsChanged.push_back(setting);
            auto HBox = new QHBoxLayout();
            auto changes = new QLabel();
            auto settingName = new QLabel(setting.name());
            if(setting.values().canConvert<QStringList>())
            {
                auto settingValues = setting.values().toStringList().join(" ");
                changes->setText("  " + (settingValues.isEmpty() ? "" : settingValues + "  ----->  "));
            }
            else
            {
                auto settingValue = setting.values().toString();
                settingValue == "true" ? settingValue = "on" : (settingValue == "false" ? settingValue = "off" : "");
                changes->setText("  " + (settingValue.isEmpty() ? "" : settingValue + "  ----->  "));
            }

            auto currentValue = setting.getter().toStringList();
            (currentValue.last() == "+" || currentValue.last() == "1") ? currentValue.last() = "on" : ((currentValue.last() == "-" || currentValue.last() == "0") ? currentValue.last() = "off" : "");

            changes->setText(changes->text() + currentValue.join(" "));
            changes->setMinimumWidth(300);
            changes->setAlignment(Qt::AlignRight);
            changes->setFont(this->font());changes->setWordWrap(1);
            settingName->setFont(this->font());
            settingName->setWordWrap(1);
            HBox->addWidget(settingName);
            HBox->addWidget(changes);
            HBox->insertStretch(1);
            resVBox->addLayout(HBox);
        }
    }

    for(const auto& resource : m_xresources)
    {
        if(resource.isChanged())
        {
            xresourcesChanged.push_back(resource);
            auto HBox = new QHBoxLayout();
            auto changes = new QLabel("  " + resource.values().toStringList().last() + "  ----->  " + resource.getter().toString());
            auto resourceName = new QLabel(resource.name());

            changes->setMinimumWidth(300);
            changes->setAlignment(Qt::AlignRight);
            changes->setFont(this->font());
            changes->setWordWrap(1);
            resourceName->setFont(this->font());
            resourceName->setWordWrap(1);
            HBox->addWidget(resourceName);
            HBox->addWidget(changes);
            HBox->insertStretch(1);
            resVBox->addLayout(HBox);
        }
    }

    for(const auto& type : m_mimeApps)
    {
        if(!type.getMimeTypesAdded().empty())
        {
            QStringList changedMimeTypes{ type.getMimeTypesAdded() };
            changedMimeTypes.removeDuplicates();

            auto HBox = new QHBoxLayout();
            auto appName = new QLabel(type.getName().toUtf8() + ": " );
            auto changes = new QLabel(changedMimeTypes.join("").trimmed());

            auto icon = new QLabel();
            icon->setPixmap(type.getIcon().pixmap(20, 20));

            changes->setMinimumWidth(300);
            changes->setAlignment(Qt::AlignRight);
            changes->setFont(this->font());
            changes->setWordWrap(1);
            appName->setFont(this->font());
            appName->setWordWrap(1);
            HBox->addWidget(icon);
            HBox->addWidget(appName);
            HBox->addWidget(changes);
            HBox->insertStretch(2);
            resVBox->addLayout(HBox);
        }
    }

    if(resVBox->isEmpty()) { ui->statusbar->showMessage("No changes were made to the configuration", 5000); }
    else
    {
        msg->setWindowTitle("Save Changes");

        auto sourceChangesHBox = new QHBoxLayout();
        auto sourceChanges = new QLabel("Source the config file in xinitrc");
        auto sourceChangesCheckBox = new QCheckBox();
        sourceChangesCheckBox->setObjectName("sourceChangesCheckBox");
        sourceChangesHBox->addWidget(sourceChanges);
        sourceChangesHBox->addWidget(sourceChangesCheckBox);
        sourceChangesHBox->insertStretch(2);
        if(!settingsChanged.empty()) { resVBox->addLayout(sourceChangesHBox); }

        auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, msg);
        resVBox->addWidget(buttons);

        connect(buttons, &QDialogButtonBox::accepted, msg, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, msg, &QDialog::reject);
        connect(msg, &QDialog::accepted, this, [&]
        {
            writeChanges(settingsChanged, xresourcesChanged);
            if(sourceChangesCheckBox->isChecked())
            {
                QFile xinitrc{ QStandardPaths::locate(QStandardPaths::HomeLocation, ".xinitrc") };
                if(!xinitrc.open(QIODevice::ReadOnly)) { std::cerr << "Cannot read xinitrc\n"; return; }
                QTextStream xinitrcIn{ &xinitrc };
                QString xinitrcContent{ xinitrcIn.readAll() };
                xinitrc.close();
                if(!xinitrcContent.contains(". " + QStandardPaths::locate(QStandardPaths::ConfigLocation, "xmulberry/xmulberry")))
                {
                    if(!xinitrc.open(QIODevice::Append)) { std::cerr << "Cannot open xinitrc, check its file permissions\n"; return; }
                    QTextStream xinitrcOut{ &xinitrc };
                    xinitrcOut << ". " + QStandardPaths::locate(QStandardPaths::ConfigLocation, "xmulberry/xmulberry") + '\n';
                    xinitrc.close();
                }
            }
        });
        msg->exec();
    }
}

void Mulberry::loadMimeApps()
{
    for(const auto& appDirsPath : QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation))
    {
        QDir dir{ appDirsPath };
        for(const auto& appFile : dir.entryInfoList(QStringList("*.desktop")))
        {
            QFile file{ appFile.absoluteFilePath() };
            MimeApp app{ QFileInfo(file.fileName()) };
            if(!(app.getMimeTypes().empty() || app.getName().isEmpty())) { m_mimeApps.push_back(app); }
        }
    }
    std::sort(m_mimeApps.begin(), m_mimeApps.end(), [=](const MimeApp& a, const MimeApp& b) { return a.getName() < b.getName(); } );
    for(const auto& app : m_mimeApps) { ui->appComboBox->addItem(app.getIcon(), app.getName()); }
}

void Mulberry::writeChanges(const QList<Setting>& settingsChanged, const QList<Setting>& xresourcesChanged)
{
    QFileInfo filepath{ QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/xmulberry/xmulberry" };
    QDir dir{ filepath.dir() };
    if(!dir.exists()) { dir.mkpath("."); }
    QFile file{ filepath.absoluteFilePath() };

    // Open it in ReadWrite mode so that if the file doesn't exist, it will be created
    if(!file.open(QIODevice::ReadWrite)) { std::cerr << "Cannot open " << filepath.absoluteFilePath().toStdString(); exit(1); }
    QTextStream streamIn{ &file };
    QString mulberryContent{ streamIn.readAll() };
    file.close();

    for(const auto& setting : settingsChanged)
    {
        // If the command exists in the file, replace it
        QString outCmd{ setting.outputCmd() };
        outCmd.replace(QRegularExpression("%\\d"), ".*");
        if(mulberryContent.contains(QRegularExpression(outCmd))) { mulberryContent.replace(QRegularExpression(outCmd), setting.outputCmdFormatted()); }
        else { mulberryContent.append(setting.outputCmdFormatted()); }
    }
    if(!file.open(QIODevice::WriteOnly)) { std::cerr << "Cannot write changes to " << filepath.absoluteFilePath().toStdString(); exit(1); }

    QTextStream streamOut{ &file };
    streamOut << mulberryContent;
    file.close();
    SH(". " + QStandardPaths::locate(QStandardPaths::ConfigLocation, "xmulberry/xmulberry")); // Source the file

    QFile xresourcesFile{ QStandardPaths::locate(QStandardPaths::HomeLocation, "/.Xresources") };
    if(!xresourcesFile.open(QIODevice::ReadOnly)) { std::cout << "Cannot read from " << xresourcesFile.fileName().toStdString(); exit(1); }
    QTextStream xresourcesStreamIn{ &xresourcesFile };
    QString xresourcesContent{ xresourcesStreamIn.readAll() };
    xresourcesFile.close();

    for(const auto& resource : xresourcesChanged)
    {
        QStringList values{ resource.values().toStringList() };
        xresourcesContent.replace(QRegularExpression("^.*" + values.first() + ".*:.*$", QRegularExpression::MultilineOption), values.first() + ": " + resource.getter().toString());
    }

    if(!xresourcesFile.open(QIODevice::WriteOnly)) { std::cout << "Cannot write changes to " << xresourcesFile.fileName().toStdString(); exit(1); }
    QTextStream xresourcesStreamOut{ &xresourcesFile };
    xresourcesStreamOut << xresourcesContent;
    xresourcesFile.close();


    QFile configFile{ QStandardPaths::locate(QStandardPaths::ConfigLocation, "mimeapps.list") };
    if(!configFile.open(QIODevice::ReadWrite)) { std::cerr << "Cannot open " << configFile.fileName().toStdString() << " check its permissions.\n"; exit(1); }

    QSettings configSettings{ configFile.fileName(), QSettings::IniFormat };
    configSettings.beginGroup("Default Applications");

    for(const auto& app : m_mimeApps)
    {
        if(app.getMimeTypesAdded().isEmpty()) continue;

        for(const auto& typeChanged : app.getMimeTypesAdded()) { configSettings.setValue(typeChanged, app.getFilename()); }
    }
    configSettings.endGroup();
    configFile.close();
}
