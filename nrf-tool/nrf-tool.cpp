#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <json.h>
#include <algorithm>

#ifdef _WIN32
#include <direct.h>
// MSDN recommends against using getcwd & chdir names
#define cwd _getcwd
#define cd _chdir
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#endif

#define NRF_TOOL_TAG "[NRF TOOL]"

const char *user_commands[4] = {"build", "flash", "config", "help"};
const char *ninja_commands[3] = {"", "merge_", "flash_"};

using namespace std;

void display_args(int argc, char *argv[])
{
    cout << "Args:" << endl;
    for (int i = 0; i < argc; i++)
    {
        cout << argv[i] << endl;
    }
}
void print_cwd()
{
    char cwd_buf[4096];
    cout << "CWD: " << cwd(cwd_buf, sizeof(cwd_buf)) << endl;
}

void nrf_default(Json::Value root, string project_target)
{
    char buf[4096];
    cwd(buf, sizeof(buf));

    string current_directory(buf);

    cd(root["sdkBuildDir"].asString().c_str());
    print_cwd();

    string command = "ninja flash_" + project_target;
    cout << command << std::endl;
    system(command.c_str());

    cd(current_directory.c_str());
}
int main(int argc, char *argv[])
{

    // display_args(argc, argv);

    ifstream config_file;
    vector<string> config_params;

    Json::Value root;
    Json::CharReaderBuilder builder;

    vector<string> args(argv, argv + argc);

    config_file.open("build_flash_config.json");

    if (!config_file.is_open())
    {
        cout << "Cannot open file!" << endl;
        cout << "Exiting Program" << endl;
        return EXIT_FAILURE;
    }
    else
    {
        cout << NRF_TOOL_TAG << " "
             << "Reading Parameters!" << endl;

        Json::CharReaderBuilder builder;
        builder["collectComments"] = false;
        JSONCPP_STRING errs;

        bool parse_success = parseFromStream(builder, config_file, &root, &errs);
        config_file.close();

        // attempt to parse the json file
        if (!parse_success)
        {
            std::cout << errs << std::endl;
            return EXIT_FAILURE;
        }

        char buf[4096];
        cwd(buf, sizeof(buf));

        string current_directory(buf);

        cd(root["sdkBuildDir"].asString().c_str());
        print_cwd();

        string project_target = root["projectTarget"].asString();

        if (project_target.empty())
        {
            cout << "Could not find project target!" << endl;
            cout << "make sure the format is projectTarget: <target>" << endl;
            cout << "Exiting program" << endl;
            cd(current_directory.c_str());
            return EXIT_FAILURE;
        }
        
        //put it all to lower case
        for (int i = 1; i < args.size(); i++)
        {
            transform(args[i].begin(), args[i].end(), args[i].begin(), [](unsigned char c)
                      { return std::tolower(c); });
            cout << args[i] << endl;
        }
        for (int i = 0; i < args.size(); i++)
        {
            if(args[i] == "config")
            {
                cout << "its config" << endl;
            }
            else if(args[i] == "flash")
            {
                cout << "its flash" << endl;
            }
            else if(args[i] == "build")
            {
                cout << "its build" << endl;
            }
        }

        return EXIT_SUCCESS;
    }

    return 0;
}
