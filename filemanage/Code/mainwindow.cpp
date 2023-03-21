#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<qdir.h>
#include<QDebug>
#include<QDateTime>
#include<QFileDialog>
#include<QMessageBox>
#include<QStandardPaths>
#include<QList>
#include<QFileIconProvider>
#include<QFileIconProvider>
#include<QTableWidgetItem>
#include<QDesktopServices>
#include<QUrl>
#include<QKeyEvent>
#include<QScrollBar>
#include<windows.h>

#include"utils.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    init();
}

MainWindow::~MainWindow()
{
    UnregisterHotKey(HWND(this->winId()), m_HTK_RUN);

    GlobalDeleteAtom(m_HTK_RUN);
    delete ui;
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    if(eventType == "windows_generic_MSG")
    {

        MSG* pMsg = reinterpret_cast<MSG*>(message);
        if(pMsg->message == WM_HOTKEY)
        {
            if(this->isVisible())
            {
                qDebug()<<"hide";
                this->hide();
            }
            else
            {
                qDebug()<<"show";
                this->show();
            }
        }
    }
    return QWidget::nativeEvent(eventType,message,result);
}

void MainWindow::beautify()
{
    ui->tableView->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{"        //垂直滑块整体
                                                              "background:#FFFFFF;"  //背景色
                                                              "padding-top:20px;"    //上预留位置（放置向上箭头）
                                                              "padding-bottom:20px;" //下预留位置（放置向下箭头）
                                                              "padding-left:3px;"    //左预留位置（美观）
                                                              "padding-right:3px;"   //右预留位置（美观）
                                                              "border-left:1px solid #d7d7d7;}"//左分割线
                                                              "QScrollBar::handle:vertical{"//滑块样式
                                                              "background:#dbdbdb;"  //滑块颜色
                                                              "border-radius:6px;"   //边角圆润
                                                              "min-height:80px;}"    //滑块最小高度
                                                              "QScrollBar::handle:vertical:hover{"//鼠标触及滑块样式
                                                              "background:#d0d0d0;}" //滑块颜色
                                                              "QScrollBar::add-line:vertical{"//向下箭头样式
                                                              "QScrollBar::sub-line:vertical{"//向上箭头样式
);

}

void MainWindow::init()
{

    ui->setupUi(this);

    m_HTK_RUN = GlobalAddAtomA("HTK_RUN");
    RegisterHotKey(HWND(this->winId()), m_HTK_RUN, MOD_ALT, VK_SPACE);	// alt+space

    m_strDataPath=QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QDir d(m_strDataPath);
    if(!d.exists())
    {
        d.mkdir(m_strDataPath);
    }

    updateFile();
    beautify();

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableviewMenu = new QMenu(ui->tableView);
    startAct = new QAction("启动",ui->tableView);
    deleteAct = new QAction("删除",ui->tableView);
    tableviewMenu->addAction(startAct);
    tableviewMenu->addAction(deleteAct);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(startAct,&QAction::triggered,this,&MainWindow::startActSlot);
    connect(deleteAct,&QAction::triggered,this,&MainWindow::deleteActSlot);
    connect(ui->tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showMemu(QPoint)));
    connect(ui->lineEdit,&QLineEdit::textChanged,this,&MainWindow::updateFile);
    connect(ui->btnFresh,&QPushButton::clicked,this,&MainWindow::updateFile);
    connect(ui->btnDelete,&QPushButton::clicked,this,&MainWindow::deleteActSlot);
    connect(ui->btnUpload,&QPushButton::clicked,this,&MainWindow::uploadSlot);

}

