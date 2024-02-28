#ifndef GUI_H
#define GUI_H


#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QString>
#include <iostream>
#include <streambuf>
#include <string>
#include "DataFrame.h"
#include <QTableWidget>
#include <QWebEngineView>
#include <QScrollArea>
#include <QLineEdit>
#include <QFile>

class TextEditStreambuf; // Forward declaration

class Gui : public QMainWindow {
    Q_OBJECT

public:
    Gui(QWidget *parent = nullptr);

private:
    QTextEdit *textEdit;
    TextEditStreambuf *streambuf;
    QLineEdit *tickerInput; 
    QLineEdit *startInput;
    QLineEdit *endInput;
    QLineEdit *initialCapital;
    QLineEdit *positionSize;

    QPushButton *prevButton;
    QPushButton *nextButton;
    int currentPlotIndex;
    int numberOfPlots;

    QWebEngineView *plotView;
    QScrollArea *scrollArea;
    QTableWidget *dataTable; // data table widget

    void setupUI();
    void onPrevClicked();
    void onNextClicked();
    void onButtonClicked();
    //void displayHtmlContent();
    void displayHtmlContent(int index);
    void runPythonScript(const QString &pythonPath, const QString &scriptPath, const QStringList &arguments = QStringList());
    void displayDataFrameInTable(const DataFrame& dataFrame); //dataframe in table
};

// Forward declare the stream buffer class used in Gui
class TextEditStreambuf : public std::streambuf {
public:
    TextEditStreambuf(QTextEdit* textEdit);

protected:
    virtual int_type overflow(int_type v);

private:
    QTextEdit* textEdit;
    
};

#endif // GUI_H