#include <iostream>
#include <vector>
#include <cmath>
#include <atomic>
#include <thread>
#include <random>
#include <chrono>
#include <iomanip>

struct Cell{
    double strike;
    double time_to_expiry;
};

//Get SE and price estimates for MC runs
struct Estimate{
    double price=0.0;
    double std_error=0.0;
};

//Define market parameters
struct Market{
    double S0=100.0; //spot price
    double rate=0.05; //Risk free rate
    double sigma=0.20; //volatility
    long paths=200'000; //num of monte carlo runs
};

static double norm_cdf(double x){
    return 0.5*std::erfc(-x/std::sqrt(2.0));
};

//Closed-form black-scholes call to check work
static double bs_call(double S0, double K, double r, double sig, double T){
    const double d2=-((std::log(K/S0)-(r-0.5*sig*sig)*T)/(sig*std::sqrt(T)));
    const double d1=d2+sig*std::sqrt(T);
    return S0*norm_cdf(d1)-K*std::exp(-r*T)*norm_cdf(d2); //Return C (payoff)
};

static Estimate mc_cell(const Market& mkt, const Cell& cell, std::mt19937& rng){
    std::normal_distribution<double> z(0.0,1.0);
    
}

int main(){
    const double s0=100.0,rate=0.05,sigma=0.20;
    const long paths=200'000; //Monte carlo paths per cell
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
