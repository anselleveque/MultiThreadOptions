This program simulates monte carlo runs on multiple computer threads, pricing european options
It is then verified against the closed form black-scholes, as an example
This can also be expanded to options without a closed-form solution, as the multithreading hardware can easily be transferred over
For now, it is quite concrete, as you must edit the code in order to change the strike/maturity surface, but I am implementing reading CSV files to make it slightly more flexible.
