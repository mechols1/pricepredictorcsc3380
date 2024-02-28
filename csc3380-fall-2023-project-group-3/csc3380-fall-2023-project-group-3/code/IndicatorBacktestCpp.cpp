#include "IndicatorBacktestCpp.h"
#include "getPath.h"
#include <iostream>

// Constructor
IndicatorBacktestCpp::IndicatorBacktestCpp(const std::string& csvFilename, double initialCapital, double positionSize) {
    // Set PYTHONHOME to your virtual environment path
    std::string user_path = getPath();
    std::string env_path = user_path + "/csc3380-fall-2023-project-group-3/env";
    
    setenv("PYTHONHOME", env_path.c_str(), 1);
    Py_Initialize();

    PyRun_SimpleString("import sys");
    std::string sys_command = "sys.path.append('" + user_path + "csc3380-fall-2023-project-group-3/code')";
    PyRun_SimpleString(sys_command.c_str()); 
    
    PyObject *pName = PyUnicode_FromString("indicator_backtest"); // Module name
    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != nullptr) {
        PyObject *pClass = PyObject_GetAttrString(pModule, "IndicatorBacktest");
        if (pClass != nullptr) {
            PyObject *pFilename = PyUnicode_FromString(csvFilename.c_str());
            pyIndicatorBacktest = PyObject_CallFunction(pClass, "Odd", pFilename, initialCapital, positionSize);
            Py_DECREF(pFilename);
            if (pyIndicatorBacktest == nullptr) {
                PyErr_Print();
            }
            Py_DECREF(pClass);
        } else {
            PyErr_Print();
        }
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
    }
}

// Destructor
IndicatorBacktestCpp::~IndicatorBacktestCpp() {
    Py_XDECREF(pyIndicatorBacktest);
    Py_Finalize();
}

void IndicatorBacktestCpp::resetState() {
    if (pyIndicatorBacktest != nullptr) {
        PyObject_CallMethod(pyIndicatorBacktest, "reset_state", nullptr);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }
}
// using fig.show()
void IndicatorBacktestCpp::plotBollingerBands() {
    if (pyIndicatorBacktest != nullptr) {
        PyObject_CallMethod(pyIndicatorBacktest, "plot_bollinger_bands", nullptr);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }
}

//using html
// std::string IndicatorBacktestCpp::plotBollingerBands() {
//     if (pyIndicatorBacktest != nullptr) {
//         PyObject* pyHtmlContent = PyObject_CallMethod(pyIndicatorBacktest, "plot_bollinger_bands", nullptr);
//         if (pyHtmlContent != nullptr) {
//             PyObject* pyStr = PyUnicode_AsEncodedString(pyHtmlContent, "utf-8", "Error ~");
//             if (pyStr != nullptr) {
//                 const char* htmlContent = PyBytes_AS_STRING(pyStr);
//                 std::string result(htmlContent);
//                 Py_DECREF(pyStr);
                
//                 return result;
//             }
//             Py_DECREF(pyHtmlContent);
//         } else {
//             PyErr_Print();
//         }
//     }
//     return std::string();
// }

// Plot Dual Thrust method
void IndicatorBacktestCpp::plotDualThrust() {
    if (pyIndicatorBacktest != nullptr) {
        PyObject_CallMethod(pyIndicatorBacktest, "plot_dual_thrust", nullptr);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }
}

