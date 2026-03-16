#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

using namespace std;

const string charset = "abcdefghijklmnopqrstuvwxyz0123456789";

const int MIN_LEN = 8;
const int MAX_LEN = 9;

atomic<unsigned long long> counter(0);

unsigned long long pow36(int p)
{
    unsigned long long r = 1;
    for(int i=0;i<p;i++) r*=36;
    return r;
}

void index_to_password(unsigned long long n,int len,char* out)
{
    for(int i=len-1;i>=0;i--)
    {
        out[i] = charset[n % 36];
        n /= 36;
    }
}

void worker(unsigned long long start,
            unsigned long long end,
            int len)
{
    char pwd[16];

    for(unsigned long long i=start;i<end;i++)
    {
        index_to_password(i,len,pwd);

        fwrite(pwd,1,len,stdout);
        fwrite("\n",1,1,stdout);

        counter++;
    }
}

int main()
{
    ios::sync_with_stdio(false);

    int threads = thread::hardware_concurrency();

    for(int len=MIN_LEN; len<=MAX_LEN; len++)
    {
        unsigned long long total = pow36(len);
        unsigned long long chunk = total / threads;

        vector<thread> workers;

        for(int t=0;t<threads;t++)
        {
            unsigned long long start = t * chunk;
            unsigned long long end = (t==threads-1) ? total : start + chunk;

            workers.emplace_back(worker,start,end,len);
        }

        for(auto &t : workers)
            t.join();
    }
}
