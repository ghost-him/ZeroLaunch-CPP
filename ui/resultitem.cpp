#include "resultitem.h"
#include "ui_resultitem.h"
#include <QResizeEvent>
#include <QSize>

ResultItem::ResultItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ResultItem)
{
    ui->setupUi(this);
    ui->programIcon->setAlignment(Qt::AlignCenter);

}

ResultItem::~ResultItem()
{
    delete ui;
}

void ResultItem::setProgramIcon(const QPixmap& pixmap)
{
    QSize labelSize = ui->programIcon->size();
    //qDebug() << "label size: "<<labelSize;
    // QMargins margins = ui->programIcon->contentsMargins();
    // QSize availableSize(labelSize.width() - margins.left() - margins.right(),
    //                     labelSize.height() - margins.top() - margins.bottom());
    // QPixmap pix = pixmap.scaled(availableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap pix = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->programIcon->setPixmap(pix);
}

void ResultItem::setProgramName(const QString &name)
{
    ui->programName->setText(name);
}

void ResultItem::resizeEvent(QResizeEvent *event)
{

    int height{event->size().height()};
    //qDebug() << "change size, height: " << height;
    ui->programIcon->setFixedSize({height, height});
    int margin = height / 20; // 边距大小为3
    ui->programIcon->setContentsMargins(0, margin, 0, margin);
    ui->programName->setContentsMargins(0, margin, 0, margin);
    QFont font;
    font.setPixelSize(height*0.35);
    ui->programName->setFont(font);
}
