#include "DataFrame.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <stdexcept>

using namespace std;

DataFrame::DataFrame(const DataFrame& other) {
    this->data = other.data;
}

string& DataFrame::operator[](int index) {
    return data[index][0];  // Assuming data is now a vector<vector<string>>
}

const string& DataFrame::operator[](int index) const {
    return data[index][0];
}

DataFrame::DataFrame(string filename) {
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        vector<string> row;  // Use a vector of strings for each row
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, ',')) {
            row.push_back(cell);  // Add each cell as a string
        }
        data.push_back(row);  // Add the row of strings to the DataFrame
    }
}

void DataFrame::print(int x) const {
    // Print the column headers
    cout << "+";
    for (size_t i = 0; i < COLUMN_TITLES.size(); i++) {
        cout << setw(column_widths[i] + 1) << setfill('-') << "+";
    }
    cout << endl;

    for (int i = 0; i < x && i < data.size(); i++) {  // Rows
        cout << "|";
        for (size_t j = 0; j < data[i].size(); j++) {  // Columns
            cout << setw(column_widths[j]) << setfill(' ') << data[i][j] << "|";
        }
        cout << endl;
        if (i == 0) {  // Print the first row as column titles
            cout << "+";
            for (size_t j = 0; j < COLUMN_TITLES.size(); j++) {
                cout << setw(column_widths[j] + 1) << setfill('-') << "+";
            }
            cout << endl;
        }
    }
}

void DataFrame::print_tail(int x) const {
    // Print the column titles first
    cout << "+";
    for (size_t i = 0; i < COLUMN_TITLES.size(); i++) {
        cout << setw(column_widths[i] + 1) << setfill('-') << "+";
    }
    cout << endl;
    cout << "|";
    for (size_t i = 0; i < COLUMN_TITLES.size(); i++) {
        cout << setw(column_widths[i]) << setfill(' ') << COLUMN_TITLES[i] << "|";
    }
    cout << endl;
    cout << "+";
    for (size_t i = 0; i < COLUMN_TITLES.size(); i++) {
        cout << setw(column_widths[i] + 1) << setfill('-') << "+";
    }
    cout << endl;

    // Print the last x rows
    int start_index = max(static_cast<int>(data.size()) - x, 0);
    for (int i = start_index; i < data.size(); i++) {
        cout << "|";
        for (size_t j = 0; j < data[i].size(); j++) {
            cout << setw(column_widths[j]) << setfill(' ') << data[i][j] << "|";
        }
        cout << endl;
    }
}

void DataFrame::normalize_col(int colIndex) {
    if (colIndex >= 0 && colIndex < data[0].size()) {
        // Find max & min among numeric values
        double minVal = numeric_limits<double>::max();
        double maxVal = numeric_limits<double>::lowest();
        for (const auto& row : data) {
            const string& valStr = row[colIndex];
            if (valStr != "NaN") {
                try {
                    double val = stod(valStr);
                    minVal = min(minVal, val);
                    maxVal = max(maxVal, val);
                } catch (const invalid_argument&) {
                    // Handle invalid conversion, e.g., non-numeric string
                }
            }
        }

        // Normalize the values
        for (auto& row : data) {
            string& valStr = row[colIndex];
            if (valStr != "NaN") {
                try {
                    double val = stod(valStr);
                    val = (val - minVal) / (maxVal - minVal); // Normalize
                    valStr = to_string(val); // Convert back to string
                } catch (const invalid_argument&) {
                    // Handle invalid conversion, e.g., non-numeric string
                }
            }
        }
    } else {
        cerr << "Invalid column index." << endl;
    }
}



void DataFrame::process_data() {

    if (!this->data.empty()) {

        cout << "processing" << endl;

        // Normalize numeric columns
        for (size_t colIndex = 0; colIndex < this->data[0].size() - 1; colIndex++) {
            this->normalize_col(colIndex);
        }

        // Remove the last row from the data member (for testing)
        this->data.erase(this->data.end() - 1);
    }
}

string DataFrame::price_to_predict() const {
    DataFrame result(*this); // Create a copy of the current DataFrame object

    if (result.data.empty()) {
        return "No data available";
    }

    const auto& lastRow = result.data.back();
    string priceToPredict = "$" + lastRow[0] + " on " + lastRow[7];
    return priceToPredict;
}


void DataFrame::remove_columns(const vector<int>& colIndices) {
    // Sort the indices in reverse order to avoid shifting issues
    vector<int> sortedIndices = colIndices;
    sort(sortedIndices.rbegin(), sortedIndices.rend());

    for (int colIndex : sortedIndices) {
        if (colIndex >= 0 && colIndex < data[0].size()) {
            for (auto& row : data) {
                row.erase(row.begin() + colIndex);
            }
            if (colIndex < COLUMN_TITLES.size()) {
                COLUMN_TITLES.erase(COLUMN_TITLES.begin() + colIndex);
            }
            if (colIndex < column_widths.size()) {
                column_widths.erase(column_widths.begin() + colIndex);
            }
        }
    }
}

vector<double> DataFrame::extractColumnAsDoubleVector(int colIndex) const {
    vector<double> result;

    for (const auto& row : data) {
        try {
            // Attempt to convert the string value to a double and add it to the result vector
            result.push_back(stod(row[colIndex]));
        } catch (const invalid_argument& e) {
            // If conversion fails (e.g., for "NaN" or non-numeric strings), push a default value or handle as needed
            result.push_back(0.0);  // Or use NaN, or another appropriate default value
        } catch (const out_of_range& e) {
            // Handle values that are out of the range of representable values
            result.push_back(0.0);  // Or handle differently if needed
        }
    }

    return result;
}

