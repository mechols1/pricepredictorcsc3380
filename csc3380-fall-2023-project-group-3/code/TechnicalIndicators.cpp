#include "TechnicalIndicators.h"
#include <cmath> 
#include <string>
#include <algorithm>
#include <numeric>
#include <iostream>

TechnicalIndicators::TechnicalIndicators(DataFrame& df) : df(df) {}

void TechnicalIndicators::bollingerBandsSignalGeneration(int window, double num_of_std) {
    auto close_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("close"));
    auto rolling_means = df.rolling_mean(df.getColumnIndex("close"), window);
    auto rolling_stds = df.rolling_std(df.getColumnIndex("close"), window);

    std::vector<string> upper_band, lower_band, signals;

    for (size_t i = 0; i < close_prices.size(); ++i) {
        if (rolling_means[i] && rolling_stds[i]) {
            double upper = rolling_means[i].value() + num_of_std * rolling_stds[i].value();
            double lower = rolling_means[i].value() - num_of_std * rolling_stds[i].value();

            upper_band.push_back(to_string(upper));
            lower_band.push_back(to_string(lower));

            if (close_prices[i] < lower) {
                signals.push_back("1");
            } else if (close_prices[i] > upper) {
                signals.push_back("-1");
            } else {
                signals.push_back("0");
            }
        } else {
            upper_band.push_back("NaN");
            lower_band.push_back("NaN");
            signals.push_back("NaN");
        }
    }

    df.add_column("bollinger bands upper band", upper_band, 20);
    df.add_column("bollinger bands lower band", lower_band, 20);
    df.add_column("bollinger bands signals", signals, 20);
}
auto compare_optional = [](const std::optional<double>& a, const std::optional<double>& b) {
    // If both values are present, compare them
    if (a.has_value() && b.has_value()) {
        return a.value() < b.value();
    }
    // If only one of them has a value, the one with nullopt is considered smaller
    if (a.has_value()) {
        return false; // a has a value and b is nullopt, so a is not less than b
    }
    // In cases where a is nullopt (whether b has a value or not), a is considered smaller
    return true;
};

void TechnicalIndicators::dualThrustSignalGeneration(int window, double trigger_range) {
    // Extract necessary columns as double vectors
    auto high_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("high"));
    auto low_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("low"));
    auto close_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("close"));

    // Calculate the dual thrust range
    std::vector<double> dual_thrust_range(close_prices.size(), 0.0);
    for (size_t i = window; i < close_prices.size(); ++i) {
        double max_high = *std::max_element(high_prices.begin() + i - window, high_prices.begin() + i);
        double min_close = *std::min_element(close_prices.begin() + i - window, close_prices.begin() + i);
        double max_close = *std::max_element(close_prices.begin() + i - window, close_prices.begin() + i);
        double min_low = *std::min_element(low_prices.begin() + i - window, low_prices.begin() + i);

        dual_thrust_range[i] = std::max(max_high - min_close, max_close - min_low);
    }

    // Calculate the high and low signals
    std::vector<double> high_signal(close_prices.size(), 0.0);
    std::vector<double> low_signal(close_prices.size(), 0.0);
    std::vector<double> signals(close_prices.size(), 0.0);

    for (size_t i = window; i < close_prices.size(); ++i) {
        double range_value = dual_thrust_range[i - window];
        high_signal[i] = trigger_range * range_value + close_prices[i - window];
        low_signal[i] = -(1 - trigger_range) * range_value + close_prices[i - window];

        if (close_prices[i] > high_signal[i]) signals[i] = 1;
        else if (close_prices[i] < low_signal[i]) signals[i] = -1;
    }

    // Convert numerical vectors to string vectors for DataFrame
    vector<string> high_signal_str;
    high_signal_str.reserve(high_signal.size());
    transform(high_signal.begin(), high_signal.end(), back_inserter(high_signal_str),
        [](double val) -> string {
            return to_string(val);
        });

    vector<string> low_signal_str;
    low_signal_str.reserve(low_signal.size());
    transform(low_signal.begin(), low_signal.end(), back_inserter(low_signal_str),
        [](double val) -> string {
            return to_string(val);
        });

    vector<string> signals_str;
    signals_str.reserve(signals.size());
    transform(signals.begin(), signals.end(), back_inserter(signals_str),
        [](double val) -> string {
            return to_string(val);
        });

    // Add calculated columns to the DataFrame
    df.add_column("dual thrust upperbound", high_signal_str);
    df.add_column("dual thrust lowerbound", low_signal_str);
    df.add_column("dual thrust signals", signals_str);
}

