#include <switch.h>
#include "Request.hpp"

static struct curl_slist *hosts = NULL;

Net::Net() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    hosts = curl_slist_append(NULL, "dootnode.org:80:91.121.75.178");
}

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

string Net::Request(string method, string url) {
    CURLcode res = CURLE_OK;
    CURL *curl = curl_easy_init();
    string response;
    if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_RESOLVE, hosts);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
    }
    return !res ? response : "{}";
}

bool Net::Download(string url, string filepath) {
    FILE *fp;
    CURLcode res = CURLE_OK;
    CURL *curl = curl_easy_init();
    
    if (curl) {   
        fp = fopen(filepath.c_str(),"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_RESOLVE, hosts);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    } else {
        res = CURLE_HTTP_RETURNED_ERROR;
    }
    return res == CURLE_OK ? false : true;
}