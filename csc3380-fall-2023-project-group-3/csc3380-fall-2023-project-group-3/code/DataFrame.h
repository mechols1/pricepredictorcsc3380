#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <vector>
#include <string>
#include <optional>
#include <functional>

using namespace std;

class DataFrame {
public:
    // Constructors
    DataFrame();
    explicit DataFrame(string filename);
    DataFrame(const DataFrame& other);

    // Attributes
    vector<vector<string>> data;


    // Assignment operator overload
    DataFrame& operator=(const DataFrame& other);

    // Data access
    string& operator[](int index);
    const string& operator[](int index) const;

    // Data processing methods
    void normalize_col(int colIndex);
    void process_data();
    string price_to_predict() const;
    void remove_columns(const vector<int>& colIndices);
    vector<double> extractColumnAsDoubleVector(int colIndex) const;
    void add_column(const string& name, const vector<string>& column, int colSize = 10);
    void apply_condition(int colIndex, const function<bool(double)>& condition, const string& new_col_name);
    void drop_na();
    void save_to_csv(const string& filename, bool printTitles=true) const;
    int getColumnIndex(const string& columnName) const;
    vector<optional<double>> rolling_mean(int colIndex, int window) const;
    vector<optional<double>> rolling_std(int colIndex, int window) const;
    void remove_column(const string& column_name);
    void remove_column_by_index(int colIndex);

    // Printing methods
    void print(int x) const;
    void print_tail(int x) const;
    
    void cropToLastNRows(size_t n);

    // Make sure to add 'sentiment' to COLUMN_TITLES when compliling with nltk sentiment // 
    
    // using alpaca
    //vector<string> COLUMN_TITLES = {"close", "high", "low", "trade_count", "open", "volume", "vwap", "date"};
    //vector<int> column_widths = {15, 15, 15, 15, 15, 15, 15, 15};
    
    // using yfinance
    vector<string> COLUMN_TITLES = {"open", "high", "low", "close", "Adj Close", "volume", "date"}; 
    vector<int> column_widths = {15, 15, 15, 15, 15, 15, 15};
    


private:
    // Helper methods for calculations
    optional<double> mean(const vector<double>& values) const;
    optional<double> stddev(const vector<double>& values) const;

  
    
};

#endif // DATAFRAME_H
