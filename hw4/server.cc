// server.cc
#include "crow_all.h"
#include "cache.hh"
#include "json.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <time.h>


int MAXMEM = 1080;
int PORTNUM = 17017;
std::unique_ptr<Cache> CACHE_PTR;

int exit(crow::SimpleApp app) {
    app.stop();
}

int main(int argc, char *argv[])
{
    crow::SimpleApp app;

    // Parse command line args
    int c;
    while( ( c = getopt (argc, argv, "m:t:") ) != -1 ) 
    {
        switch(c)
        {
            case 'm':
                if(optarg) MAXMEM = std::atoi(optarg);
                break;
            case 't':
                if(optarg) PORTNUM = std::atoi(optarg) ;
                break;
        }
    }
    
    CACHE_PTR.reset(new Cache(MAXMEM));

    app.route_dynamic("/memsize")
    .methods("GET"_method)
    ([](const crow::request& req) {
        if (req.method == "GET"_method){
            Cache::index_type mem = (*CACHE_PTR).space_used();
            crow::json::wvalue return_json;
            return_json["memused"] = mem;
            return crow::response{return_json};
        }
    });

    app.route_dynamic("/key/<string>/<int>")
    .methods("PUT"_method)
    ([](const crow::request& req,std::string key, int val){
        if (req.method == "PUT"_method){
            Cache::val_type val_point = &val;
            uint32_t val_size = sizeof(val);
            int set_return = (*CACHE_PTR).set(key,val_point,val_size);
            if (set_return != 0) {
                return(crow::response(400,"400 COULD NOT INSERT KEY/VALUE\n"));
            }
            Cache::index_type memused = (*CACHE_PTR).space_used();
            crow::json::wvalue return_json;
            return_json["value"] = val;
            return_json["key"] = key;
            return crow::response{return_json};
            } else {
            return(crow::response(400));
        }
    });
    
    app.route_dynamic("/key/<string>") 
    .methods("DELETE"_method,"GET"_method,"HEAD"_method)
    ([](const crow::request& req,std::string key) {
        if (req.method == "DELETE"_method)
        {
            Cache::index_type delete_return = (*CACHE_PTR).del(key);
            if (delete_return != 0)
            {
                return(crow::response(400,"400 KEY NOT IN CACHE \n"));
            }
            return crow::response(200,"200 VALUE DELETED");
        } else if (req.method == "GET"_method) {
            Cache::index_type sized;
            Cache::val_type the_point = (*CACHE_PTR).get(key, sized);
            if (the_point == NULL)
            {
                return(crow::response(400,"400 KEY NOT IN CACHE \n"));
            }
            const uint32_t &val = *(static_cast<const u_int32_t *>(the_point));
            crow::json::wvalue return_json;
            return_json["value"] = val;
            return_json["key"] = key;
            return crow::response{return_json};
        } else if (req.method == "HEAD"_method) {
            crow::response resp;
            resp.add_header("Accept","text/plain");
            resp.add_header("Accept-Charset","utf-8");
            resp.add_header("Content-Type","text/plain");
            return resp;

        } else {
            return(crow::response(400));
        }
    });

    app.route_dynamic("/shutdown")
    .methods("POST"_method)
    ([&app](const crow::request& req) {
        if (req.method == "POST"_method) {
            app.stop();
            return(crow::response(200,"200 SHUTTING DOWN \n"));
        }
    });


    app.port(PORTNUM)
        .run();
}