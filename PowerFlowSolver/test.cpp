#include "main.cpp"

int main() {
    // 3 bus power system example
    std::vector<std::vector<Complex>> Y(3, std::vector<Complex>(3));

    auto addLine = [&](int a, int b, double R, double X){
        double denom = R*R + X*X;
        Complex Yline(R/denom, - X/denom);
        Y[a][a] = Y[a][a] + Yline;
        Y[b][b] = Y[b][b] + Yline;
        Y[a][b] = Y[a][b] - Yline;
        Y[b][a] = Y[b][a] - Yline;
    };

    // example system
    addLine(0,1,0.0,0.2);
    addLine(0,2,0.0,0.25);
    addLine(1,2,0.0,0.3);

    // Bus types: 0 = slack, 1 = PQ
    std::vector<int> busType = {0,1,1};

    // P and Q (slack ignored)
    std::vector<double> Pspec = {0.0, -0.5, -0.3};
    std::vector<double> Qspec = {0.0, -0.2, -0.1};

    // Run Newton-Raphson power flow analysis
    NRPowerFlow(Y, Pspec, Qspec, busType);

    return 0;
}