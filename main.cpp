#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include "MainWindow.h"


//一些背景美化设置
void setModernDarkTheme(QApplication& app) {
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
    darkPalette.setColor(QPalette::AlternateBase, QColor(50, 50, 50));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    app.setPalette(darkPalette);
    
    app.setStyleSheet(
        "QPushButton { background-color: #0e639c; color: white; border: none; padding: 8px 16px; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background-color: #1177bb; }"
        "QPushButton:pressed { background-color: #094771; }"
        "QListWidget { background-color: #252526; border: none; outline: 0; font-size: 14px; border-right: 1px solid #444; }"
        "QListWidget::item { padding: 12px; border-bottom: 1px solid #333; color: #ccc; }"
        "QListWidget::item:selected { background-color: #0e639c; color: white; }"
    );
}

// 主函数，程序的入口点
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    setModernDarkTheme(a);
    MainWindow w;
    w.resize(1100, 700);
    w.show();
    return a.exec();
}
