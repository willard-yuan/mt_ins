#pragma execution_character_set("utf-8")

#include <QDesktopWidget>
#include <QStringListModel>
#include <QListWidget>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QtWidgets>
#include <QtDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"

#include <algorithm>
#include <QDir>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    // 调用窗口初始化
    this->initForm();
    this->initModel();
    ui->setupUi(this);

    //创建菜单
    createActions();
    createMenus();

    // 设置窗口尺寸
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();  //屏幕区域
    int screenW = screenRect.width();
    int screenH = screenRect.height();
    this->resize(QSize(int(0.64*screenW), int(0.82*screenH)));
    this->resize(QSize(int(0.732*screenW), int(0.82*screenH)));

    // 启动是窗口放正中间
    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint center = desktopRect.center();
    move(int(center.x()-width()*0.5), int(center.y()-height()*0.5));


    ui->scrlArea->setContentsMargins(0, 0, 0, 0);
    ui->scrlArea->setAlignment(Qt::AlignCenter);
    ui->scrlArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrlArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrlArea->setFrameStyle(0);
    ui->scrlArea->setWidgetResizable(true);

    imgs_listeWidget = new QListWidget;
    imgs_listeWidget->setViewMode(QListWidget::IconMode);
    imgs_listeWidget->setIconSize(QSize(260, 220)); // 设置Icon大小
    imgs_listeWidget->setResizeMode(QListWidget::Adjust);
    imgs_listeWidget->setStyleSheet("QListWidget::item { width: 255; height: 260; font-size: 8pt; border-width: 5px 1px 5px 1px; padding-left: 8px; padding-right: 8px;};"
                                    "QListView::item::text { font-size: 8pt; border-style: dot-dash; border-width: 5px 1px 5px 1px; border-color: yellow; }");
    imgs_listeWidget->setMovement(QListView::Static);

    // 设置最小化、最大化以及关闭按钮
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    if(!this->isActiveWindow()){
        this->setWindowFlags((windowFlags() & Qt::WindowStaysOnTopHint));
        this->setWindowFlags((windowFlags() & ~Qt::WindowStaysOnTopHint));
        this->show();
    }
    ui->scrlArea->setLineWidth(3);


    // 设置
    topK = 30;
    qeTopK = 1;

    QString OutputFolder = "C:/tmpData/";
    if(!QDir(OutputFolder).exists())
    {
        QDir().mkdir(OutputFolder);
    }

    featsPath = OutputFolder.toStdString() + featsPath;

    // 设置中文解码
    code = QTextCodec::codecForName("GB2312");
}

// 初始化窗口样式、皮肤
void MainWindow::initForm()
{
    QString qss;
    QFile qssFile(":/qss/dev.css");

    qssFile.open(QFile::ReadOnly | QFile::Text);
    if(qssFile.isOpen()){
        //qss = QLatin1String(qssFile.readAll());
        qss = qssFile.readAll();
        qApp->setStyleSheet(qss);
        qssFile.close();        
    } else {
        qDebug() << "open qss failed";
    }
}

MainWindow::~MainWindow()
{
    code = nullptr;
    delete p_calculator;
    delete ui;
}

void MainWindow::initModel()
{
    std::string modelBin = modelBinPath;

    if (p_calculator == nullptr) {
        p_calculator = new CnnFeature(inputSize_, blobName_, modelBin);
    }
}


// 创建菜单
void MainWindow::createMenus()
{

    aboutMenu = menuBar()->addMenu(QString("菜单"));
    aboutMenu->setMinimumWidth(160);
    //aboutMenu->setFixedWidth(120);
    aboutMenu->addAction(settingAction);
    aboutMenu->addAction(aboutAction);

    helpMenu = menuBar()->addMenu(QString("帮助"));
    helpMenu->setMinimumWidth(160);
    //helpMenu->setFixedWidth(120);
    helpMenu->addAction(aboutAuthorAction);
}