// std::string IndicatorBacktestCpp::plotDualThrust() {
//     if (pyIndicatorBacktest != nullptr) {
//         PyObject* pyHtmlContent = PyObject_CallMethod(pyIndicatorBacktest, "plot_dual_thrust", nullptr);
//         if (pyHtmlContent != nullptr) {
//             PyObject* pyStr = PyUnicode_AsEncodedString(pyHtmlContent, "utf-8", "Error ~");
//             if (pyStr != nullptr) {
//                 const char* htmlContent = PyBytes_AS_STRING(pyStr);
//                 std::string result(htmlContent);
//                 Py_DECREF(pyStr);
//                 return result;
//             }
//             Py_DECREF(pyHtmlContent);
//         } else {
//             PyErr_Print();
//         }
//     }
//     return std::string();
// }
// Plot Heikin-Ashi
void IndicatorBacktestCpp::plotHeikinAshi() {
    if (pyIndicatorBacktest != nullptr) {
        PyObject_CallMethod(pyIndicatorBacktest, "plot_heikin_ashi", nullptr);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }
}
// std::string IndicatorBacktestCpp::plotHeikinAshi() {
//     if (pyIndicatorBacktest != nullptr) {
//         PyObject* pyHtmlContent = PyObject_CallMethod(pyIndicatorBacktest, "plot_heikin_ashi", nullptr);
//         if (pyHtmlContent != nullptr) {
//             PyObject* pyStr = PyUnicode_AsEncodedString(pyHtmlContent, "utf-8", "Error ~");
//             if (pyStr != nullptr) {
//                 const char* htmlContent = PyBytes_AS_STRING(pyStr);
//                 std::string result(htmlContent);
//                 Py_DECREF(pyStr);
//                 return result;
//             }
//             Py_DECREF(pyHtmlContent);
//         } else {
//             PyErr_Print();
//         }
//     }
//     return std::string();
// }

// Plot Awesome 
void IndicatorBacktestCpp::plotAwesome() {
    if (pyIndicatorBacktest != nullptr) {
        PyObject_CallMethod(pyIndicatorBacktest, "plot_awesome", nullptr);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }
}

// std::string IndicatorBacktestCpp::plotAwesome() {
//     if (pyIndicatorBacktest != nullptr) {
//         PyObject* pyHtmlContent = PyObject_CallMethod(pyIndicatorBacktest, "plot_awesome", nullptr);
//         if (pyHtmlContent != nullptr) {
//             PyObject* pyStr = PyUnicode_AsEncodedString(pyHtmlContent, "utf-8", "Error ~");
//             if (pyStr != nullptr) {
//                 const char* htmlContent = PyBytes_AS_STRING(pyStr);
//                 std::string result(htmlContent);
//                 Py_DECREF(pyStr);
//                 return result;
//             }
//             Py_DECREF(pyHtmlContent);
//         } else {
//             PyErr_Print();
//         }
//     }
//     return std::string();
// }

// Plot MACD
void IndicatorBacktestCpp::plotMACD() {
    if (pyIndicatorBacktest != nullptr) {
        PyObject_CallMethod(pyIndicatorBacktest, "plot_macd", nullptr);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }
}
// std::string IndicatorBacktestCpp::plotMACD() {
//     if (pyIndicatorBacktest != nullptr) {
//         PyObject* pyHtmlContent = PyObject_CallMethod(pyIndicatorBacktest, "plot_macd", nullptr);
//         if (pyHtmlContent != nullptr) {
//             PyObject* pyStr = PyUnicode_AsEncodedString(pyHtmlContent, "utf-8", "Error ~");
//             if (pyStr != nullptr) {
//                 const char* htmlContent = PyBytes_AS_STRING(pyStr);
//                 std::string result(htmlContent);
//                 Py_DECREF(pyStr);
//                 return result;
//             }
//             Py_DECREF(pyHtmlContent);
//         } else {
//             PyErr_Print();
//         }
//     }
//     return std::string();
// }

// Plot RSI
void IndicatorBacktestCpp::plotRSI() {
    if (pyIndicatorBacktest != nullptr) {
        PyObject_CallMethod(pyIndicatorBacktest, "plot_rsi", nullptr);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }
}
// std::string IndicatorBacktestCpp::plotRSI() {
//     if (pyIndicatorBacktest != nullptr) {
//         PyObject* pyHtmlContent = PyObject_CallMethod(pyIndicatorBacktest, "plot_rsi", nullptr);
//         if (pyHtmlContent != nullptr) {
//             PyObject* pyStr = PyUnicode_AsEncodedString(pyHtmlContent, "utf-8", "Error ~");
//             if (pyStr != nullptr) {
//                 const char* htmlContent = PyBytes_AS_STRING(pyStr);
//                 std::string result(htmlContent);
//                 Py_DECREF(pyStr);
//                 return result;
//             }
//             Py_DECREF(pyHtmlContent);
//         } else {
//             PyErr_Print();
//         }
//     }
//     return std::string();
// }