void MainWindow::updateFile()
{
    QDir dir(m_strDataPath);
    QStringList lFiter;
    QFileInfoList lFileInfo =  dir.entryInfoList();
    m_model.clear();
    QString strFilter = ui->lineEdit->text();
    QList<QStandardItem*> lRow;
    for(int i=0;i<lFileInfo.size();i++)
    {
        lRow.clear();
        QFileInfo &info = lFileInfo[i];
        auto str = info.completeBaseName();
        if((!Utils::matchLetters(Utils::GetPinyins(str),strFilter)&&strFilter!="")||info.baseName()=="")
        {
            continue;
        }
        QStandardItem *p1=new QStandardItem(info.baseName()+"."+info.suffix());
        QStandardItem *p2=new QStandardItem(info.absoluteFilePath());
        QStandardItem *p3=new QStandardItem(info.birthTime().toString("yyyy/MM/dd hh:mm:ss"));
        QStandardItem *p4;
        if (!info.isFile())
        {
            p4=new QStandardItem("");
        }
        else
        {
            p4=new QStandardItem(QString::number(info.size()/1024)+"KB");
        }
        lRow<<p1<<p2<<p3<<p4;
        QFileIconProvider iconProvider;
        QIcon icon = iconProvider.icon(info.absoluteFilePath());
        p1->setIcon(icon);
        m_model.appendRow(lRow);

    }
    QStringList lHeaders{"文件名","绝对路径","创建日期","文件大小"};
    m_model.setHorizontalHeaderLabels(lHeaders);
    ui->tableView->setModel(&m_model);
}

void MainWindow::startActSlot()
{
    auto modelIndex=ui->tableView->currentIndex();
    if(!modelIndex.isValid() ||  modelIndex.row() >=m_model.rowCount() || m_model.rowCount()==0)
    {
        return;
    }
    QString str=m_model.data(modelIndex,0).toString();
    QString startPath=m_strDataPath+"/"+str;
    QFile file(startPath);
    QDesktopServices::openUrl(QUrl::fromLocalFile(startPath));
}

void MainWindow::deleteActSlot()
{
    auto ret=QMessageBox::question(nullptr,"删除","确定删除吗,此操作无法撤销！",QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(QMessageBox::No == ret)
        return;
    auto modelIndex=ui->tableView->currentIndex();
    if(!modelIndex.isValid() ||  modelIndex.row() >=m_model.rowCount() || m_model.rowCount()==0)
    {
        return;
    }
    int index=modelIndex.row();
    auto ans=m_model.takeRow(index).first();
    QString str=ans[0].text();
    QString deletePath=m_strDataPath+"/"+str;
    QFile file(deletePath);
    if (!file.exists())
    {
        QMessageBox::warning(nullptr,"Failed","文件不存在");
        return ;
    }
    if(!file.remove())
    {
        QMessageBox::warning(nullptr,"Failed","文件删除失败");
        return ;
    }
}

bool MainWindow::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    if (eventType == "windows_generic_MSG")
    {
        MSG  *msg = (MSG*)message;
        if (msg->message == WM_HOTKEY && msg->wParam == 100 )
        {
            if(this->isVisible())
            {
                qDebug()<<"hide";
                this->hide();
            }
            else
            {
                qDebug()<<"show";
                this->show();
            }
        }
        return true;
    }
    return false;
}

void MainWindow::showMemu(QPoint p)
{
    QModelIndex index = ui->tableView->indexAt(p);//获取鼠标点击位置项的索引
    if(index.isValid())//数据项是否有效，空白处点击无菜单
    {
        QItemSelectionModel* selections = ui->tableView->selectionModel();
        QModelIndexList selected = selections->selectedIndexes();
        if(selected.count() ==1)
        {
            startAct->setVisible(true);
        }
        else
        {
            deleteAct->setVisible(true);
        }
        tableviewMenu->exec(QCursor::pos());
    }
}

void MainWindow::uploadSlot()
{
    auto strPath=QFileDialog::getOpenFileName(nullptr,"文件上传",QDir::homePath(),"*.*");
    if(strPath.isEmpty())
    {
        return;
    }
    auto uploadPath = m_strDataPath+"/"+strPath.section("/",-1);
    if(QFile::exists(uploadPath))
    {
        QMessageBox::warning(nullptr,"warning","文件已经存在");
        return;
    }
    bool ret= QFile::copy(strPath,uploadPath);
    QMessageBox::warning(nullptr,"warning",ret?"success":"fail");
    updateFile();
}


