// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / ProcessingTest
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#include "ProcessingPresenter.h"

#include <chrono>
#include <vector>

#include <QFileDialog>
#include <QFileInfo>
#include <QStringList>
#include <QThread>
#include <QTime>
#include <QCoreApplication>

#include <Image/BilinearDebayer.h>
#include <Image/BilinearDebayerOMP.h>

#include <Image/GEDIDebayer.h>
#include <Image/GEDIDebayerOMP.h>

#include "Image/SHOODAKDebayer.h"
#include "Image/SHOODAKDebayerOMP.h"

#include <Image/NearestNeighborScaler.h>

#include "Image/EndianHelper.h"
#include "Image/RawDump.h"
#include <Log/Logger.h>
#include <Memory/StaticAllocator.h>

#include "AVIEncode/AVIContainer.h"

using namespace OC::DataProvider;
using namespace OC::Image;

ProcessingPresenter::ProcessingPresenter(IProcessingView& view) : _currentDebayerProcessor(0), _lastDir(QDir::currentPath())
{
    _view = &view;

    QStringList debayerMethods = {"Bilinear", "GEDI", "SHOODAK", "Bilinear (OMP)", "GEDI (OMP)", "SHOODAK (OMP)", "None"};
    _view->SetAvailableDebayerMethods(debayerMethods);

    _debayerProcessors.push_back(std::make_shared<BilinearDebayer>());
    _debayerProcessors.push_back(std::make_shared<GEDIDebayer>());
    _debayerProcessors.push_back(std::make_shared<SHOODAKDebayer>());
    _debayerProcessors.push_back(std::make_shared<BilinearDebayerOMP>());
    _debayerProcessors.push_back(std::make_shared<GEDIDebayerOMP>());
    _debayerProcessors.push_back(std::make_shared<SHOODAKDebayerOMP>());

    provider.reset(new ImageProvider());

    connect(_view, &IProcessingView::OpenRAWFile, this, &ProcessingPresenter::OpenRAWFile);
    connect(_view, SIGNAL(DebayerMethodChanged(int)), this, SLOT(ChangeDebayerMethod(int)));
    connect(_view, SIGNAL(DumpPNG()), this, SLOT(DumpPNG()));
}

void ProcessingPresenter::Show()
{
    RawPoolAllocator* poolAllocator = new RawPoolAllocator();

    OC_LOG_INFO("Loading File");
    // File format is set to "unknown" to let OC determine it automatically
    provider->Load(_currentFilePath, FileFormat::Unknown, *_image.get(), *poolAllocator);

    OC_LOG_INFO("Loading finished");
    
    auto start = std::chrono::high_resolution_clock::now();
      
    unsigned int frameCount = poolAllocator->GetFrameCount();
    
    AVIContainer avi(_image->Width() / 2, _image->Height() / 2, 30, frameCount);
    int i = 0;
   
    // last frame is skipped to avoid segmentation fault
    for(unsigned int frameNumber = 1; frameNumber < frameCount; frameNumber++)
    {
      _view->EnableRendering(false);
      
      std::string frameLog = "processing Frame No: " + std::to_string(frameNumber); 
      OC_LOG_INFO(frameLog);
      
      provider->ProcessFrame(frameNumber, *_image.get(), *poolAllocator); 
      avi.AddMoviChild(frameNumber, *_image.get());
      
      _view->SetFrame(*_image.get());
      _view->EnableRendering(true);
    
      QTime dieTime= QTime::currentTime().addMSecs(30);
      while (QTime::currentTime() < dieTime)
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
      
      OC_LOG_INFO("Processing finished");
    }
    
    avi.AddMoviChild(frameCount + 1, *_image.get());   // to call AviBuild  
}

// TODO: Check how to remove allFormats, as it is used as workaround for filter selection in QFileDialog
void ProcessingPresenter::CreateFileFilters(std::unordered_map<std::string, std::string> filterList,
                                            std::string& concatenatedFormats, std::string& allFormats)
{
    allFormats = "All suppported formats(";
    for(auto& format : filterList)
    {
        allFormats += ' ' + format.second;
    }
    allFormats += ')';

    for(auto& format : filterList)
    {
        concatenatedFormats += format.first + '(' + format.second + ");;";
    }
    concatenatedFormats += allFormats;
}

void ProcessingPresenter::OpenRAWFile()
{
    _view->EnableRendering(false);

    std::unordered_map<std::string, std::string> supportedFormatsList = {{"DNG Files", "*.dng *.DNG"},
                                                                         {"MagicLantern files", "*.mlv *.MLV"}};

    std::string formats, allFormats;
    CreateFileFilters(supportedFormatsList, formats, allFormats);

    QString fileName =
        QFileDialog::getOpenFileName(_view, tr("Open Image"), _lastDir, formats.c_str(), new QString(allFormats.c_str()));

    _currentFilePath = fileName.toStdString();
    _lastDir = QFileInfo(fileName).path();

    Show();

    //_view->EnableRendering(true);
}

void ProcessingPresenter::ChangeDebayerMethod(unsigned int debayerMethod)
{
    _currentDebayerProcessor = debayerMethod;
    Show();
}

void ProcessingPresenter::DumpPNG()
{
    _view->EnableRendering(false);
    QString fileName = QFileDialog::getSaveFileName(_view, tr("Save PNG"), QDir::currentPath(), tr("PNG Files(*.png *.PNG)"));

    OC::Image::RawDump::DumpPNG(fileName.toStdString(), *_image.get());

    _view->EnableRendering(true);
}
