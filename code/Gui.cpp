#include "IndicatorBacktestCpp.h" // must be first (has #include <Pthon.h>)
#include "Gui.h"

#include "DataFrame.h"
#include "TechnicalIndicators.h"
#include "getPath.h"

#include <iostream>
#include <streambuf>
#include <string>
#include <QProcess>
#include <QTextEdit>
#include <QFile>
#include <QTableWidgetItem>
#include <vector>
#include <QSplitter>


using namespace std;

Gui::Gui(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    //displayHtmlContent();
}
// Constructor definition
TextEditStreambuf::TextEditStreambuf(QTextEdit* textEdit) : textEdit(textEdit) {}

// Member function definition
std::streambuf::int_type TextEditStreambuf::overflow(std::streambuf::int_type v) {
    if (v == '\n') {
        textEdit->append("");
    } else {
        textEdit->insertPlainText(QString(QChar(v)));
    }
    return v;
}
void Gui::setupUI() {
    // Set the window title
    this->setWindowTitle("Profit Predictors");

    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    // Create a QLineEdit for ticker symbol input
    tickerInput = new QLineEdit(this);
    tickerInput->setPlaceholderText("Enter ticker symbol"); // Optional: placeholder text

    startInput = new QLineEdit(this);
    startInput->setPlaceholderText("Enter start day - YYYY-MM-DD");

    endInput = new QLineEdit(this);
    endInput->setPlaceholderText("Enter end day - YYYY-MM-DD");

    initialCapital = new QLineEdit(this);
    initialCapital->setPlaceholderText("Enter initial capital to use in backtest");

    positionSize = new QLineEdit(this);
    positionSize->setPlaceholderText("Enter amount of shares to buy for each signal");

    // Create and configure the button
    QPushButton *button = new QPushButton("Start", this);
    connect(button, &QPushButton::clicked, this, &Gui::onButtonClicked);

    // Create the QWebEngineView for plot display
    plotView = new QWebEngineView(centralWidget);
    plotView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plotView->setMinimumHeight(200); // Ensure the plot view is initially visible

    // Create and configure the text edit
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    // Optionally, set a minimum size for the textEdit if needed
    // textEdit->setMinimumSize(QSize(200, 100));

    // Create the QTableWidget
    dataTable = new QTableWidget(this);
    dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);  // Make the table read-only
    // Optionally, set a minimum size for the dataTable if needed
    // dataTable->setMinimumSize(QSize(400, 300));

    // Create a horizontal splitter and add text edit and data table
    QSplitter *horizontalSplitter = new QSplitter(Qt::Horizontal, this);
    horizontalSplitter->addWidget(textEdit);
    horizontalSplitter->addWidget(dataTable);

    // Create a vertical splitter and add the horizontal splitter and the plot view
    QSplitter *verticalSplitter = new QSplitter(Qt::Vertical, this);
    verticalSplitter->addWidget(horizontalSplitter);
    verticalSplitter->addWidget(plotView);

    // Optionally, set the initial sizes for the children of the vertical splitter
    QList<int> sizes;
    sizes << this->height() * 0.5 << this->height() * 0.5; // Adjust the ratio as needed
    verticalSplitter->setSizes(sizes);

    // Create a layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tickerInput);
    layout->addWidget(startInput);
    layout->addWidget(endInput);
    layout->addWidget(initialCapital);
    layout->addWidget(positionSize);
    layout->addWidget(button);
    layout->addWidget(verticalSplitter); // Add the vertical splitter to the layout

    // Set the layout to the central widget
    centralWidget->setLayout(layout);

    // Initialize the custom stream buffer for redirecting cout
    streambuf = new TextEditStreambuf(textEdit);
}



//displays given dataFrame in a QDataTable
void Gui::displayDataFrameInTable(const DataFrame& dataFrame) {
    // Clear the existing content of the table
    dataTable->clearContents();

    // Set the number of rows and columns in the table
    dataTable->setRowCount(dataFrame.data.size());
    dataTable->setColumnCount(dataFrame.COLUMN_TITLES.size());

    // Set the column headers
    for (size_t i = 0; i < dataFrame.COLUMN_TITLES.size(); ++i) {
        QTableWidgetItem *headerItem = new QTableWidgetItem(QString::fromStdString(dataFrame.COLUMN_TITLES[i]));
        dataTable->setHorizontalHeaderItem(i, headerItem);
    }

    // Populate the table with data
    for (size_t i = 0; i < dataFrame.data.size(); ++i) {
        for (size_t j = 0; j < dataFrame.data[i].size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(dataFrame.data[i][j]));
            dataTable->setItem(i, j, item);
        }
    }
}