int DataFrame::getColumnIndex(const string& columnName) const {
    auto it = find(COLUMN_TITLES.begin(), COLUMN_TITLES.end(), columnName);
    if (it != COLUMN_TITLES.end()) {
        return distance(COLUMN_TITLES.begin(), it);
    } else {
        throw runtime_error("Column name \"" + columnName + "\" not found");
    }
}

// Helper function to calculate mean
optional<double> DataFrame::mean(const vector<double>& values) const {
    if (values.empty()) return nullopt;
    double sum = accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

// Helper function to calculate standard deviation
optional<double> DataFrame::stddev(const vector<double>& values) const {
    if (values.size() <= 1) return nullopt;
    auto avg = mean(values);
    if (!avg) return nullopt;
    double sum_sq = accumulate(values.begin(), values.end(), 0.0,
                               [&avg](double acc, double val) {
                                   return acc + pow(val - avg.value(), 2);
                               });
    return sqrt(sum_sq / (values.size() - 1));
}

vector<optional<double>> DataFrame::rolling_mean(int colIndex, int window) const {
    vector<optional<double>> result;
    for (size_t i = 0; i < data.size(); ++i) {
        vector<double> window_values;
        for (size_t j = (i < window - 1 ? 0 : i - window + 1); j <= i; ++j) {
            const string& valStr = data[j][colIndex];
            if (valStr != "NaN") {
                try {
                    double val = stod(valStr);
                    window_values.push_back(val);
                } catch (const invalid_argument&) {
                    // Handle invalid conversion if necessary
                }
            }
        }
        result.push_back(mean(window_values));
    }
    return result;
}

vector<optional<double>> DataFrame::rolling_std(int colIndex, int window) const {
    vector<optional<double>> result;
    for (size_t i = 0; i < data.size(); ++i) {
        vector<double> window_values;
        for (size_t j = (i < window - 1 ? 0 : i - window + 1); j <= i; ++j) {
            const string& valStr = data[j][colIndex];
            if (valStr != "NaN") {
                try {
                    double val = stod(valStr);
                    window_values.push_back(val);
                } catch (const invalid_argument&) {
                    // Handle invalid conversion if necessary
                }
            }
        }
        result.push_back(stddev(window_values));
    }
    return result;
}


void DataFrame::add_column(const string& name, const vector<string>& column, int colSize) {
    if (column.size() != data.size()) {
        cerr << "Column size mismatch. Cannot add column." << endl;
        return;
    }
    for (size_t i = 0; i < data.size(); ++i) {
        data[i].push_back(column[i]);
    }
    COLUMN_TITLES.push_back(name);
    column_widths.push_back(colSize);
}

void DataFrame::remove_column_by_index(int colIndex) {
    // Check if the DataFrame is empty or if colIndex is out of range
    if (data.empty() || colIndex < 0 || colIndex >= data[0].size()) {
        std::cerr << "Invalid column index or empty DataFrame." << std::endl;
        return;
    }

    // Remove the column from each row
    for (auto& row : data) {
        row.erase(row.begin() + colIndex);
    }

    // Remove the column title and width
    if (colIndex < COLUMN_TITLES.size()) {
        COLUMN_TITLES.erase(COLUMN_TITLES.begin() + colIndex);
    }
    if (colIndex < column_widths.size()) {
        column_widths.erase(column_widths.begin() + colIndex);
    }
}



void DataFrame::remove_column(const string& column_name) {
    int colIndex = getColumnIndex(column_name); // Assuming getColumnIndex() is a method that returns the index of a column by its name
    if (colIndex != -1) {
        remove_column_by_index(colIndex);
    } else {
        cerr << "Column '" << column_name << "' not found." << endl;
    }
}

void DataFrame::apply_condition(int colIndex, const function<bool(double)>& condition, const string& new_col_name) {
    vector<string> new_column_str(data.size(), "NaN");

    for (size_t i = 0; i < data.size(); ++i) {
        const string& valStr = data[i][colIndex];
        try {
            // Convert the string to a double and then apply the condition
            double val = stod(valStr);
            new_column_str[i] = condition(val) ? "1.0" : "0.0";
        } catch (const invalid_argument&) {
            // Handle invalid string-to-double conversion by keeping "NaN"
        }
    }

    // Add the new column with string values to the DataFrame
    add_column(new_col_name, new_column_str);
}


void DataFrame::drop_na() {
    auto new_end = remove_if(data.begin(), data.end(), 
        [](const auto& row) {
            return any_of(row.begin(), row.end(), 
                [](const string& value) { return value == "NaN"; });
        });

    data.erase(new_end, data.end());
}
void DataFrame::save_to_csv(const string& filename, bool printTitles) const {
    try {
        ofstream outfile(filename);

        if (!outfile.is_open()) {
            cerr << "Failed to open the file for writing: " << filename << endl;
            return;
        }

        // Write the column titles
        if (printTitles) {
            for (size_t i = 0; i < COLUMN_TITLES.size(); ++i) {
                outfile << COLUMN_TITLES[i];
                if (i != COLUMN_TITLES.size() - 1) {
                    outfile << ",";
                }
            }
            outfile << "\n";
        }

        // Write the data rows, optionally skipping the first row
        size_t startRow = printTitles ? 1 : 0;  // Skip first row if printTitles is true
        for (size_t i = startRow; i < data.size(); i++) {
            for (size_t j = 0; j < data[i].size(); j++) {
                outfile << data[i][j];
                if (j != data[i].size() - 1) {
                    outfile << ",";
                }
            }
            outfile << "\n";
        }

        outfile.close();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void DataFrame::cropToLastNRows(size_t n) {
    if (data.size() > n) {
        auto start = data.size() - n;
        vector<vector<string>> croppedData(data.begin() + start, data.end());
        data = std::move(croppedData);
    }
}







