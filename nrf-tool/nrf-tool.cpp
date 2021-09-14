#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <json.h>
#include <algorithm>
#include "terminal_colors.h"
#include "about.h"
#include <iomanip>

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
void cout_terminal_color(string color, string output)
{
    cout << color << output << endl;
    cout << TERMINAL_COLOR_RESET;
}
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
void run_build(string project_target)
{
    string command = "ninja " + project_target;
    cout << command << endl;
    system(command.c_str());
}
void nrf_build(Json::Value root, string project_target)
{
    char buf[4096];
    cwd(buf, sizeof(buf));

    cd(root["sdkBuildDir"].asString().c_str());

    run_build(project_target);

    cd(buf);
}
void run_merge_hex(string project_target)
{
    string command = "ninja merge_" + project_target;
    cout << command << endl;
    system(command.c_str());
}
void nrf_merge_hex(Json::Value root, string project_target)
{
    char buf[4096];
    cwd(buf, sizeof(buf));

    cd(root["sdkBuildDir"].asString().c_str());

    run_merge_hex(project_target);

    cd(buf);
}
void run_flash(string project_target)
{
    string command = "ninja flash_" + project_target;
    cout << command << std::endl;
    system(command.c_str());

}
void nrf_flash_project(Json::Value root, string project_target)
{
    char buf[4096];
    cwd(buf, sizeof(buf));

    cd(root["sdkBuildDir"].asString().c_str());

    run_flash(project_target);

    cd(buf);
}
void run_config(string cmsis_path, string sdk_config_path)
{
    string command = "java -jar " + cmsis_path + " " + sdk_config_path;
    cout << TERMINAL_COLOR_YELLOW << "Warning: CMSIS Configurator blocks terminal input" << endl;
    cout << "Make sure to close out after use" << TERMINAL_COLOR_RESET << endl;
    cout << command << endl;
    system(command.c_str());
}
void nrf_config(Json::Value root)
{
    string cmsis_path = root["cmsisPath"].asString();
    string sdk_config_path = root["sdkConfPath"].asString();
    
    if(cmsis_path.empty())
    {
        cout << "Given path to cmsis configurator is empty!" << endl;
        return;
    }
    if(sdk_config_path.empty())
    {
        cout << "Given Path to sdk_config.h is empty!" << endl;
        return;
    }

    run_config(cmsis_path, sdk_config_path);

}
void nrf_default(Json::Value root, string project_target)
{
    char buf[4096];
    cwd(buf, sizeof(buf));

    cd(root["sdkBuildDir"].asString().c_str());
    print_cwd();

    run_build(project_target);
    run_merge_hex(project_target);
    run_flash(project_target);

    cd(buf);
}
void nrf_help()
{
    const map<string, string> help ={
        {"build",  "Use Ninja to build the application into hex file"},
        {"merge" , "Merge soft device hex and application hex into one file"},
        {"flash" , "Use Ninja target to flash to MCU"},
        {"config" , "Open an instance of the CMSIS Configuration Wizzard"},
        {"--version" ,  "Get the version number of the nRF Tool"}
    };
    
    cout << TERMINAL_COLOR_GREEN << endl;
    cout << "nRF-Tool Help" << endl;
    cout << "------------------------------------------------" << endl;

    for(auto const &x : help)
    {
         cout << x.first    // string (key)
              << '\t'
              << x.second   // string's value 
              << endl;
    }

    cout << TERMINAL_COLOR_RESET << endl;
}
void nrf_rtt_viewer(Json::Value root)
{
    string rtt_viewer_path = "\"" + root["rttViewerPath"].asString() + "\"";

    if(rtt_viewer_path.empty())
    {
        cout << TERMINAL_COLOR_RED << "RTT Viewer Path Empty" << endl;
        cout << TERMINAL_COLOR_RESET << endl;
        return;
    }
    system(rtt_viewer_path.c_str());
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
        }
        if (args.size() == 1)
        {
            //run default program
            nrf_default(root, project_target);
        }
        else
        {
            for (int i = 0; i < args.size(); i++)
            {
                

                if (args[i] == "config")
                {
                    cout << "its config" << endl;
                    nrf_config(root);
                }
                else if (args[i] == "build")
                {
                    
                    nrf_build(root, project_target);
                }
                else if(args[i] == "merge")
                {
                    nrf_merge_hex(root, project_target);
                }
                else if (args[i] == "flash")
                {
                    nrf_flash_project(root, project_target);
                }
                else if(args[i] == "--version")
                {
                    cout << TERMINAL_COLOR_GREEN << "Version " << NRF_TOOL_VERSION << endl;
                    cout << TERMINAL_COLOR_RESET << endl;
                }
                else if(args[i] == "help" || args[i] == "man")
                {
                    nrf_help();
                }   
                else if(args[i] == "rtt-viewer")
                {
                    nrf_rtt_viewer(root);
                }
            }
        }

        return EXIT_SUCCESS;
    }

    return 0;
}
