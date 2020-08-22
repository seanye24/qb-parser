#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QGraphicsView>
#include <QGridLayout>
#include <QTextEdit>
#include <QApplication>
#include <QLabel>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue>

class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);

private:
    QPushButton* search;
    QPushButton* quit;
    QLabel* tossup_label;
    QLabel* category_label;
    QLabel* num_words_label;
    QLineEdit* tossup;
    QLineEdit* category;
    QLineEdit* num_words;
    QTextEdit* results;

    struct tossup_data {
        std::string answer;
        std::string question;
        int category;

        tossup_data(std::string ans, std::string ques, int cat) : answer(ans), question(ques), category(cat) {}
    };

    std::unordered_map<std::string, std::vector<tossup_data*>*>* tossups;
    std::unordered_map<std::string, int>* categories;
    std::unordered_map<std::string, int>* phrases;

    bool Compare(std::string str1, std::string str2, std::unordered_map<std::string, int> &map);
    void fillTossups(std::unordered_map<std::string, std::vector<tossup_data*>*>* tossups);
    void parse(std::string &question, int phrase_len);

signals:

public slots:
    void query();
};

#endif // WINDOW_H
