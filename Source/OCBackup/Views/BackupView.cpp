// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / OCBackup
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#include "BackupView.h"
#include "ui_BackupView.h"

#include <QFileSystemModel>
#include <QtQuick/QQuickItem>
#include <QDebug>

#include "Presenters/BackupPresenter.h"

#include <Helper/QMLThumbnailProvider.h>

#include "../Data/PathInfoQML.h"
#include "../Data/ItemInfoQML.h"

BackupView::BackupView(/*QWidget *parent,*/ IBackupPresenter* presenter) :
    ui(new Ui::BackupView)
{
    ui->setupUi(this);

    SetupDriveView();
    SetupFolderView();
    SetupThumbnailView();
    SetupDestinationsView();

    connect(ui->destinationsControl, SIGNAL(AddDestinationClicked()), this, SIGNAL(AddDestinationClicked()));
}

BackupView::~BackupView()
{
    delete ui;
}

void BackupView::SetupDriveView()
{
    _qmlContext = ui->driveListControl->rootContext();
    _qmlContext->setContextProperty("listModel", QVariant::fromValue(dataList));

    ui->driveListControl->setSource(QUrl("./Widgets/DriveList.qml"));

    QQuickItem* item = ui->driveListControl->rootObject();
    connect(item, SIGNAL(itemSelectionChanged(int)), this, SLOT(CurrentDriveChanged(int)));
}

void BackupView::SetupFolderView()
{
    _folderTreeModel = std::make_shared<QFileSystemModel>();
    ui->folderTreeControl->setModel(_folderTreeModel.get());

    connect(ui->folderTreeControl->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this,      SLOT(CurrentFolderChanged(const QItemSelection&, const QItemSelection&)));
}

void ListQMLMethods(QQuickItem* item)
{
    const QMetaObject* metaObj = item->metaObject();
    for (int i = 0; i < metaObj->methodCount(); ++i)
    {
        QMetaMethod method = metaObj->method(i);
        qDebug() << method.methodSignature();
    }
}

//QList<QObject*>*  _fileList;
void BackupView::SetupThumbnailView()
{
    QQmlEngine* engine = ui->thumbnailViewControl->engine();
    engine->addImageProvider(QLatin1String("OCthumb"), new QMLThumbnailProvider());

    _fileList = new QList<QObject*>();
    _qmlContext = ui->thumbnailViewControl->rootContext();
    _qmlContext->setContextProperty("fileList", QVariant::fromValue(*_fileList));

    ui->thumbnailViewControl->setSource(QUrl("./Widgets/ThumbnailView.qml"));

    connect(ui->thumbnailViewControl->rootObject(), SIGNAL(playClip(int)), this, SLOT(PlayClip(int)));
}

void BackupView::SetupDestinationsView()
{
    _fileList = new QList<QObject*>();
}

void BackupView::TransferButtonClicked()
{
    //TODO: Rework create a task directly, later it can be pushed through an event bus
    emit StartTransfer();
}

void BackupView::CurrentDriveChanged(int currentDrive)
{
    emit DriveSelectionChanged(currentDrive);
}

void BackupView::CurrentFolderChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    QModelIndex itemIndex = ui->folderTreeControl->selectionModel()->currentIndex();
    QString path = _folderTreeModel->filePath(itemIndex);

    emit FolderSelectionChanged(path);
}

void BackupView::SetDriveList(std::vector<PathInfo> driveList)
{
    dataList.clear();

    for (auto& drive : driveList)
    {
        dataList.append(new PathInfoQML("", QString::fromStdString(drive.DrivePath), QString::fromStdString(drive.DriveName), drive.UsedSpace, drive.TotalSpace, QString::fromStdString(drive.SpaceUnit)));
    }

    _qmlContext = ui->driveListControl->rootContext();
    _qmlContext->setContextProperty("listModel", QVariant::fromValue(dataList));
}



void BackupView::SetItemList(std::vector<FileInfo> fileList)
{
    _fileList->clear();

    for (auto& fileInfo : fileList)
    {
        // TODO: supply path as first parameter, so it and file name are distinct values
        QString folderPath = fileInfo.GetFolderPath() + "/";
        _fileList->append(new ItemInfoQML(folderPath, fileInfo.GetFileName(), 640, 480, 30));
    }

    _qmlContext = ui->thumbnailViewControl->rootContext();
    _qmlContext->setContextProperty("fileList", QVariant::fromValue(*_fileList));
}

void BackupView::SetDestinationList(std::vector<PathInfo> destinationList)
{
    _destinationList.clear();

    for (auto& destination : destinationList)
    {
        _destinationList.push_back(new PathInfoQML(QString::fromStdString(destination.RelativePath), QString::fromStdString(destination.DrivePath), QString::fromStdString(destination.DriveName), 640, 480, "30"));
    }

    ui->destinationsControl->SetDestinationList(_destinationList);
}

void BackupView::OpenClip(QString clipPath)
{
    ui->widget_2->LoadFile(clipPath);
}

void BackupView::SetCurrentFolder(QString folderPath)
{
    if (folderPath == "")
    {
        ui->folderTreeControl->setModel(nullptr);
        return;
    }

    _folderTreeModel->setRootPath(QDir::currentPath());
    _folderTreeModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::AllDirs);

    ui->folderTreeControl->setModel(_folderTreeModel.get());
    ui->folderTreeControl->setRootIndex(_folderTreeModel->index(folderPath));
}

void BackupView::PlayClip(int clipIndex)
{
    emit LoadClip(clipIndex);
    //qDebug() << "Clip selected: " << QString::number(clipIndex);

}
