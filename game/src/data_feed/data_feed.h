#ifndef __DATA_FEED_H__
#define __DATA_FEED_H__

#include <string>
#include <vector>

using namespace std;

class DataSource
{
    public:
        DataSource(string &loc);
        ~DataSource();

        float poll();
    private:
        float value;
        string location;

        vector<string> var_names;
        vector<float> var_values;
};

class DataFeed
{
    public:
        DataFeed();
        ~DataFeed();

        float query();

        void init();
        void simulate(const float t);

    private:
        float polling_rate;     //how often we poll the source (times/sec)
        DataSource source;
};

#endif // __DATA_FEED_H__
