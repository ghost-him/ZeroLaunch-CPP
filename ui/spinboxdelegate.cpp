#include "spinboxdelegate.h"
#include <QDoubleSpinBox>

SpinBoxDelegate::SpinBoxDelegate(QObject *parent) : QStyledItemDelegate(parent) {

}



QWidget *SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QDoubleSpinBox *editor {new QDoubleSpinBox(parent)};
    editor->setFrame(false);
    editor->setMinimum(-10000);
    editor->setMaximum(10000);
    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDoubleSpinBox * spinBox = dynamic_cast<QDoubleSpinBox*>(editor);
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox * spinBox = dynamic_cast<QDoubleSpinBox*>(editor);
    double value = spinBox->value();
    model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
