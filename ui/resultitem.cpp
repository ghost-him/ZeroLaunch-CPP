#include "resultitem.h"
#include "ui_resultitem.h"

ResultItem::ResultItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ResultItem)
{
    ui->setupUi(this);
    ui->programIcon->setAlignment(Qt::AlignCenter);
    //int margin = 0; // 边距大小为3
    //ui->programIcon->setContentsMargins(margin, margin, margin, margin);
}

ResultItem::~ResultItem()
{
    delete ui;
}

void ResultItem::setProgramIcon(const QPixmap& pixmap)
{
    QSize labelSize = ui->programIcon->size();
    QMargins margins = ui->programIcon->contentsMargins();
    QSize availableSize(labelSize.width() - margins.left() - margins.right(),
                        labelSize.height() - margins.top() - margins.bottom());
    QPixmap pix = pixmap.scaled(availableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->programIcon->setPixmap(pix);
}

void ResultItem::setProgramName(const QString &name)
{
    ui->programName->setText(name);
}