void TechnicalIndicators::heikinAshiSignalGeneration(int stop_loss) {
    auto open_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("open"));
    auto close_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("close"));
    auto high_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("high"));
    auto low_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("low"));

    std::vector<double> ha_close, ha_high, ha_low, ha_open(open_prices.size());
    std::vector<double> signals(open_prices.size(), 0.0);
    std::vector<int> cumsum(open_prices.size(), 0);

    // Calculate HA close
    for (size_t i = 0; i < open_prices.size(); ++i) {
        ha_close.push_back((open_prices[i] + close_prices[i] + high_prices[i] + low_prices[i]) / 4);
    }

    // Calculate HA open
    ha_open[0] = open_prices[0];
    for (size_t i = 1; i < open_prices.size(); ++i) {
        ha_open[i] = (ha_open[i - 1] + ha_close[i - 1]) / 2;
    }

    // Calculate HA high and low
    for (size_t i = 0; i < open_prices.size(); ++i) {
        ha_high.push_back(std::max({ha_open[i], ha_close[i], high_prices[i]}));
        ha_low.push_back(std::min({ha_open[i], ha_close[i], low_prices[i]}));
    }

    // Generate signals
    for (size_t i = 1; i < open_prices.size(); ++i) {
        if (ha_open[i] > ha_close[i] && ha_open[i] == ha_high[i] &&
            std::abs(ha_open[i] - ha_close[i]) > std::abs(ha_open[i - 1] - ha_close[i - 1]) &&
            ha_open[i - 1] > ha_close[i - 1]) {
            signals[i] = (cumsum[i - 1] < stop_loss) ? 1.0 : 0.0;
        }
        else if (ha_open[i] < ha_close[i] && ha_open[i] == ha_low[i] &&
                 ha_open[i - 1] < ha_close[i - 1]) {
            signals[i] = (cumsum[i - 1] > 0) ? -1.0 : 0.0;
        }
        cumsum[i] = cumsum[i - 1] + signals[i];
    }

    // Convert numerical vectors to string vectors for DataFrame
    vector<string> ha_open_str, ha_close_str, ha_high_str, ha_low_str, signals_str;
    transform(ha_open.begin(), ha_open.end(), back_inserter(ha_open_str), [](double val) { return to_string(val); });
    transform(ha_close.begin(), ha_close.end(), back_inserter(ha_close_str), [](double val) { return to_string(val); });
    transform(ha_high.begin(), ha_high.end(), back_inserter(ha_high_str), [](double val) { return to_string(val); });
    transform(ha_low.begin(), ha_low.end(), back_inserter(ha_low_str), [](double val) { return to_string(val); });
    transform(signals.begin(), signals.end(), back_inserter(signals_str), [](double val) { return to_string(val); });

    // Add calculated columns to DataFrame
    df.add_column("HA open", ha_open_str);
    df.add_column("HA close", ha_close_str);
    df.add_column("HA high", ha_high_str);
    df.add_column("HA low", ha_low_str);
    df.add_column("HA signals", signals_str);
}



