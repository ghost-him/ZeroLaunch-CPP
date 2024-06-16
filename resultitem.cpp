#include "resultitem.h"
#include "ui_resultitem.h"

ResultItem::ResultItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ResultItem)
{
    ui->setupUi(this);


}

ResultItem::~ResultItem()
{
    delete ui;
}

void ResultItem::setProgramIcon(const QPixmap &pixmap)
{
    ui->programIcon->setPixmap(pixmap);
}

void ResultItem::setProgramName(const QString &name)
{
    ui->programName->setText(name);
}
