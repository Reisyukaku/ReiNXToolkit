#pragma once
#include <string>
#include <sys/select.h>
#include <curl/curl.h>
#include <switch.h>

using namespace std;

class Net {
    public:
        Net();
        string Request(string method, string url);
        bool Download(string url, string filepath);
};