void TechnicalIndicators::awesomeSignalGeneration(int ma1, int ma2) {
    auto high_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("high"));
    auto low_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("low"));

    // Calculate mid prices
    std::vector<double> mid_prices(high_prices.size());
    for (size_t i = 0; i < high_prices.size(); ++i) {
        mid_prices[i] = (high_prices[i] + low_prices[i]) / 2.0;
    }

    // Add mid prices as a new column to DataFrame (optional)
    vector<string> mid_prices_str;
    transform(mid_prices.begin(), mid_prices.end(), back_inserter(mid_prices_str),
              [](double val) { return to_string(val); });
    df.add_column("Mid Price", mid_prices_str);

    // Calculate moving averages using the rolling_mean method
    auto awesome_ma1 = df.rolling_mean(df.getColumnIndex("Mid Price"), ma1);
    auto awesome_ma2 = df.rolling_mean(df.getColumnIndex("Mid Price"), ma2);

    vector<double> awesome_oscillator(mid_prices.size(), 0.0);
    vector<double> awesome_signals(mid_prices.size(), 0.0);

    // Calculate Awesome Oscillator
    for (size_t i = 0; i < mid_prices.size(); ++i) {
        if (awesome_ma1[i].has_value() && awesome_ma2[i].has_value()) {
            awesome_oscillator[i] = awesome_ma1[i].value() - awesome_ma2[i].value();
        }
    }

    auto open_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("open"));
    auto close_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("close"));

    // Generate signals based on bearish and bullish saucer conditions
    for (size_t i = 2; i < awesome_signals.size(); ++i) {
        bool bearish_saucer = open_prices[i] > close_prices[i] &&
                              open_prices[i - 1] < close_prices[i - 1] &&
                              open_prices[i - 2] < close_prices[i - 2] &&
                              awesome_oscillator[i - 1] > awesome_oscillator[i - 2] &&
                              awesome_oscillator[i - 1] < 0 &&
                              awesome_oscillator[i] < 0;

        bool bullish_saucer = open_prices[i] < close_prices[i] &&
                              open_prices[i - 1] > close_prices[i - 1] &&
                              open_prices[i - 2] > close_prices[i - 2] &&
                              awesome_oscillator[i - 1] < awesome_oscillator[i - 2] &&
                              awesome_oscillator[i - 1] > 0 &&
                              awesome_oscillator[i] > 0;

        if (bearish_saucer) {
            awesome_signals[i] = 1.0;
        } else if (bullish_saucer) {
            awesome_signals[i] = -1.0;
        }
    }

    // Convert numerical vectors to string vectors
    vector<string> awesome_ma1_str, awesome_ma2_str, awesome_signals_str;
    transform(awesome_ma1.begin(), awesome_ma1.end(), back_inserter(awesome_ma1_str), 
              [](const optional<double>& val) { return val ? to_string(val.value()) : "NaN"; });
    transform(awesome_ma2.begin(), awesome_ma2.end(), back_inserter(awesome_ma2_str), 
              [](const optional<double>& val) { return val ? to_string(val.value()) : "NaN"; });
    transform(awesome_signals.begin(), awesome_signals.end(), back_inserter(awesome_signals_str), 
              [](double val) { return to_string(val); });

    // Add calculated columns to DataFrame
    df.add_column("awesome ma1", awesome_ma1_str);
    df.add_column("awesome ma2", awesome_ma2_str);
    df.add_column("awesome signals", awesome_signals_str);
    df.remove_column("Mid Price");
}


std::vector<double> TechnicalIndicators::ewma(const std::vector<double>& prices, int span) {
    std::vector<double> result(prices.size(), 0.0);
    if (prices.empty() || span <= 0) return result;

    double alpha = 2.0 / (span + 1); // Weighting factor
    result[0] = prices[0]; // First value is the same as input

    for (size_t i = 1; i < prices.size(); ++i) {
        result[i] = alpha * prices[i] + (1 - alpha) * result[i - 1];
    }

    return result;
}

