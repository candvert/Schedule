#ifndef TASK_H
#define TASK_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QMouseEvent>
#include <QDockWidget>
#include <QMenu>
#include <QAction>

class Task : public QWidget
{
    Q_OBJECT
public:
    explicit Task(QWidget *parent = nullptr);
    QAction *action;
    QLabel *label;

signals:
    void actionSignal();
    void deleteSignal(QString title);

protected:
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
    QCheckBox *checkBox;
    QPushButton *button;
    QMenu *contextMenu;
};

#endif // TASK_H
