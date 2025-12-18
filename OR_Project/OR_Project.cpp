#include <iostream>
#include <iomanip>
#include <vector>
#include <windows.h>
#include <cmath>
#include <string>

using namespace std;

int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

void centerText(const string& text) {
    int width = getConsoleWidth();
    int padding = (width - text.length()) / 2;
    if (padding < 0) padding = 0;
    cout << string(padding, ' ') << text << endl;
}

void drawScreen() {
    cout << "\n\n";
    centerText("========================================");
    centerText("Hello Business Man");
    centerText("========================================");
    cout << "\n";
    centerText(">> Calculate Profit <<");
    cout << "\n";
    centerText("$$$$$$$$$$$$$$$$$$$$$$");
    centerText("$$ PROFIT MODE $$");
    centerText("$$$$$$$$$$$$$$$$$$$$$$");
    cout << "\n";
    centerText("Business Optimization");
    centerText("[ Profit | Resources | Machines ]");
}

void printTable(const vector<vector<double>>& table, int n, int m, int iteration,
    const vector<int>& basicVars, const vector<double>& profit) {
    cout << "\n========== Iteration " << iteration << " ==========\n\n";
    cout << setw(8) << "Base" << " | " << setw(8) << "CB" << " | " << setw(8) << "XB" << " | ";

    for (int i = 1; i <= n; i++)
        cout << setw(8) << ("x" + to_string(i)) << " | ";

    for (int i = 1; i <= m; i++)
        cout << setw(8) << ("s" + to_string(i)) << " | ";

    cout << setw(10) << "RHS" << endl;
    cout << string(12 + (n + m + 3) * 11, '-') << endl;

    for (int i = 0; i < m; i++) {        
        string baseVar;
        double cb = 0;
        if (basicVars[i] < n) {
            baseVar = "x" + to_string(basicVars[i] + 1);
            cb = profit[basicVars[i]];
        }
        else {
            baseVar = "s" + to_string(basicVars[i] - n + 1);
            cb = 0;
        }

        cout << setw(8) << baseVar << " | ";
        cout << setw(8) << fixed << setprecision(2) << cb << " | ";
        cout << setw(8) << fixed << setprecision(2) << table[i][n + m] << " | ";

        for (int j = 0; j < n + m; j++)
            cout << setw(8) << fixed << setprecision(2) << table[i][j] << " | ";

        cout << setw(10) << fixed << setprecision(2) << table[i][n + m] << endl;
    }

    cout << string(12 + (n + m + 3) * 11, '-') << endl;

    cout << setw(8) << "Z" << " | ";
    cout << setw(8) << "-" << " | ";
    cout << setw(8) << fixed << setprecision(2) << table[m][n + m] << " | ";

    for (int j = 0; j < n + m; j++)
        cout << setw(8) << fixed << setprecision(2) << table[m][j] << " | ";

    cout << setw(10) << fixed << setprecision(2) << table[m][n + m] << endl;
    cout << string(12 + (n + m + 3) * 11, '-') << endl;

    cout << setw(8) << "Cj-Zj" << " | ";
    cout << setw(8) << "-" << " | ";
    cout << setw(8) << "-" << " | ";

    for (int j = 0; j < n + m; j++)
        cout << setw(8) << fixed << setprecision(2) << -table[m][j] << " | ";

    cout << setw(10) << "-" << endl;
    cout << "====================================================\n";
}

bool isOptimal(const vector<vector<double>>& table) {
    int lastRow = table.size() - 1;
    int cols = table[0].size();
    for (int j = 0; j < cols - 1; j++)
        if (table[lastRow][j] < -1e-6)
            return false;
    return true;
}

int getPivotColumn(const vector<vector<double>>& table) {
    int lastRow = table.size() - 1;
    int cols = table[0].size();
    int pivotCol = 0;
    double minVal = table[lastRow][0];

    for (int j = 1; j < cols - 1; j++) {
        if (table[lastRow][j] < minVal) {
            minVal = table[lastRow][j];
            pivotCol = j;
        }
    }
    return pivotCol;
}

int getPivotRow(const vector<vector<double>>& table, int pivotCol) {
    int rows = table.size();
    int cols = table[0].size();
    int pivotRow = -1;
    double minRatio = 1e9;

    for (int i = 0; i < rows - 1; i++) {
        if (table[i][pivotCol] > 1e-6) {
            double ratio = table[i][cols - 1] / table[i][pivotCol];
            if (ratio >= 0 && ratio < minRatio) {
                minRatio = ratio;
                pivotRow = i;
            }
        }
    }
    return pivotRow;
}

void pivotOperation(vector<vector<double>>& table, int pivotRow, int pivotCol) {
    int rows = table.size();
    int cols = table[0].size();

    double pivot = table[pivotRow][pivotCol];

    for (int j = 0; j < cols; j++)
        table[pivotRow][j] /= pivot;

    for (int i = 0; i < rows; i++) {
        if (i != pivotRow) {
            double factor = table[i][pivotCol];
            for (int j = 0; j < cols; j++)
                table[i][j] -= factor * table[pivotRow][j];
        }
    }
}

