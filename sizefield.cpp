#include <QRect>

#include "sizefield.h"
#include "ui_sizefield.h"

SizeField::SizeField(QWidget *parent, QString text) :
    QFrame(parent),
    ui(new Ui::SizeField)
{
    ui->setupUi(this);
    setFocusProxy(ui->lineEdit);
    ui->lineEdit->setText(text);
}

SizeField::~SizeField()
{
    delete ui;
}

QString SizeField::currentText() const
{
    return ui->lineEdit->text();
}

void SizeField::setText(const QString text)
{
    ui->lineEdit->setText(text);
}

void SizeField::setGeometry(const QRect &rect)
{
    int buttonWidth = ui->toolButton->width();
    ui->lineEdit->setGeometry(rect);
    ui->toolButton->move(rect.topRight() + QPoint(-buttonWidth, 2));
    ui->toolButton->resize(buttonWidth, rect.height() * 0.8f);
}
