#include <iostream>
#include <vector>
#include <cmath>
#include <atomic>
#include <thread>
#include <random>
#include <chrono>
#include <iomanip>


static double norm_cdf(double x){return 0.5*std::erfc(-x/std::sqrt(2.0));};


int main(){
    std::vector<double> maturities{0.25,0.5,1.0,2.0}; //Maturity times in years
    std::vector<double> strikes;
    for (int i=80;i<=120;i+=5){
        strikes.push_back(static_cast<double>(i)); //Strike prices (spot 100)
    }

    struct Cell{
        double strike;
        double time_to_expiry;
    };
    std::vector<Cell> tasks;

    for (double T:maturities){
        for (double K:strikes){
            tasks.push_back({T,K});
        }
    }

    std::vector<double> results(tasks.size(),0);



    

}