void MainWindow::createActions()
{
    aboutAuthorAction = new QAction(QString("关于"), this);
    connect(aboutAuthorAction, &QAction::triggered, this, &MainWindow::aboutAuthor);

    aboutAction = new QAction(QString("退出"), this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::quite);

    settingAction = new QAction(QString("设置"), this);

    //connect(settingAction, &QAction::triggered, this, &MainWindow::setParams);
    connect(settingAction, SIGNAL(triggered()), this, SLOT(setParams()));
}

//  关于软件介绍
void MainWindow::quite()
{
    QApplication::quit();
}

// 关于开发者介绍
void MainWindow::aboutAuthor()
{
    QMessageBox::about(this, QString("关于"), QString("The Software is developed by YongYuan. More details access to yongyuan.name. Version @2023."));
}

//  参数设置
void MainWindow::setParams()
{
    paramsDlog = new Params(this);
    paramsDlog->setModal(true);

    // 设置窗口尺寸
    QSize size = this->size();
    paramsDlog->resize(QSize(int(size.width()*0.5), int(size.height()*0.5)));

    // 启动是窗口放正中间
    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint center = desktopRect.center();
    move(int(center.x()-width()*0.5), int(center.y()-height()*0.5));

    paramsDlog->show();

    /* The user clicked Ok */
    if (paramsDlog->exec() ==  Params::Accepted) {
        topK = paramsDlog->topK;
        qeTopK = paramsDlog->qeTopK;
    }
}

string MainWindow::UTF8ToGBK(const char* strUTF8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len+1];
    memset(wszGBK, 0, len*2+2);
    MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len+1];
    memset(szGBK, 0, len+1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    string strTemp(szGBK);
    if(wszGBK) delete[] wszGBK;
    if(szGBK) delete[] szGBK;
    return strTemp;
}

string MainWindow::GBKToUTF8(const char* strGBK)
{
    int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len+1];
    memset(wstr, 0, len+1);
    MultiByteToWideChar(CP_ACP, 0, strGBK, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len+1];
    memset(str, 0, len+1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    string strTemp = str;
    if(wstr) delete[] wstr;
    if(str) delete[] str;
    return strTemp;
}

// 提取图库的特征
void MainWindow::on_extractFeatsButton_clicked(bool checked)
{
    int count = 0;
    int countGood = 0;
    if(checked) {
        std::vector<std::pair<std::string, std::vector<float>>> feats;
        std::vector<float> feat;

        progBar = new QProgressDialog(tr("索引构建进度"), tr("取消"), 1, img_names.size(), this);
        QList<QPushButton *> L = progBar->findChildren<QPushButton *>();
        L.at(0)->hide();
        //L.at(0)->show();
        progBar->setWindowTitle(tr("进度"));

        int step = int(img_names.size()/100);
        for(int i = 0; i < img_names.size(); i++)
        {
            std::string tmpFileName = code->fromUnicode(img_names.at(i)).data();
            std::string tmpDir = code->fromUnicode(db_root_dir).data();
            std::string tmpImgfullPath = tmpDir + '/' + tmpFileName;

            cv::Mat img_color;
            cv::Mat img_gray = cv::imread(tmpImgfullPath, 0);
            if(img_gray.empty()) continue;
            cv::cvtColor(img_gray, img_color, cv::COLOR_GRAY2BGR);

            // 如果采用彩色图像模式
            if(ui->colorModelCheckBox->isChecked()) {
                img_color = cv::imread(tmpImgfullPath, 1);
            }

            ++count;

            progBar->setValue(i+1);
            QCoreApplication::processEvents();

            if(img_color.empty()) continue;
            p_calculator->computeFeat(img_color, feat);
            feats.push_back(std::pair<std::string, std::vector<float>>(GBKToUTF8(tmpImgfullPath.c_str()), feat)); // 防止张文乱码，非常重要
            ++countGood;
        }

        delete progBar;

        std::ofstream fp;
        fp.open(featsPath.c_str(), std::ios::out|std::ios::binary);

        fp.write((char *)(&countGood), sizeof(int));
        for(int i = 0; i < countGood; i++)
        {
            int imgNameSize = (int)feats.at(i).first.size();
            fp.write(reinterpret_cast<const char *>(&imgNameSize), sizeof(int));
            fp.write((const char *)feats.at(i).first.c_str(), feats.at(i).first.size());
            fp.write((const char *)(&feats.at(i).second[0]), dimFeat*sizeof(float));
        }
        fp.close();

        if(count == img_names.size() && count > 0)
        {
            QMessageBox::information(nullptr, "提示", "图片特征提取完毕", QMessageBox::Yes);
        }
    } else {
        QMessageBox::information(nullptr, "提示", "请选择图库后，再构建索引", QMessageBox::Yes);
    }
}

