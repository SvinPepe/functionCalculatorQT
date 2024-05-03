#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegularExpressionValidator>

typedef double ld;
const ld INF = 1e9;
bool errorFlag = 0;

struct Stack {
    Stack *next_elem;
    ld elem;
    char znak;

    void push(Stack *&st, ld new_elem, char new_znak) {
        st = new Stack{st, new_elem, new_znak};
    }

    Stack pop(Stack *&st) {
        auto tmp = st;
        auto tmp_to_return = *tmp;
        st = st->next_elem;
        delete (tmp);
        return tmp_to_return;
    }
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //QRegularExpression functionRegExp("((([1-9][0-9]*|[0-9])[.,]?[0-9]*|x)[+\\-*/^])*");
    //QValidator* functionValidator = new QRegularExpressionValidator(functionRegExp, this);
    //ui->functionInput->setValidator(functionValidator);
    QRegularExpression argRegExp("([1-9][0-9]*|[0-9])[.,]?[0-9]*");
    QValidator* argValidator = new QRegularExpressionValidator(argRegExp, this);
    ui->argInput->setValidator(argValidator);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int priority(char ch) {
    if (ch == '(' || ch == ')') return 0;
    if (ch == '+' || ch == '-') return 1;
    if (ch == '*' || ch == '/') return 2;
     if (ch == '^') return 3;
    if (ch >= '0' && ch <= '9') return -999;
    errorFlag = 1;
    return 999;
}

ld apply(Stack a, Stack b, char znak) {
    if (znak == '+') {
        return a.elem + b.elem;
    }
    if (znak == '-') {
        return a.elem - b.elem;
    }
    if (znak == '/') {
        if (b.elem == 0) {
            errorFlag = 1;
        }
        return ld(a.elem) / ld(b.elem);
    }
    if (znak == '*') {
        return a.elem * b.elem;
    }

    if (znak == '^') {
        ld ans = 1;
        for (int i = 0; i < b.elem; i++) {
            ans *= a.elem;
        }
        return ans;
    }
    return -9999;
}

void prochistka(Stack *&st, Stack *&st2, int prio) {
    if (prio == -100) {
        int lolka = 0;
        while (st2 != nullptr) {

            if (st2->znak == '(') break;

            auto tmp1 = st->pop(st);
            auto tmp2 = st->pop(st);
            st->push(st, apply(tmp2, tmp1, st2->znak), '#');

            st2->pop(st2);
        }

        if (st2 == nullptr) {
            errorFlag = 1;
        }
        st2->pop(st2);
    } else {
        while (priority(st2->znak) > prio && st2->next_elem != nullptr) {
            auto tmp1 = st->pop(st);
            auto tmp2 = st->pop(st);
            st->push(st, apply(tmp2, tmp1, st2->znak), '#');
            st2->pop(st2);
        }
        if (st2->znak != '(') {
            auto tmp1 = st->pop(st);
            auto tmp2 = st->pop(st);
            st->push(st, apply(tmp2, tmp1, st2->znak), '#');
            st2->pop(st2);
        }
    }
}

ld calculate(std::string s) {
       Stack *st2 = nullptr;
       Stack *st = nullptr;
       if (priority(s[0]) > 0 && s[0] != '-') {
           errorFlag = 1;
       }
       int flag = 0;
       for (int i = 0; i < s.size(); i++) {
           if (priority(s[i]) < 0) {
               flag = 1;
           }
           if (i != s.size() - 1) {
               if (priority(s[i]) > 0 && s[i + 1] == ')') {
                   errorFlag = 1;
               }
               if (s[i] == '(' && s[i + 1] == ')') {
                   errorFlag = 1;
               }
               if (priority(s[i]) > 0 && priority(s[i + 1]) > 0) {
                   errorFlag = 1;
               }
           } else {
               if (priority(s[i]) > 0) {
                   errorFlag = 1;
               }
           }
       }
       if (!flag) {
           errorFlag = 1;
       }
       ld num = 0;
       for (int i = 0; i < s.size(); i++) {
           if ('0' <= s[i] && '9' >= s[i]) {
               if (num == INF) num = 0;
               num = num * 10 + (s[i] - '0');
               continue;
           } else {
               if (num != INF) {
                   st->push(st, num, '#');
                   num = INF;
               }
           }
           if (s[i] == '(') {
               st2->push(st2, 0.0, s[i]);
           } else if (s[i] == ')') {
               prochistka(st, st2, -100);
           } else {
               if (i == 0 && s[i] == '-') {
                   st->push(st, 0.0, '#');
               } else if (s[i - 1] == '(' && s[i] == '-') {
                   st->push(st, 0.0, '#');
               }

               if (st2 != nullptr) {
                   if (priority(s[i]) > priority(st2->znak)) {
                       st2->push(st2, 0.0, s[i]);
                   } else {
                       prochistka(st, st2, priority(s[i]));
                       st2->push(st2, 0.0, s[i]);
                   }
               } else {
                   st2->push(st2, 0.0, s[i]);
               }
           }
       }
       if (num != INF) {
           st->push(st, num, '#');
       }
       while (st2 != nullptr) {
           auto tmp1 = st->pop(st);
           auto tmp2 = st->pop(st);
           st->push(st, apply(tmp2, tmp1, st2->znak), '#');
           st2->pop(st2);
       }
    return st->elem;
}


void MainWindow::on_countButton_clicked()
{
    errorFlag = 0;
    QString log = "";
    auto text = ui->functionInput->text().toStdString();
    auto arg = ui->argInput->text().toStdString();
    std::string res = "";
    for (auto i : text) {
        if (i != 'x') {
            if (i != ' ') {
                res += i;
            }
        }else{
            res += arg;
        }

    }
    if (errorFlag) {
        ui->ansOutput->setText("Error");
    }else{
        ui->ansOutput->setText(QString::number(calculate(res)));
    }
    //ui->ansOutput->setText(QString::fromStdString(res));
}
