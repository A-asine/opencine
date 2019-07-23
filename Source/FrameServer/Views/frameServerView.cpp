#include "frameServerView.h"
#include "ui_frameServerView.h"

FrameServerView::FrameServerView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrameServerView)
{
    ui->setupUi(this);
}

FrameServerView::~FrameServerView()
{
    delete ui;
}

void FrameServerView::on_fontComboBox_2_activated(const QString &arg1)
{

}

void FrameServerView::on_fontComboBox_activated(const QString &arg1)
{

}

void FrameServerView::on_fontComboBox_4_activated(const QString &arg1)
{

}

void FrameServerView::on_fontComboBox_3_activated(const QString &arg1)
{

}

void FrameServerView::on_pushButton_clicked()
{

}