vector<double> getVariableValues(const vector<vector<double>>& table, int n) {
    int rows = table.size();
    int cols = table[0].size();
    vector<double> values(n, 0.0);

    for (int j = 0; j < n; j++) {
        int oneRow = -1;
        bool basic = true;

        for (int i = 0; i < rows; i++) {
            if (fabs(table[i][j] - 1.0) < 1e-6) {
                if (oneRow == -1) oneRow = i;
                else basic = false;
            }
            else if (fabs(table[i][j]) > 1e-6) {
                basic = false;
            }
        }

        if (basic && oneRow != -1)
            values[j] = table[oneRow][cols - 1];
    }
    return values;
}

int main() {
    drawScreen();

    cout << "\n\n";
    centerText("Press ENTER to start...");
    cin.get();

    int n, m;
    cout << "\nEnter number of products: ";
    cin >> n;

    vector<double> profit(n);
    cout << "\nEnter profit per unit:\n";
    for (int i = 0; i < n; i++) {
        cout << "Product " << i + 1 << ": ";
        cin >> profit[i];
    }

    cout << "\nEnter number of machines: "; 
    cin >> m;

    vector<double> timeAvailable(m);
    cout << "\nEnter available time (Mintues):\n";
    for (int i = 0; i < m; i++) {
        cout << "Machine or Resource " << i + 1 << ": ";
        cin >> timeAvailable[i];
    }

    vector<vector<double>> timeUsed(m, vector<double>(n));
    cout << "\nEnter Time (Mintues) used by each product on each machine:\n";
    for (int i = 0; i < m; i++) {
        cout << "\nMachine " << i + 1 << ":\n";
        for (int j = 0; j < n; j++) {
            cout << "  Product " << j + 1 << ": ";
            cin >> timeUsed[i][j];
        }
    }

    cout << "\n================ MATHEMATICAL MODEL ================\n\n";
    cout << "Max Z = ";
    for (int i = 0; i < n; i++) {
        cout << profit[i] << "x" << i + 1;
        if (i != n - 1) cout << " + ";
    }
    cout << "\n\nSubject to:\n";

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            cout << timeUsed[i][j] << "x" << j + 1;
            if (j != n - 1) cout << " + ";
        }
        cout << " <= " << timeAvailable[i] << endl;
    }

    cout << "\n";
    for (int i = 0; i < n; i++) {
        cout << "x" << i + 1;
        if (i != n - 1) cout << ", ";
    }
    cout << " >= 0\n";
    cout << "====================================================\n";

    int rows = m + 1;
    int cols = n + m + 1;
    vector<vector<double>> table(rows, vector<double>(cols, 0));

    vector<int> basicVars(m);
    for (int i = 0; i < m; i++) {
        basicVars[i] = n + i;  
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            table[i][j] = timeUsed[i][j];
        table[i][n + i] = 1;
        table[i][cols - 1] = timeAvailable[i];
    }

    for (int j = 0; j < n; j++)
        table[m][j] = -profit[j];

    int iteration = 0;
    printTable(table, n, m, iteration, basicVars, profit);

    while (!isOptimal(table)) {
        int pivotCol = getPivotColumn(table);
        int pivotRow = getPivotRow(table, pivotCol);

        if (pivotRow == -1) {
            cout << "\nUnbounded Solution!\n";
            return 0;
        }

        string enteringVar = (pivotCol < n) ? "x" + to_string(pivotCol + 1) : "s" + to_string(pivotCol - n + 1);
        string leavingVar;
        if (basicVars[pivotRow] < n)
            leavingVar = "x" + to_string(basicVars[pivotRow] + 1);
        else
            leavingVar = "s" + to_string(basicVars[pivotRow] - n + 1);

        cout << "\n>>> Entering Variable: " << enteringVar << " (Column " << pivotCol + 1 << ")\n";
        cout << ">>> Leaving Variable: " << leavingVar << " (Row " << pivotRow + 1 << ")\n";
        cout << ">>> Pivot Element: " << fixed << setprecision(2) << table[pivotRow][pivotCol] << "\n";

        basicVars[pivotRow] = pivotCol;

        pivotOperation(table, pivotRow, pivotCol);

        iteration++;
        printTable(table, n, m, iteration, basicVars, profit);
    }
    vector<double> solution = getVariableValues(table, n);

    cout << "\n=========== OPTIMAL SOLUTION ===========\n\n";
    for (int i = 0; i < n; i++)
        cout << "x" << i + 1 << " = " << fixed << setprecision(2) << solution[i] << endl;

    cout << "\nMaximum Profit (Z) = " << fixed << setprecision(2) << table[rows - 1][cols - 1] << endl;
    cout << "\n========================================\n";

    cout << "\nPress ENTER to exit...";
    cin.ignore();
    cin.get();

    return 0;
}