void MainWindow::on_loadIndexButton_clicked()
{
    index_file_path = QFileDialog::getOpenFileName(this, tr("选择索引"), "", tr("all Files (*.data)"));
    std::string tmpFileName = code->fromUnicode(index_file_path).data();
    if ( !index_file_path.isNull() )
    {
        float* feat = new float[dimFeat];
        std::ifstream fp;
        fp.open(tmpFileName, std::ios::in|std::ios::binary);

        if (!fp.good()) {
            QMessageBox::information(nullptr, "提示", "索引文件不存在，请对图库先建立索引", QMessageBox::Yes);
        }
        int countTmp = 0;
        int countImgs = 0;
        fp.read((char *)&countImgs, sizeof(int));
        featsMat = cv::Mat((int)countImgs, dimFeat, CV_32FC1);

        for(int i = 0; i < countImgs; i++)
        {
            int imgNameSize = 0;
            char imgName[1024] = {""};
            fp.read((char *)&imgNameSize, sizeof(int));
            fp.read(imgName, imgNameSize);
            std::string idNameStr(imgName);
            fp.read(reinterpret_cast<char *>(feat), dimFeat*sizeof(float));
            for(int j = 0; j < dimFeat; ++j)
            {
                featsMat.at<float>(i, j) = feat[j];
            }

            imgNamesDB.push_back(idNameStr); // 防止中文乱码，非常重要
            ++countTmp;
        }
        delete [] feat;

        fp.close(); // 关闭

        if(countTmp == countImgs && countImgs > 0)
        {
            QMessageBox::information(nullptr, "提示", "索引载入成功", QMessageBox::Yes);
        }

    }
}

