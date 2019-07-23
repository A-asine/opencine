#ifndef FRAMESERVERVIEW_H
#define FRAMESERVERVIEW_H

#include <QDialog>

namespace Ui
{
    class FrameServerView;
}

class FrameServerView : public QDialog
{
    Q_OBJECT

public:
    explicit FrameServerView(QWidget *parent = 0);
    ~FrameServerView();

private slots:
    void on_fontComboBox_2_activated(const QString &arg1);

    void on_fontComboBox_activated(const QString &arg1);

    void on_fontComboBox_4_activated(const QString &arg1);

    void on_fontComboBox_3_activated(const QString &arg1);

    void on_pushButton_clicked();

private:
    Ui::FrameServerView *ui;
};

#endif // FRAMESERVERVIEW_H
