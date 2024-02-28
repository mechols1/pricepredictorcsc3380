#ifndef INDICATOR_BACKTEST_CPP_H
#define INDICATOR_BACKTEST_CPP_H

#include <Python.h>
#include <string>

class IndicatorBacktestCpp {
public:
    IndicatorBacktestCpp(const std::string& csvFilename, double initialCapital, double positionSize);
    ~IndicatorBacktestCpp();

    void resetState();
    
    void plotBollingerBands();
    void plotDualThrust();
    void plotHeikinAshi();
    void plotAwesome();
    void plotMACD();
    void plotRSI();

private:
    PyObject *pyIndicatorBacktest;
};

#endif // INDICATOR_BACKTEST_CPP_H
