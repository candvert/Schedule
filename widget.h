#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include "form.h"
#include "task.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QList<QString> todayList;
    QList<QString> weekList;
    QList<QString> monthList;
    QList<QString> today2List;
    QList<QString> week2List;
    QList<QString> month2List;
    Form *form;

protected:
    virtual void closeEvent(QCloseEvent *event) override;

public slots:
    void actionClicked();
    void todayButtonClicked();
    void weekButtonClicked();
    void monthButtonClicked();
    void addButtonClicked();
    void deleteTask(QString title);

private:
    Ui::Widget *ui;
    int openedForm;
    QVBoxLayout *todayLayout;
};
#endif // WIDGET_H
