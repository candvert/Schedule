#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    form = new Form();

    // 加载数据
    todayLayout = new QVBoxLayout();
    weekLayout = new QVBoxLayout();
    monthLayout = new QVBoxLayout();
    loadData(todayLayout, "today");
    loadData(weekLayout, "week");
    loadData(monthLayout, "month");

    // 信号连接
    connect(ui->todayButton, &QPushButton::clicked, this, &Widget::todayButtonClicked);
    connect(ui->weekButton, &QPushButton::clicked, this, &Widget::weekButtonClicked);
    connect(ui->monthButton, &QPushButton::clicked, this, &Widget::monthButtonClicked);
    connect(form, &Form::buttonClicked, this, &Widget::addButtonClicked);

    // 设置布局
    ui->todayTasks->setLayout(todayLayout);
    ui->weekTasks->setLayout(weekLayout);
    ui->monthTasks->setLayout(monthLayout);

    // 读取当前任务
    QFile file("data.json");
    file.open(QIODevice::ReadOnly);
    QByteArray arr = file.readAll();
    ui->currentTask->setText(QJsonDocument::fromJson(arr).object()["currentTask"].toString());
    if (ui->currentTask->text() == "") {
        ui->currentTask->setText("当前任务：");
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::loadData(QVBoxLayout *layout, QString tabel)
{
    // 加载数据
    {
        QSqlDatabase todayDB = QSqlDatabase::addDatabase("QSQLITE", "conn");
        todayDB.setDatabaseName("record.db");
        if (!todayDB.open()) {
            qDebug() << "Error: unalbe to open database";
            return;
        }

        QSqlQuery todayQuery(todayDB);
        todayQuery.exec(QString("CREATE TABLE IF NOT EXISTS %1 (id INTEGER PRIMARY KEY "
                                "AUTOINCREMENT, title TEXT, content TEXT, "
                                "checked INTEGER)").arg(tabel));
        todayQuery.exec(QString("SELECT title, content, checked FROM %1").arg(tabel));
        while (todayQuery.next()) {
            Task *task = new Task();
            task->label->setText(todayQuery.value(0).toString());
            task->checkBox->setChecked(todayQuery.value(2).toInt());
            layout->addWidget(task);
            connect(task, &Task::actionSignal, this, &Widget::actionClicked);
            connect(task, &Task::deleteSignal, this, &Widget::deleteTask);
            connect(task, &Task::checkedSignal, this, &Widget::checkBox);
        }
    }
    QSqlDatabase::removeDatabase("conn");

    // 设置布局
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignHCenter);
    layout->setAlignment(Qt::AlignTop);
}

void Widget::actionClicked()
{
    Task *task = qobject_cast<Task*>(sender());
    if (task) {
        ui->currentTask->setText("当前任务：" + task->label->text());
    }
}

void Widget::checkBox()
{
    sqlOperation("UPDATE", qobject_cast<Task*>(sender()));
}

// 每个+按钮都会显示一个新界面
void Widget::todayButtonClicked()
{
    openedForm = 0;
    this->hide();
    form->move(this->pos());
    form->show();
    form->lineEdit()->setFocus();
}

void Widget::weekButtonClicked()
{
    openedForm = 1;
    this->hide();
    form->move(this->pos());
    form->show();
    form->lineEdit()->setFocus();
}

void Widget::monthButtonClicked()
{
    openedForm = 2;
    this->hide();
    form->move(this->pos());
    form->show();
    form->lineEdit()->setFocus();
}

void Widget::closeEvent(QCloseEvent*)
{
    // 将当前任务存放在data.json文件中
    QJsonObject obj;
    obj["currentTask"] = ui->currentTask->text();
    QFile file("data.json");
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(obj).toJson());
    file.close();
}

void Widget::addButtonClicked()
{
    if (form->titleText() == "") {
        form->close();
        this->move(form->pos());
        this->show();
        return;
    }

    sqlOperation("INSERT", nullptr);

    Task *task = new Task();
    task->label->setText(form->titleText());
    connect(task, &Task::actionSignal, this, &Widget::actionClicked);
    connect(task, &Task::deleteSignal, this, &Widget::deleteTask);
    connect(task, &Task::checkedSignal, this, &Widget::checkBox);
    if (openedForm == 0)
        todayLayout->addWidget(task);
    else if (openedForm == 1)
        weekLayout->addWidget(task);
    else if (openedForm == 2)
        monthLayout->addWidget(task);
    form->close();
    this->move(form->pos());
    this->show();
}

void Widget::deleteTask(QString title)
{
    sqlOperation("DELETE", qobject_cast<Task*>(sender()));

    if (ui->currentTask->text() == "当前任务："+title) {
        ui->currentTask->setText("当前任务：");
    }
}

void Widget::sqlOperation(QString operation, Task *task)
{
    // 判断是更新、插入还是删除数据，并判断是操作的是今天、本周还是本月
    QString current;
    if (task) {
        if (task->parent() == qobject_cast<QObject*>(ui->todayTasks)) {
            current = "today";
        } else if (task->parent() == qobject_cast<QObject*>(ui->weekTasks)) {
            current = "week";
        } else if (task->parent() == qobject_cast<QObject*>(ui->monthTasks)) {
            current = "month";
        }
    } else {
        if (openedForm == 0) {
            current = "today";
        } else if (openedForm == 1) {
            current = "week";
        } else if (openedForm == 2) {
            current = "month";
        }
    }

    // 实际操纵数据库
    {
        QSqlDatabase todayDB = QSqlDatabase::addDatabase("QSQLITE", "conn");
        todayDB.setDatabaseName("record.db");
        if (!todayDB.open()) {
            qDebug() << "Error: unalbe to open database";
            return;
        }

        QSqlQuery todayQuery(todayDB);
        if (operation == "UPDATE") {
            todayQuery.prepare(QString("UPDATE %1 SET checked = ? where title = ?").arg(current));
            todayQuery.addBindValue(task->checkBox->isChecked());
            todayQuery.addBindValue(task->label->text());
            todayQuery.exec();
        } else if (operation == "INSERT") {
            todayQuery.prepare(QString("INSERT INTO %1 (title, content, checked)"
                                       " VALUES (?, ?, ?)").arg(current));
            todayQuery.addBindValue(form->titleText());
            todayQuery.addBindValue(form->contentText());
            todayQuery.addBindValue(0);
            todayQuery.exec();
        } else if (operation == "DELETE") {
            todayQuery.prepare(QString("DELETE FROM %1 WHERE title = ?").arg(current));
            todayQuery.addBindValue(task->label->text());
            todayQuery.exec();
        }
    }
    QSqlDatabase::removeDatabase("conn");
}
