#include "params.h"
#include "ui_params.h"

#include <iostream>

#include <QFile>

Params::Params(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Params)
{
    ui->setupUi(this);

    QString topKStr = "100";
    ui->topKLineEdit->setText(topKStr);

    QString qeKStr = "1";
    ui->qeLineEdit->setText(qeKStr);
}

Params::~Params()
{
    delete ui;
}

void Params::on_pButton_clicked()
{
    QString topKStr = ui->topKLineEdit->text();
    QString qeTopKStr = ui->qeLineEdit->text();
    topK = topKStr.toInt();
    qeTopK= qeTopKStr.toInt();

    accept();
    //close();

}
