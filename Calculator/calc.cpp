#include "calc.h"
#include "ui_calc.h"
#include <string>
#include <string.h>
#include <iostream>
#include <cstdio>
#include <map>

Calc::Calc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Calc)
{
    ui->setupUi(this);
    hd['0'] = 0;
    hd['1'] = 1;
    hd['2'] = 2;
    hd['3'] = 3;
    hd['4'] = 4;
    hd['5'] = 5;
    hd['6'] = 6;
    hd['7'] = 7;
    hd['8'] = 8;
    hd['9'] = 9;
    hd['A'] = 10;
    hd['B'] = 11;
    hd['C'] = 12;
    hd['D'] = 13;
    hd['E'] = 14;
    hd['F'] = 15;

    buttons->addButton(ui->pushButton_0,0);
    buttons->addButton(ui->pushButton_1,1);
    buttons->addButton(ui->pushButton_2,2);
    buttons->addButton(ui->pushButton_3,3);
    buttons->addButton(ui->pushButton_4,4);
    buttons->addButton(ui->pushButton_5,5);
    buttons->addButton(ui->pushButton_6,6);
    buttons->addButton(ui->pushButton_7,7);
    buttons->addButton(ui->pushButton_8,8);
    buttons->addButton(ui->pushButton_9,9);
    buttons->addButton(ui->pushButton_10,10);
    buttons->addButton(ui->pushButton_11,11);
    buttons->addButton(ui->pushButton_12,12);
    buttons->addButton(ui->pushButton_13,13);
    buttons->addButton(ui->pushButton_14,14);
    buttons->addButton(ui->pushButton_15,15);
    buttons->addButton(ui->plus,-1);
    buttons->addButton(ui->minus,-2);
    QObject::connect(buttons,SIGNAL(buttonClicked(int)),this,SLOT(addItem(int)));
    QObject::connect(ui->reset,SIGNAL(pressed()),this,SLOT(clear()));
    QObject::connect(ui->eval,SIGNAL(pressed()),this,SLOT(eval()));
    QObject::connect(ui->del_button,SIGNAL(pressed()),this,SLOT(backspcace()));
    std::cout << "calculator is created " << std::endl;
}

Calc::~Calc()
{
    delete ui;
    delete buttons;
}

void Calc::addItem(int button){
    if( str == "" && (button == 0 || button == -1 ) )
        return;
    if( str!= "" && ( *(str.rbegin()) == '-'|| *(str.rbegin()) == '+' ) && ( button == -1 || button == -2 || button == 0 ) )
        return;
    if( button == -1 )
        str += "+";
    else if ( button == -2 )
        str += "-";
    else if( button < 10)
        str += std::to_string(button);
    else
        str += (char)('A'+button-10);

    std::cout << str << " added" << std::endl;
    ui->label->setText(str.c_str());
}

std::string Calc::hex_to_dec(std::string hex){
    long dec = 0;
    for ( long i =0 ; i < hex.size() ; i++ ){
        dec = dec*16 + hd[hex[i]];
    }
    std::cout << " hex is " << hex << " dec is " << dec << std::endl;
    return std::to_string(dec);
}

std::string Calc::dec_to_hex(std::string dec){
    std::string hex = "";
    long _dec = std::stol(dec);
    if( _dec == 0 )
        return "0";
    std::string pre = "";
    if ( _dec < 0 ){
        _dec *= -1;
        pre = "-";
    }
    int rem = 0;
    while(_dec > 0){
        rem = _dec%16;
        _dec = (_dec-rem)/16;
        if ( rem < 10 )
            hex += std::to_string(rem);
        else
            hex += (char)('A' + rem - 10);

    }
    std::reverse(hex.begin(),hex.end());
    std::cout << " dec " << dec << " is hex " << pre+hex << std::endl;
    return pre+hex;
}

void Calc::eval(){
    if(str.size() == 0)
        return;
    if(str[0] == '-')
        str = "0" + str;
    std::cout << str << std::endl;
    std::vector<std::string> parser;
    for (long i = 0 ; i < str.size() ; i++){
        if(str[i] == '+' || str[i] == '-' ){
            parser.push_back(str.substr(0,i));
            parser.push_back(str.substr(i,i+1));
            str.erase(0,i+1);
            i=0;
        }
    }
    parser.push_back(str);
    for (long i = 0 ; i < parser.size() ; i++){
        if( parser[i][0] != '+' && parser[i][0] != '-' )
            parser[i] = hex_to_dec(parser[i]);
    }
    while( parser.size() > 2 ){
        if(parser[1][0] == '+' ){
            parser[2] = std::to_string(std::stol(parser[0]) + std::stol(parser[2]));
            parser.erase(parser.begin());
            parser.erase(parser.begin());
        }
        else{
            parser[2] = std::to_string(std::stol(parser[0]) - std::stol(parser[2]));
            parser.erase(parser.begin());
            parser.erase(parser.begin());
        }
    }
    str = dec_to_hex( parser[0] );
    std::cout << "evaluated dec is " << parser[0] << std::endl ;
    ui->label->setText(str.c_str());
    if( str == "0" )
        str = "";
}

void Calc::clear(){
    str = "";
    std::cout << "cleaned " << std::endl ;
    ui->label->setText("0");
}

void Calc::backspcace(){
    if(!str.empty()){
        str.erase(str.size()-1,str.size());
        if(str.empty())
            ui->label->setText("0");
        else
            ui->label->setText(str.c_str());
        std::cout << "backspace is performed" << std::endl;
    }
}