void MainWindow::on_imgsOpenButton_clicked()
{
    db_root_dir = QFileDialog::getExistingDirectory(this, tr("打开文件夹"), "/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if ( !db_root_dir.isNull() )
    {
        QMessageBox::information(nullptr, tr("Path"), tr("你选择了") + db_root_dir);
        QDir export_folder(db_root_dir);
        export_folder.setNameFilters(QStringList()<<"*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG" << "*.PNG" << "*.png" << "*.bmp" << "*.BMP");
        img_names = export_folder.entryList();
        QStringListModel *model = new QStringListModel(this);
        model->setStringList(img_names);
        ui->listView->setModel(model);
    }else{
        QMessageBox::information(nullptr, "提示", "初始使用需要对图库建立索引，请选择图库", QMessageBox::Yes);
    }
}

// 点击搜索按钮触发的事件
void MainWindow::on_queryButton_clicked()
{

    if(featsMat.empty())
    {
        QMessageBox::information(nullptr, "提示", "请先载入索引文件再搜索", QMessageBox::Yes);
        return;
    }

    // 必须先释放，否则按钮会导致多次触发
    disconnect(imgs_listeWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                                  this, SLOT(on_listeWidget_clicked(QListWidgetItem *)));
    imgs_listeWidget->clear();
    //QString query_img_path = QFileDialog::getOpenFileName(this, tr("选择图像"), QString());
    QString query_img_path = QFileDialog::getOpenFileName(this, tr("选择图像"), "", tr("Image Files (*.jpg *.jpeg *.JPG *.JPEG *.png *.PNG *.bmp *.BMP)"));

    if ( !query_img_path.isNull() )
    {
        std::string tmpFileName = code->fromUnicode(query_img_path).data(); // 中文名字支持

        cv::Mat img_color_show = cv::imread(tmpFileName, 1);

        if(img_color_show.empty()) {
            QMessageBox::information(nullptr, "提示", "查询图片是损坏的图片，请用别的图片查询", QMessageBox::Yes);
            return;
        }

        cv::Mat img_color;
        cv::Mat img_gray = cv::imread(tmpFileName, 0);

        if(img_gray.empty()) {
            QMessageBox::information(nullptr, "提示", "查询图片是损坏的图片，请用别的图片查询", QMessageBox::Yes);
            return;
        }

        cv::cvtColor(img_gray, img_color, cv::COLOR_GRAY2BGR);

        if(ui->colorModelCheckBox->isChecked()) {
            img_color = img_color_show;
        }

        std::vector<float> feat;
        p_calculator->computeFeat(img_color, feat);

        cv::Mat queryMat(1, dimFeat, CV_32FC1);
        for(int i= 0; i < dimFeat; ++i)
        {
            queryMat.at<float>(0, i) = feat.at(i);
        }

        cv::Mat scoresMat = queryMat*featsMat.t();
        cv::Mat indices;
        cv::sortIdx(scoresMat, indices, cv::SORT_EVERY_ROW + cv::SORT_DESCENDING);
        cv::Mat sortScoresMat;
        cv::sort(scoresMat, sortScoresMat, cv::SORT_EVERY_ROW + cv::SORT_DESCENDING);

        //std::cout << sortScoresMat << std::endl;

        // 拓展重排
        if(ui->checkBox->isChecked()) {
            for(int i = 0; i < qeTopK; i ++) {
                int idx = indices.at<int>(0, i);
                cv::Mat tmpMat = featsMat.row(idx);
                queryMat.push_back(tmpMat);
            }
            cv::reduce(queryMat, queryMat, 0, cv::REDUCE_AVG);
            scoresMat = queryMat*featsMat.t();
            cv::sortIdx(scoresMat, indices, cv::SORT_EVERY_ROW + cv::SORT_DESCENDING);
            cv::sort(scoresMat, sortScoresMat, cv::SORT_EVERY_ROW + cv::SORT_DESCENDING);
        }

        QVector<QString> results;
        int tmpNum = (int)std::min((double)topK, (double)imgNamesDB.size());
        for(int i = 0; i < tmpNum; i++){
            int idx = indices.at<int>(0, i);
            QString resultStr = QString::fromStdString(imgNamesDB.at(idx));
            results.push_back(resultStr);
        }

        scores.clear();
        for (int i = 0 ; i != results.size() ; i++) {
            QString tmpImgName = results[i];
            // qDebug() << tmpImgName;
            QFileInfo tmpfileInfo(tmpImgName);
            // 检查文件是否存在
            if(QFileInfo(tmpfileInfo.filePath()).exists()) {
                imgs_listeWidget->addItem(new QListWidgetItem(QIcon(tmpImgName), tmpfileInfo.filePath()));
                scores.push_back(sortScoresMat.at<float>(0,i));
            } else {
                continue;
            }
        }

        ui->previewImg->setPixmap(QPixmap::fromImage(Helper::mat2qimage(img_color_show)).scaled(310, 360));
        ui->scrlArea->setWidget(imgs_listeWidget);

        connect(imgs_listeWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                this, SLOT(on_listeWidget_clicked(QListWidgetItem *)));

    } else {
        QMessageBox::information(nullptr, "提示", "未选取查询图片，请选择一张图片", QMessageBox::Yes);
    }
}

// 右键菜单添加搜索功能
void MainWindow::searchSimilarImgs()
{
    // 必须先释放，否则按钮会导致多次触发
    disconnect(imgs_listeWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                                  this, SLOT(on_listeWidget_clicked(QListWidgetItem *)));
    imgs_listeWidget->clear();

    if(featsMat.empty())
    {
        QMessageBox::information(nullptr, "提示", "请先载入索引文件再搜索", QMessageBox::Yes);
        return;
    }

    std::string tmpFileName = code->fromUnicode(img_name_selected_inlist).data(); // 中文名字支持
    cv::Mat img_color_show = cv::imread(tmpFileName, 1);
    if(img_color_show.empty()) {
        QMessageBox::information(nullptr, "提示", "查询图片是损坏的图片，请用别的图片查询", QMessageBox::Yes);
        return;
    }

    cv::Mat img_color;
    cv::Mat img_gray = cv::imread(tmpFileName, 0);
    if(img_gray.empty()) {
        QMessageBox::information(nullptr, "提示", "查询图片是损坏的图片，请用别的图片查询", QMessageBox::Yes);
        return;
    }
    cv::cvtColor(img_gray, img_color, cv::COLOR_GRAY2BGR);

    // 如果采用彩色图像模式
    if(ui->colorModelCheckBox->isChecked()) {
        img_color = img_color_show;
    }

    ui->previewImg->setPixmap(QPixmap::fromImage(Helper::mat2qimage(img_color_show)).scaled(310, 360));

    std::vector<float> feat;
    p_calculator->computeFeat(img_color, feat);

    cv::Mat queryMat(1, dimFeat, CV_32FC1);
    for(int i= 0; i < dimFeat; ++i)
    {
        queryMat.at<float>(0, i) = feat.at(i);
    }

    cv::Mat scoresMat = queryMat*featsMat.t();
    cv::Mat indices;
    cv::sortIdx(scoresMat, indices, cv::SORT_EVERY_ROW + cv::SORT_DESCENDING);
    cv::Mat sortScoresMat;
    cv::sort(scoresMat, sortScoresMat, cv::SORT_EVERY_ROW + cv::SORT_DESCENDING);

    // 重排
    if(ui->checkBox->isChecked()) {
        for(int i = 0; i < qeTopK; i ++) {
            int idx = indices.at<int>(0, i);
            cv::Mat tmpMat = featsMat.row(idx);
            queryMat.push_back(tmpMat);
        }
        cv::reduce(queryMat, queryMat, 0, cv::REDUCE_AVG);
        scoresMat = queryMat*featsMat.t();
        cv::sortIdx(scoresMat, indices, cv::SORT_EVERY_ROW + cv::SORT_DESCENDING);
        cv::sort(scoresMat, sortScoresMat, cv::SORT_EVERY_ROW + cv::SORT_DESCENDING);
    }

    QVector<QString> results;
    int tmpNum = (int)std::min((double)topK, (double)imgNamesDB.size());
    for(int i = 0; i < tmpNum; i++)
    {
        int idx = indices.at<int>(0, i);
        QString resultStr = QString::fromStdString(imgNamesDB.at(idx));
        results.push_back(resultStr);
    }

    for (int i = 0 ; i != results.size() ; i++)
    {
        QString tmpImgName = results[i];
        QFileInfo tmpfileInfo(tmpImgName);
        //imgs_listeWidget->addItem(new QListWidgetItem(QIcon(tmpImgName), tmpfileInfo.completeBaseName()));
        // 检测文件是否存在
        if(QFileInfo(tmpfileInfo.filePath()).exists())
        {
            imgs_listeWidget->addItem(new QListWidgetItem(QIcon(tmpImgName), tmpfileInfo.filePath()));
        } else {
            continue;
        }
    }

    ui->scrlArea->setWidget(imgs_listeWidget);

    connect(imgs_listeWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(on_listeWidget_clicked(QListWidgetItem *)));

}

// 预览图片
void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    imgs_listeWidget->clear();
    img_name_selected_inlist =  db_root_dir + '/' + img_names.at(index.row());
    QPixmap pixmap;
    pixmap.load(img_name_selected_inlist);
    ui->previewImg->setPixmap(pixmap.scaled(310, 360));

    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(provideContextMenu(QPoint)));
    myMenu = nullptr;
}