void TechnicalIndicators::macdSignalGeneration(int ma1, int ma2) {
    auto close_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("close"));

    // Calculate EWMA
    auto macd_ma1 = ewma(close_prices, ma1);
    auto macd_ma2 = ewma(close_prices, ma2);

    std::vector<double> macd_positions(close_prices.size(), 0.0);
    std::vector<double> macd_signals(close_prices.size(), 0.0);

    // Determine positions
    for (size_t i = ma1; i < close_prices.size(); ++i) {
        macd_positions[i] = macd_ma1[i] >= macd_ma2[i] ? 1.0 : 0.0;
    }

    // Calculate signals
    for (size_t i = 1; i < close_prices.size(); ++i) {
        macd_signals[i] = macd_positions[i] - macd_positions[i - 1];
    }

    // Convert to string for DataFrame compatibility
    vector<string> macd_ma1_str;
    vector<string> macd_ma2_str;
    vector<string> macd_signals_str;

    std::transform(macd_ma1.begin(), macd_ma1.end(), std::back_inserter(macd_ma1_str), [](double val) { return std::to_string(val); });
    std::transform(macd_ma2.begin(), macd_ma2.end(), std::back_inserter(macd_ma2_str), [](double val) { return std::to_string(val); });
    std::transform(macd_signals.begin(), macd_signals.end(), std::back_inserter(macd_signals_str), [](double val) { return std::to_string(val); });

    // Add calculated columns to DataFrame
    df.add_column("macd ma1", macd_ma1_str);
    df.add_column("macd ma2", macd_ma2_str);
    df.add_column("macd signals", macd_signals_str);
}




std::vector<double> TechnicalIndicators::smma(const std::vector<double>& series, int n) {
    std::vector<double> output(series.size());
    output[0] = series[0];

    for (size_t i = 1; i < series.size(); ++i) {
        output[i] = (output[i - 1] * (n - 1) + series[i]) / n;
    }

    return output;
}


void TechnicalIndicators::rsiSignalGeneration(int lag_days) {
    auto close_prices = df.extractColumnAsDoubleVector(df.getColumnIndex("close"));

    std::vector<double> delta(close_prices.size());
    std::adjacent_difference(close_prices.begin(), close_prices.end(), delta.begin());

    std::vector<double> gains(delta.size()), losses(delta.size());
    for (size_t i = 0; i < delta.size(); ++i) {
        gains[i] = std::max(delta[i], 0.0);  // Gains are positive deltas
        losses[i] = std::max(-delta[i], 0.0); // Losses are negative deltas made positive
    }

    auto smma_gains = smma(gains, lag_days);
    auto smma_losses = smma(losses, lag_days);

    std::vector<double> rsi(close_prices.size(), 0.0);
    for (size_t i = lag_days; i < close_prices.size(); ++i) {
        if (smma_losses[i - lag_days] != 0) {
            double rs = smma_gains[i - lag_days] / smma_losses[i - lag_days];
            rsi[i] = 100 - (100 / (1 + rs));
        } else {
            rsi[i] = 100; // If no losses, RSI is set to 100
        }
    }

    std::vector<string> rsi_str(rsi.size());
    std::vector<string> signals_str(rsi.size(), "0"); // Default to "0" for no signal

    for (size_t i = lag_days; i < rsi.size(); ++i) {
        rsi_str[i] = std::to_string(rsi[i]);
        if (rsi[i] < 30) {
            signals_str[i] = "1";
        } else if (rsi[i] > 70) {
            signals_str[i] = "-1";
        }
    }

    // Add calculated columns to DataFrame
    df.add_column("RSI", rsi_str);
    df.add_column("RSI signals", signals_str);
}

void TechnicalIndicators::getAllIndicators() {
    // Call each indicator method
    //std::cout << "Getting Bollinger Bands signals" << std::endl;
    bollingerBandsSignalGeneration(20, 2);
    //std::cout << "Getting Dual Thrust signals" << std::endl;
    dualThrustSignalGeneration(5, 0.5);    
    //std::cout << "Getting Heikin-Ashi signals" << std::endl;
    heikinAshiSignalGeneration(3);    
    //std::cout << "Getting Awesome Oscillator signals" << std::endl;
    awesomeSignalGeneration(5, 34);       
    //std::cout << "Getting MACD Oscillator signals" << std::endl;
    macdSignalGeneration(5, 34);          
    //std::cout << "Getting RSI signals" << std::endl;
    rsiSignalGeneration(14);        

}





