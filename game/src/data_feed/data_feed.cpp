#include <stdio.h>
#include <assert.h>
#include <mxml.h>
#include <iostream>
#include "data_feed.h"

DataSource::DataSource(string &loc)
{
    location = loc;
    value = 0.0f;
}

DataSource::~DataSource()
{

}

float DataSource::poll()
{
    FILE *fp = fopen(location.c_str(), "r");
    assert(fp);

    mxml_node_t *tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    assert(tree);

    mxml_node_t *latest_update;
    latest_update = mxmlFindElement(tree,
                                    tree,
                                    "ob",
                                    "utime",
                                    NULL,
                                    MXML_DESCEND);

    //this should be the most up to date update
    if(latest_update)
    {
        int utime = atoi(mxmlElementGetAttr(latest_update, "utime"));
        cout<<"utime: "<<utime<<endl;

        //loop through all the variables
        mxml_node_t *var_node = NULL;
        mxml_node_t *start_node = latest_update;
        do
        {
            var_node = mxmlFindElement( start_node,
                                        latest_update,
                                        "variable",
                                        "var",
                                        NULL,
                                        MXML_DESCEND);
            if(var_node)
            {
                cout<<mxmlElementGetAttr(var_node, "var");
                cout<<"\t\t"<<mxmlElementGetAttr(var_node, "value");
                cout<<endl;
            }
            start_node = var_node;
        } while(var_node);
    }

    fclose(fp);
    return 0.0f;
}


/*
DataFeed::DataFeed()
{
    polling_rate = 0.1f;
}

DataFeed::~DataFeed()
{

}

float DataFeed::query()
{

}

void DataFeed::init()
{

}

void DataFeed::simulate(const float t)
{

}*/
