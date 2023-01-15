#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <fcnn.h>

#include <QMainWindow>
#include <QListWidget>
#include <QTimer>
#include <QProgressDialog>
#include <QTextCodec>

#include "params.h"

class QAction;
class QActionGroup;
class QLabel;
class QMenu;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initForm();
    void initModel();

private:
    Ui::MainWindow *ui;
    QListWidget *imgs_listeWidget;

    QStringList img_names;
    QString db_root_dir;

    QString img_name_selected_inlist;
    QString index_file_path;

    void createActions();
    void createMenus();
    void aboutAuthor();

    std::string UTF8ToGBK(const char* strUTF8);
    std::string GBKToUTF8(const char* strGBK);


    QMenu* toolMenu;
    QMenu* aboutMenu;
    QProgressDialog* progBar;

    QAction* aboutAction;
    QAction* aboutAuthorAction;
    QAction* settingAction;

    QTextCodec *code;

    CnnFeature* p_calculator = NULL;
    int inputSize_ = 288;
    const std::string blobName_ = "fc1";
    int dimFeat = 512;
    std::string featsPath = "feats.data";
    cv::Mat featsMat;
    std::vector<std::string> imgNamesDB;

    cv::Mat increFeatsMat;
    std::vector<std::string> increImgNamesDB;

    //std::string modelBinPath = "./model/sscd_disc_mixup.torchscript.onnx";
    std::string modelBinPath = "C:/visWin2022/model/sscd_disc_mixup.torchscript.onnx";

    QMenu *myMenu;
    QMenu *helpMenu;
    Params* paramsDlog;

    //QListWidgetItem* itemTmp;

    int topK;
    int qeTopK;
    std::vector<float> scores;

private slots:
    // 搜索事件
    void on_queryButton_clicked();
    // 右键菜单搜索事件
    void searchSimilarImgs();
    void on_listView_clicked(const QModelIndex &index);

    void quite();

    void on_loadIndexButton_clicked(); // 加载特征
    void on_extractFeatsButton_clicked(bool checked); // 提取图库特征
    void on_imgsOpenButton_clicked(); // 打开图库事件

    void provideContextMenu(const QPoint &pos);
    void on_addIndexButton_clicked(bool checked);

    //void provideContextMenuSide(const QPoint &pos);
    //void showImgInfo();

    void on_listeWidget_clicked(QListWidgetItem* item);

    void setParams();
};

#endif // MAINWINDOW_H
