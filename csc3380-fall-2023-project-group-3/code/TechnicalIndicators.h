#ifndef TECHNICALINDICATORS_H
#define TECHNICALINDICATORS_H

#include "DataFrame.h"
#include <vector>
#include <string>

class TechnicalIndicators {
public:
    TechnicalIndicators(DataFrame& df);

    void bollingerBandsSignalGeneration(int window = 20, double num_of_std = 2);
    void dualThrustSignalGeneration(int window = 5, double trigger_range = 0.5);
    void heikinAshiSignalGeneration(int stop_loss = 3);
    std::vector<double> ewma(const std::vector<double>& prices, int span);
    void awesomeSignalGeneration(int ma1 = 5, int ma2 = 34);
    void macdSignalGeneration(int ma1 = 5, int ma2 = 34);
    
    std::vector<double> smma(const std::vector<double>& series, int n);
    void rsiSignalGeneration(int lag_days = 14);

    void getAllIndicators();

private:
    DataFrame& df; // Reference to a DataFrame object
};

#endif // TECHNICALINDICATORS_H
