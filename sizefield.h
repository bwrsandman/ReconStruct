#ifndef SIZEFIELD_H
#define SIZEFIELD_H

#include <QFrame>

namespace Ui {
class SizeField;
}
class QRect;

class SizeField : public QFrame
{
    Q_OBJECT

public:
    explicit SizeField(QWidget *parent = 0, QString text=QString());
    ~SizeField();
    QString currentText() const;
    void setGeometry(const QRect &rect);
    void setText(const QString text);

private:
    Ui::SizeField *ui;
};

#endif // SIZEFIELD_H