// listView右键菜单添加搜索功能
void MainWindow::provideContextMenu(const QPoint &pos)
{
    QAction *searchAction = new QAction(tr("使用当前图片搜索"), this);
    searchAction->setObjectName("使用当前图片搜索");
    searchAction->setStatusTip(tr("使用当前图片搜索"));
    searchAction->setCheckable(true);
    connect(searchAction, SIGNAL(triggered()), this, SLOT(searchSimilarImgs()));

    /*QAction *deleteAction = new QAction(tr("删除"), this);
    deleteAction->setObjectName("删除");
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteImg()));*/

    // Create menu and insert some actions
    myMenu = new QMenu();
    myMenu->addAction(searchAction);
    myMenu->addSeparator(); // 添加分隔线
    //myMenu->addAction(deleteAction);

    // Handle global position
    QPoint globalPos = ui->listView->mapToGlobal(pos);

    if(myMenu)
    {
        // Show context menu at handling position
        myMenu->exec(globalPos);
    }

    if (myMenu)
    {
        myMenu->menuAction()->setVisible(false);
        myMenu->close();
    };
}


void MainWindow::on_addIndexButton_clicked(bool checked)
{
    if(checked) {
        if(featsMat.empty())
        {
            QMessageBox::information(nullptr, "提示", "索引文件不存在，请先建立索引文件再往里提添加图片", QMessageBox::Yes);
            return;
        }

        int numImgsOld = featsMat.rows;

        QString addImgsdir = QFileDialog::getExistingDirectory(this, tr("打开文件夹"), "/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if ( !addImgsdir.isNull() )
        {
            std::ofstream fp;
            std::string tmpFileName = code->fromUnicode(index_file_path).data();
            fp.open(tmpFileName, std::ios::ate|std::ios::binary);
            if (!fp.good()) {
                QMessageBox::information(nullptr, "提示", "索引文件不存在，请对图库先建立索引", QMessageBox::Yes);
                return;
            }
            QMessageBox::information(nullptr, tr("提示"), tr("你选择了") + addImgsdir + tr("文件夹并准备将其添加进索引"));
            QDir export_folder(addImgsdir);
            export_folder.setNameFilters(QStringList()<<"*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG" << "*.PNG" << "*.png" << "*.BMP" << "*.bmp");
            QStringList addImgNames = export_folder.entryList();
            int count = 0;
            std::vector<float> feat;

            progBar = new QProgressDialog(tr("索引构建进度"), tr("取消"), 1, addImgNames.size(), this);
            QList<QPushButton *> L = progBar->findChildren<QPushButton *>();
            L.at(0)->hide();
            //L.at(0)->show();
            progBar->setWindowTitle(tr("进度"));

            for(int i = 0; i < addImgNames.size(); i++)
            {
                // 中文支持
                std::string tmpFileName = code->fromUnicode(addImgNames.at(i)).data();
                std::string tmpDir = code->fromUnicode(addImgsdir).data(); // 非常重要
                std::string tmpImgfullPath2 = tmpDir + '/' + tmpFileName;

                if (std::find(imgNamesDB.begin(), imgNamesDB.end(), tmpImgfullPath2) != imgNamesDB.end()) {
                    continue;
                } else {
                    feat.clear();

                    cv::Mat img_color;
                    cv::Mat img_gray = cv::imread(tmpImgfullPath2, 0);

                    if(img_gray.empty()) {
                        continue;
                    };

                    cv::cvtColor(img_gray, img_color, cv::COLOR_GRAY2BGR);

                    // 如果采用彩色图像模式
                    if(ui->colorModelCheckBox->isChecked()) {
                        img_color = cv::imread(tmpImgfullPath2, 1);
                    }

                    ++ count;
                    if(img_color.empty()) continue;
                    p_calculator->computeFeat(img_color, feat);

                    cv::Mat tmpMat(1, dimFeat, CV_32FC1);
                    for(int i= 0; i < dimFeat; ++i)
                    {
                        tmpMat.at<float>(0, i) = feat.at(i);
                    }
                    featsMat.push_back(tmpMat);
                    imgNamesDB.push_back(GBKToUTF8(tmpImgfullPath2.c_str())); // 防止中文乱码，非常重要

                    progBar->setValue(i+1);
                    QCoreApplication::processEvents();

                    // 增量特征
                    //increFeatsMat.push_back(tmpMat);
                    //increImgNamesDB.push_back(GBKToUTF8(tmpImgfullPath.c_str())); // 防止中文乱码，非常重要
                }
            }

            delete progBar;

            int countGood = featsMat.rows;
            fp.seekp(0, std::ios::beg);
            fp.write((char*)(&countGood), sizeof(int));
            for(int i = 0; i < countGood; i++)
            {
                int imgNameSize = (int)imgNamesDB.at(i).size();
                fp.write(reinterpret_cast<const char*>(&imgNameSize), sizeof(int));
                fp.write(imgNamesDB.at(i).c_str(), imgNameSize);
                fp.write((const char*)(featsMat.row(i).data), dimFeat*sizeof(float));
            }

            fp.close();

            QMessageBox::information(nullptr, "提示", "增量添加索引完成", QMessageBox::Yes);

        } else {
            QMessageBox::information(nullptr, "提示", "未选择文件夹或者文件夹为空", QMessageBox::Yes);
        }
    }
}

/*void MainWindow::showImgInfo()
{
    QString imgPathText = itemTmp->text();
    QMessageBox::information(nullptr, "图片所在路径", imgPathText, QMessageBox::Yes);
    QMessageBox mb(QMessageBox::NoIcon, "图片所在路径",
                   imgPathText, QMessageBox::Ok, this);
    mb.setTextInteractionFlags(Qt::TextSelectableByMouse);
    int dialogResult = mb.exec();
}*/

// listView右键菜单添加搜索功能
/*void MainWindow::provideContextMenuSide(const QPoint &pos)
{
    QAction *searchAction = new QAction(tr("图片路径"), this);
    searchAction->setObjectName("图片路径");
    searchAction->setStatusTip(tr("图片路径"));
    searchAction->setCheckable(true);
    connect(searchAction, SIGNAL(triggered()), this, SLOT(showImgInfo()));

    QAction *deleteAction = new QAction(tr("显示图片"), this);
    deleteAction->setObjectName("显示图片");
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteImg()));

    myMenu = new QMenu();
    myMenu->addAction(searchAction);
    myMenu->addSeparator();
    myMenu->addAction(deleteAction);

    QPoint globalPos = ui->scrlArea->mapToGlobal(pos);

    if(myMenu)
    {
        myMenu->exec(globalPos);
    }

    if (myMenu)
    {
        myMenu->menuAction()->setVisible(false);
        myMenu->close();
    };
}*/

void MainWindow::on_listeWidget_clicked(QListWidgetItem* item)
{
    imgs_listeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    if(item->isSelected()){
        int idx = imgs_listeWidget->currentRow();
        QString imgPathText = "路径: " + item->text() + "\n\n相似度: " + QString::number(scores.at(idx));
        //QMessageBox::information(nullptr, "图片所在路径", imgPathText, QMessageBox::Yes);
        QMessageBox mb(QMessageBox::NoIcon, "图片所在路径",
                       imgPathText, QMessageBox::Ok, this);
        mb.setTextInteractionFlags(Qt::TextSelectableByMouse);
        int dialogResult = mb.exec();
    }
}
