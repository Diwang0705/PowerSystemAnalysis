#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>

struct Complex {
    double r, i;
    Complex(double r=0, double i=0) : r(r), i(i) {}
    Complex operator+(const Complex& o) const { return Complex(r+o.r, i+o.i); }
    Complex operator-(const Complex& o) const { return Complex(r-o.r, i-o.i); }
    Complex operator*(const Complex& o) const { return Complex(r*o.r - i*o.i, r*o.i + i*o.r); }
    Complex conj() const { return Complex(r, -i); }
    double mag() const { return std::sqrt(r*r + i*i); }
    double ang() const { return std::atan2(i, r); }
};

// solve 4*4 linear system (Gaussian elimination)
static void solve4x4(double A[4][4], double b[4], double x[4]) {
    double M[4][5];
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++) M[i][j] = A[i][j];
        M[i][4] = b[i];
    }
    for(int k=0;k<4;k++){
        double piv = M[k][k];
        for(int j=k;j<5;j++) M[k][j] /= piv;
        for(int i=0;i<4;i++){
            if(i==k) continue;
            double f = M[i][k];
            for(int j=k;j<5;j++)
                M[i][j] -= f*M[k][j];
        }
    }
    for(int i=0;i<4;i++) x[i] = M[i][4];
}

// Newton-Raphson Power Flow (3-busm 1 slack, 2 PQ)
void NRPowerFlow(
    const std::vector<std::vector<Complex>>& Y,
    const std::vector<double>& Pspec,
    const std::vector<double>& Qspec,
    const std::vector<int>& busType
){
    double delta[3] = {0,0,0};
    double Vm[3]    = {1,1,1};

    int maxIter = 20;
    double tol = 1e-6;
for(int iter=0; iter<maxIter; iter++) {

        // Build V phasors
        Complex V[3];
        for(int i=0;i<3;i++)
            V[i] = Complex(Vm[i]*cos(delta[i]), Vm[i]*sin(delta[i]));

        // Compute P,Q
        double P[3], Q[3];
        for(int i=0;i<3;i++){
            Complex I(0,0);
            for(int k=0;k<3;k++)
                I = I + Y[i][k]*V[k];
            Complex S = V[i]*I.conj();
            P[i] = S.r;
            Q[i] = S.i;
        }

        // Mismatch vector
        double d[4];
        d[0] = Pspec[1] - P[1];
        d[1] = Pspec[2] - P[2];
        d[2] = Qspec[1] - Q[1];
        d[3] = Qspec[2] - Q[2];

        double norm = fabs(d[0])+fabs(d[1])+fabs(d[2])+fabs(d[3]);
        if(norm < tol){
            std::cout << "Converged in " << iter << " iterations\n";
            break;
        }

        // Jacobian 4*4
        double J[4][4];
        auto G = [&](int i,int k){ return Y[i][k].r; };
        auto B = [&](int i,int k){ return Y[i][k].i; };

        // H = dP/dδ
        for(int i=1;i<=2;i++){
            for(int k=1;k<=2;k++){
                if(i==k){
                    double sum=0;
                    for(int m=0;m<3;m++){
                        if(m==i) continue;
                        sum += Vm[i]*Vm[m] *
                            (-G(i,m)*sin(delta[i]-delta[m]) +
                              B(i,m)*cos(delta[i]-delta[m]));
                    }
                    J[i-1][k-1] = sum;
                } else {
                    J[i-1][k-1] =
                        Vm[i]*Vm[k] *
                        ( G(i,k)*sin(delta[i]-delta[k]) -
                          B(i,k)*cos(delta[i]-delta[k]) );
                }
            }
        }

        // N = dP/dV
        for(int i=1;i<=2;i++){
            for(int k=1;k<=2;k++){
                int col = 2+(k-1);
                if(i==k){
                    double sum=0;
                    for(int m=0;m<3;m++)
                        sum += Vm[m] *
                            ( G(i,m)*cos(delta[i]-delta[m]) +
                              B(i,m)*sin(delta[i]-delta[m]) );
                    J[i-1][col] = sum + 2*Vm[i]*G(i,i);
                } else {
                    J[i-1][col] =
                        Vm[i] *
                        ( G(i,k)*cos(delta[i]-delta[k]) +
                          B(i,k)*sin(delta[i]-delta[k]) );
                }
            }
        }

        // M = dQ/dδ
        for(int i=1;i<=2;i++){
            for(int k=1;k<=2;k++){
                int row = 2+(i-1);
                if(i==k){
                    double sum=0;
                    for(int m=0;m<3;m++){
                        if(m==i) continue;
                        sum += Vm[i]*Vm[m] *
                            ( G(i,m)*cos(delta[i]-delta[m]) +
                              B(i,m)*sin(delta[i]-delta[m]) );
                    }
                    J[row][k-1] = -sum;
                } else {
                    J[row][k-1] =
                        -Vm[i]*Vm[k] *
                        ( G(i,k)*cos(delta[i]-delta[k]) +
                          B(i,k)*sin(delta[i]-delta[k]) );
                }
            }
        }

        // L = dQ/dV
        for(int i=1;i<=2;i++){
            for(int k=1;k<=2;k++){
                int row = 2+(i-1);
                int col = 2+(k-1);
                if(i==k){
                    double sum=0;
                    for(int m=0;m<3;m++)
                        sum += Vm[m] *
                            ( G(i,m)*sin(delta[i]-delta[m]) -
                              B(i,m)*cos(delta[i]-delta[m]) );
                    J[row][col] = -sum - 2*Vm[i]*B(i,i);
                } else {
                    J[row][col] =
                        -Vm[i] *
                        ( G(i,k)*sin(delta[i]-delta[k]) -
                          B(i,k)*cos(delta[i]-delta[k]) );
                }
            }
        }

        // Solve Jx = d
        double x[4];
        solve4x4(J, d, x);

        // Update state
        delta[1] += x[0];
        delta[2] += x[1];
        Vm[1]    += x[2];
        Vm[2]    += x[3];
    }

    // Final voltages
    Complex V[3];
    for(int i=0;i<3;i++)
        V[i] = Complex(Vm[i]*cos(delta[i]), Vm[i]*sin(delta[i]));

    // Compute active and reactive power at each bus
    double P[3], Q[3];
    for (int i = 0; i < 3; i++) {
        Complex I(0, 0);
        for (int k = 0; k < 3; k++)
            I = I + Y[i][k] * V[k];
        Complex S = V[i] * I.conj();
        P[i] = S.r;
        Q[i] = S.i;
    }

    std::cout << std::fixed << std::setprecision(5);
    for(int i=0;i<3;i++){
        std::cout << "Bus " << i+1
                  << ": |V|=" << V[i].mag()
                  << "  angle=" << V[i].ang()*180/M_PI << " deg\n"
                  << "  P=" << P[i]
                  << "  Q=" << Q[i] << "\n";
    }
}