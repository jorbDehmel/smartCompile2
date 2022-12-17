#include <iostream>
#include <string>
#include <fstream>
#include <boost/regex.hpp>

#include "tags.hpp"

using namespace std;

using boost::regex;
using boost::regex_match;
using boost::regex_replace;

//////////////////////////////////

string BUILD_DIR = "./.build";

string CC = "clang++";
string ARGS = "-pedantic -Wall";
string NAME = "./a.out";

//////////////////////////////////

void tassert(bool in)
{
    if (!in)
    {
        throw runtime_error("tassert failed");
    }
    return;
}

void smartSystem(const char *command)
{
    cout << command << '\n';
    tassert(system(command) == 0);
    return;
}

void smartSystem(string command)
{
    smartSystem(command.c_str());
    return;
}

//////////////////////////////////

vector<string> makeObjs()
{
    cout << tags::green_bold
         << "Preparing to compile...\n\n"
         << tags::reset;

    string command = "mkdir -p " + BUILD_DIR;
    smartSystem(command);
    smartSystem("find -print >.build/temp.txt");

    ifstream fin(".build/temp.txt");
    tassert(fin.is_open());

    vector<string> cpps;
    vector<string> objs;
    string line;
    while (getline(fin, line))
    {
        if (regex_match(line, regex(".*?\\.cpp")))
        {
            cpps.push_back(line);
            objs.push_back(BUILD_DIR + '/' + regex_replace(line, regex("\\.cpp"), ".o"));

            command = CC + ' ' + ARGS + " -c " + line + " -o " + objs[objs.size() - 1];
            smartSystem(command);
        }
    }

    fin.close();
    system("rm .build/temp.txt");

    cout << tags::green_bold
         << "\nCompiled " << objs.size() << " objects.\n"
         << tags::reset;

    return objs;
}

void link(const vector<string> objs)
{
    cout << tags::green_bold
         << "Calling linker...\n\n"
         << tags::reset;

    string command = CC + ' ' + ARGS + ' ';

    for (string o : objs)
        command += o + ' ';

    command += "-o " + NAME;

    smartSystem(command);

    cout << tags::green_bold
         << "\nSuccessfully linked.\n"
         << tags::reset;

    return;
}

//////////////////////////////////

int main(const int argc, const char *argv[])
{
    try
    {

        string tag;
        for (int i = 1; i < argc; i++)
        {
            tag = string(argv[i]);
            if (tag == "-u" || tag == "--use")
            {
                i++;
                CC = argv[i];
            }
            else if (tag == "-o" || tag == "--output")
            {
                i++;
                NAME = argv[i];
            }
            else if (tag == "-org")
            {
                smartSystem("mkdir headers && mv *.h headers");
                smartSystem("mkdir source && mv *.cpp source");
                smartSystem("mkdir bin && mv *.out bin");
            }
            else if (tag == "-h" || tag == "--help")
            {
                cout << tags::violet_bold
                     << "Tag | Long version | Purpose\n"
                     << "----------------------------\n"
                     << "-u  | --use <CC>   | changes compilers\n"
                     << "-o  | --output <P> | changes the output path\n"
                     << "    | -org         | organizes files\n"
                     << "-h  | --help       | shows this page\n"
                     << "    | -<argname>   | adds an argument to the compiler call\n"
                     << "(Jorb Dehmel, 2022, jdehmel@outlook.com)\n"
                     << tags::reset;

                return 0;
            }
            else if (tag[0] == '-')
            {
                ARGS += ' ' + tag;
            }
        }

        link(makeObjs());

        cout << tags::green_bold
             << "Completed.\n"
             << tags::reset;
    }
    catch (...)
    {
        cout << tags::red_bold
             << "\nFatal error encountered; Canceling compilation...\n"
             << tags::reset;
        return 1;
    }

    return 0;
}
