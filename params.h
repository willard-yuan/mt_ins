#ifndef PARAMS_H
#define PARAMS_H

#include <QDialog>

namespace Ui {
class Params;
}

class Params : public QDialog
{
    Q_OBJECT

public:
    explicit Params(QWidget *parent = nullptr);
    ~Params();

    int topK;
    int qeTopK;

private slots:
    void on_pButton_clicked();

private:
    Ui::Params *ui;
};

#endif // PARAMS_H
