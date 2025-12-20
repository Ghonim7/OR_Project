#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <cmath>
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
    centerText("$$ PROFIT CALCULATE $$");
    centerText("$$$$$$$$$$$$$$$$$$$$$$");
    cout << "\n";
    centerText("Business Optimization");
    centerText("[ Profit | Resources | Machines ]");
}

struct Point {
    double x, y;
};


bool exists(const vector<Point> &v, Point p) { 
    for (int i = 0; i < v.size(); i++) {
        if (v[i].x == p.x && v[i].y == p.y)
            return true;
    }
    return false;
}



bool feasible(double x, double y,
    const vector<vector<double>>& A,
    const vector<double>& b) {

    if (x < 0 || y < 0) return false;

    for (int i = 0; i < b.size(); i++) {
        if (A[i][0] * x + A[i][1] * y > b[i] + 1e-6)
            return false;
    }
    return true;
}

int main() {

    drawScreen();
    cout << "\n\n";
    centerText("Press ENTER to start...");
    cin.get();

    int n;
    cout << "\nEnter number of products: ";
    cin >> n;

    vector<double> profit(n);
    cout << "\nEnter profit per unit for each product:\n";
    for (int i = 0; i < n; i++) {
        cout << "Product " << i + 1 << ": ";
        cin >> profit[i];
    }

    int m;
    cout << "\nEnter number of machines: ";
    cin >> m;

    vector<double> timeAvailable(m);
    cout << "\nEnter available time (Mintues) for each machine:\n";
    for (int i = 0; i < m; i++) {
        cout << "Machine " << i + 1 << ": ";
        cin >> timeAvailable[i];
    }

    vector<vector<double>> timeUsed(m, vector<double>(n));
    cout << "\nEnter time (Mintues) used by each product on each machine:\n";
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            cout << "Machine " << i + 1 << ", Product " << j + 1 << ": ";
            cin >> timeUsed[i][j];
            
        }
        
    }

    cout << "\n====================================\n";
    cout << "Mathematical Model:\n\n";

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

    cout << "\n====================================\n";
    cout << "Solution:\n\n";

    if (n != 2) {
        cout << "Graphical method works only for 2 variables.\n";
        cout << "Please use Simplex Method.\n";
    }
    else {

        vector<Point> candidates;

        Point origin{ 0,0 };
        if (!exists(candidates, origin))
            candidates.push_back(origin);

        
        for (int i = 0; i < m; i++) {
            if (timeUsed[i][0] != 0) {
                Point p{ timeAvailable[i] / timeUsed[i][0], 0 };
                if (!exists(candidates, p))
                    candidates.push_back(p);
            }

            if (timeUsed[i][1] != 0) {
                Point p{ 0, timeAvailable[i] / timeUsed[i][1] };
                if (!exists(candidates, p))
                    candidates.push_back(p);
            }
        }

        
        for (int i = 0; i < m; i++) {
            for (int j = i + 1; j < m; j++) {

                double a1 = timeUsed[i][0], b1 = timeUsed[i][1], c1 = timeAvailable[i];
                double a2 = timeUsed[j][0], b2 = timeUsed[j][1], c2 = timeAvailable[j];

                double det = a1 * b2 - a2 * b1;
                if (det != 0) {
                    double x = (c1 * b2 - c2 * b1) / det;
                    double y = (a1 * c2 - a2 * c1) / det;

                    Point p{ x, y };
                    if (!exists(candidates, p))
                        candidates.push_back(p);
                }
            }
        }

        double maxZ = -1;
        Point best{ 0,0 };

        for (int i = 0; i < candidates.size(); i++) {
            Point p = candidates[i];
            if (feasible(p.x, p.y, timeUsed, timeAvailable)) {
                double Z = profit[0] * p.x + profit[1] * p.y;
                cout << "x1 = " << p.x << ", x2 = " << p.y
                    << "  =>  Z = " << Z << endl;

                if (Z > maxZ) {
                    maxZ = Z;
                    best = p;
                }
            }
        }

        cout << "\nOptimal Solution:\n";
        cout << "x1 = " << best.x << endl;
        cout << "x2 = " << best.y << endl;
        cout << "Max Z = " << maxZ << endl;
    }

    cout << "\n====================================\n";
    cout << "Press ENTER to exit...";
    cin.ignore();
    cin.get();

    return 0;
}
