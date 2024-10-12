#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    form = new Form();

    // 添加今日任务
    todayLayout = new QVBoxLayout();
    {
        QSqlDatabase todayDB = QSqlDatabase::addDatabase("QSQLITE", "today");
        todayDB.setDatabaseName("record.db");
        if (!todayDB.open()) {
            qDebug() << "Error: unalbe to open database";
            return;
        }

        QSqlQuery todayQuery(todayDB);
        todayQuery.exec("CREATE TABLE IF NOT EXISTS today (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT, content TEXT)");
        todayQuery.exec("SELECT title, content FROM today");
        while (todayQuery.next()) {
            Task *task = new Task();
            task->label->setText(todayQuery.value(0).toString());
            todayLayout->addWidget(task);
            connect(task, &Task::actionSignal, this, &Widget::actionClicked);
            connect(task, &Task::deleteSignal, this, &Widget::deleteTask);
        }
    }
    QSqlDatabase::removeDatabase("today");

    todayLayout->setSpacing(0);
    todayLayout->setContentsMargins(0, 0, 0, 0);
    todayLayout->setAlignment(Qt::AlignHCenter);
    todayLayout->setAlignment(Qt::AlignTop);

    connect(ui->todayButton, &QPushButton::clicked, this, &Widget::todayButtonClicked);
    connect(ui->weekButton, &QPushButton::clicked, this, &Widget::weekButtonClicked);
    connect(ui->monthButton, &QPushButton::clicked, this, &Widget::monthButtonClicked);
    connect(form, &Form::buttonClicked, this, &Widget::addButtonClicked);

    ui->todayTasks->setLayout(todayLayout);

    QFile file("data.json");
    file.open(QIODevice::ReadOnly);
    QByteArray arr = file.readAll();
    ui->currentTask->setText(QJsonDocument::fromJson(arr).object()["currentTask"].toString());
    if (ui->currentTask->text() == "") {
        ui->currentTask->setText("当前任务：");
    }

    // 显示消息
}

Widget::~Widget()
{
    delete ui;
}

void Widget::actionClicked()
{
    Task *task = qobject_cast<Task*>(sender());
    if (task) {
        ui->currentTask->setText("当前任务：" + task->label->text());
    }
}

// 每个+按钮都会显示一个新界面
void Widget::todayButtonClicked()
{
    openedForm = 0;
    form->move(this->pos());
    form->show();
}

void Widget::weekButtonClicked()
{
    openedForm = 1;
    form->move(this->pos());
    form->show();
}

void Widget::monthButtonClicked()
{
    openedForm = 2;
    form->move(this->pos());
    form->show();
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
        return;
    }

    {
        QSqlDatabase todayDB = QSqlDatabase::addDatabase("QSQLITE", "today");
        todayDB.setDatabaseName("record.db");
        if (!todayDB.open()) {
            qDebug() << "Error: unalbe to open database";
            return;
        }

        QSqlQuery todayQuery(todayDB);
        todayQuery.exec("INSERT INTO today (title, content) VALUES (?, ?)");
        todayQuery.addBindValue(form->titleText());
        todayQuery.addBindValue(form->contentText());
        todayQuery.exec();
    }

    QSqlDatabase::removeDatabase("today");

    Task *task = new Task();
    task->label->setText(form->titleText());
    connect(task, &Task::actionSignal, this, &Widget::actionClicked);
    connect(task, &Task::deleteSignal, this, &Widget::deleteTask);
    todayLayout->addWidget(task);
    form->close();
}

void Widget::deleteTask(QString title)
{
    {
        QSqlDatabase todayDB = QSqlDatabase::addDatabase("QSQLITE", "today");
        todayDB.setDatabaseName("record.db");
        if (!todayDB.open()) {
            qDebug() << "Error: unalbe to open database";
            return;
        }

        QSqlQuery todayQuery(todayDB);
        todayQuery.prepare("DELETE FROM today WHERE title = ?");
        todayQuery.addBindValue(title);
        todayQuery.exec();
    }
    QSqlDatabase::removeDatabase("today");

    if (ui->currentTask->text() == "当前任务："+title) {
        ui->currentTask->setText("当前任务：");
    }
}