void Gui::onButtonClicked() {

    // Redirect cout to textEdit
    std::streambuf *oldbuf = std::cout.rdbuf(streambuf);
    string user_path = getPath();

    // Fetch the ticker symbol from the QLineEdit
    QString ticker = tickerInput->text();
    QString startDate = startInput->text();
    QString endDate = endInput->text();
    QString initialCapitalStr = initialCapital->text();
    QString positionSizeStr = positionSize->text(); 
    bool ok1, ok2;
    double initialCapitalValue = initialCapitalStr.toDouble(&ok1);
    double positionSizeValue = positionSizeStr.toDouble(&ok2);

    // Check if the conversion was successful
    if (!ok1) {
        // Handle error for initialCapital
        std::cout << "Initial capital must be a number" << std::endl;
    }

    if (!ok2) {
        // Handle error for positionSize
        std::cout << "Position size must be a number" << std::endl;
    }
    QString pythonPath = QString::fromStdString(user_path + "csc3380-fall-2023-project-group-3/env/bin/python");
    QString runPyPath = QString::fromStdString(user_path + "csc3380-fall-2023-project-group-3/code/run_py.py");


    // Run get_data.py script
    runPythonScript(pythonPath, runPyPath, QStringList() << "get_data.py" << ticker << startDate << endDate);


    DataFrame df(user_path + "csc3380-fall-2023-project-group-3/data/" + ticker.toStdString() + ".csv");
    
    //cout << "Data from API" << endl;
    //df.print_tail(10);

    DataFrame og_df(df);

    // string priceToPredict = og_df.price_to_predict();
    // cout << "Price to predict = " << priceToPredict << endl;
    // cout << endl;
    
    df.process_data();
    //cout << "Processed dataframe: " << endl;
    //df.print_tail(10);
    df.save_to_csv(user_path + "csc3380-fall-2023-project-group-3/data/processed_" + ticker.toStdString() + ".csv", false);
    //cout << endl;
    

    runPythonScript(pythonPath, runPyPath, QStringList() << "modeling.py" << ticker << "1" << "2" << "10");
    // after modeling, we can get the tickers
    TechnicalIndicators indicators(og_df);
    // Call getAllIndicators to apply technical indicators to the DataFrame
    indicators.getAllIndicators();
    //cout << "DataFrame with technical indicators:" << endl;
    //og_df.print_tail(10);
    og_df.drop_na();
    og_df.save_to_csv(user_path + "csc3380-fall-2023-project-group-3/data/" + ticker.toStdString() + "_indicators.csv", true);
    
    
    std::string csvFilename = user_path + "csc3380-fall-2023-project-group-3/data/" + ticker.toStdString() + "_indicators.csv";
    //double initialCapital = 10000.0;
    //double positionSize = 100.0;

    IndicatorBacktestCpp indicatorBacktest(csvFilename, initialCapitalValue, positionSizeValue);
    //cout<<"getting backtest html plots"<<endl;
 
    indicatorBacktest.plotBollingerBands();
    indicatorBacktest.plotDualThrust();
    indicatorBacktest.plotHeikinAshi();
    indicatorBacktest.plotAwesome();
    indicatorBacktest.plotMACD();
    indicatorBacktest.plotRSI();
    displayDataFrameInTable(og_df); // displays some dataframe in dataTable 
    
    displayHtmlContent();
    plotView->show();
    // Reset cout to its old self
    std::cout.rdbuf(oldbuf);
}

void Gui::displayHtmlContent() {
    std::string user_path = getPath();
    std::vector<std::string> htmls = {"true_vs_pred.html", "bollinger_bands.html", "dual_thrust.html", "heikin_ashi.html", "awesome.html", "macd.html", "rsi.html"};

    QString concatenatedHtmlContent;

    for(const std::string& html : htmls) {
        QFile file(QString::fromStdString(user_path + "csc3380-fall-2023-project-group-3/data/" + html));
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QString htmlContent = file.readAll();
            concatenatedHtmlContent += htmlContent + "<hr>"; // Use <hr> to separate the contents visually
        } else {
            concatenatedHtmlContent += "<html><body><h1>Error: Unable to open " + QString::fromStdString(html) + "!</h1></body></html><hr>";
        }
    }

    plotView->setHtml(concatenatedHtmlContent);
}


void Gui::runPythonScript(const QString &pythonPath, const QString &runPyPath, const QStringList &arguments) {

    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);

    // Starting the process with run_py.py as the script and passing the necessary arguments
    process.start(pythonPath, QStringList() << runPyPath << arguments);

    bool finished = process.waitForFinished(120000); // Wait for 2 minutes

    if (finished) {
        QString output(process.readAll());
        textEdit->append(output);
    } else {
        QString errorStr = process.errorString();
        textEdit->append("Error running command: " + errorStr);
    }
}