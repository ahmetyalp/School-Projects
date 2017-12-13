#ifndef CALC_H
#define CALC_H

#include <vector>
#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <map>

namespace Ui {
class Calc;
}

class Calc : public QWidget
{
    Q_OBJECT

public:
    explicit Calc(QWidget *parent = 0);
    std::map<char,int> hd; // hex to dec digits
    ~Calc();
    std::string hex_to_dec(std::string hex);
    std::string dec_to_hex(std::string dec);

private slots:
    void addItem(int digit); // add number to list that be calculated after pressing "="
    void eval();  // calculates what user wrote
    void clear(); // sets calculator to zero
    void backspcace();

private:
    Ui::Calc *ui;
    QButtonGroup *buttons = new QButtonGroup();
    std::string str = "";
};

#endif // CALC_H
