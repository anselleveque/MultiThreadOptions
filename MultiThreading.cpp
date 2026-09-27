#include <iostream>
#include <vector>
#include <cmath>
#include <atomic>
#include <thread>
#include <random>
#include <chrono>
#include <iomanip>
#include <algorithm>

struct Cell{
    double strike=0.0;
    double time_to_expiry=0.0;
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
static double bs_call(double S0,double K,double r,double sig,double T){
    const double d2=-((std::log(K/S0)-(r-0.5*sig*sig)*T)/(sig*std::sqrt(T)));
    const double d1=d2+sig*std::sqrt(T);
    return S0*norm_cdf(d1)-K*std::exp(-r*T)*norm_cdf(d2); //Return C (payoff)
};

static Estimate mc_cell(const Market& mkt,const Cell& cell,std::mt19937_64& rng){
    //Local, want each thread to have its own
    std::normal_distribution<double> z(0.0,1.0);

    //Calculate outside individual threads, b/c identical on every path
    const double t=cell.time_to_expiry;
    const double drift=(mkt.rate-(0.5*mkt.sigma*mkt.sigma))*t;
    const double vol_t=mkt.sigma*std::sqrt(t);
    const double discount=std::exp(-mkt.rate*t);
    const double n=static_cast<double>(mkt.paths);

    //Use Welford's algorithm to get mean and var in one pass
    //Doesn't suffer from cancellation regular way does when mean large
    double mean=0.0,m2=0.0;
    for (long i=0;i<mkt.paths;++i){
        //Calculate GBM solution and option contract
        const double st=mkt.S0*std::exp(drift+vol_t*z(rng));
        const double payoff=std::max(st-cell.strike,0.0);

        //Diff between new point and old mean
        const double delta=payoff-mean;
        //Move mean along diff by 1 step
        mean+=delta/(static_cast<double>(i)+1.0);
        //Store sum of squared deviations
        m2+=delta*(payoff-mean);
    }
    const double variance=m2/(n-1.0);

    return {discount*mean,discount*sqrt(variance/n)};
}

static std::vector<Cell> build_tasks(const std::vector<double>& strikes,const std::vector<double>& maturities){
    std::vector<Cell> tasks;
    tasks.reserve(strikes.size()*maturities.size());
    for (double t:maturities)
        for (double k:strikes)
            tasks.push_back({.strike=k,.time_to_expiry=t});
    return tasks;
}

//Seed per cell
static std::uint64_t seed_for_cell(std::size_t index){
    return 104729ULL*index+1; //+1 so no cell 0 seeded with 0
}

//Spread cells among workers
//Return elapsed time
static double fill_results(const Market& mkt,const std::vector<Cell>& tasks,std::vector<Estimate>& out,unsigned nthreads){
    std::atomic<std::size_t>next{ 0 };
    std::fill(out.begin(),out.end(),Estimate{});

    auto worker=[&mkt,&tasks,&out,&next]{
        std::size_t i;
        //Return val before increment, so threads don't recieve same index
        while ((i=next.fetch_add(1))<tasks.size()){
            //Cell's stream only writes to slot i, this thread owns
            std::mt19937_64 rng(seed_for_cell(i));
            out[i]=mc_cell(mkt,tasks[i],rng);
        }
    };

    const auto t0=std::chrono::steady_clock::now();

    std::vector<std::thread> pool;
    pool.reserve(nthreads);
    for (unsigned t=0;t<nthreads;++t)
        pool.emplace_back(worker);
    for (std::thread& th:pool)
        th.join();

    return std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-t0).count();
}

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
