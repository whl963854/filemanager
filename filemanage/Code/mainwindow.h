#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QStandardItemModel>
#include<QTimer>
#include<QFileInfoList>
#include<QAction>
#include<QMenu>
#include <QAbstractNativeEventFilter>
#include<windows.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow,public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void updateFile();
    void startActSlot();//菜单项槽函数
    void showMemu(QPoint p);
    void uploadSlot();
    void deleteActSlot();
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) override;
private:
    Ui::MainWindow *ui;
    QString m_strDataPath;
    QStandardItemModel m_model;
    QTimer m_timer;
    QAction *startAct;
    QAction *deleteAct;
    QMenu *tableviewMenu;
    ATOM m_HTK_RUN;

    bool searchName(QFileInfoList fileInfoList);
    void init();
    bool nativeEvent(const QByteArray &eventType, void *message, long *result)override;
    void beautify();
};
#endif // MAINWINDOW_H
