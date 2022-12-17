// SC_ARGS -lboost_regex
// Jordan Dehmel, 2022, jdehmel@outlook.com, github.com/jorbDehmel

// TODO: Implement age-checking so as not to compile things that don't need it

#include <iostream>
#include <string>
#include <fstream>
#include <boost/regex.hpp>

#include "tags.hpp"

using namespace std;
using namespace boost;

//////////////////////////////////

string CC = "clang++";
string ARGS = "";

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

bool isMain(const string &path, string &args)
{
    ifstream f(path);
    if (!f.is_open())
    {
        cout << tags::red_bold << "Could not open file.\n"
             << tags::reset;
        return false;
    }

    string text, line;

    getline(f, text);
    if (regex_match(text, regex("// SC_ARGS .*")))
    {
        cout << tags::violet_bold << "Found arguments " << text.substr() << ".\n"
             << tags::reset;

        args += ' ' + text.substr(11);
    }

    while (getline(f, line))
    {
        text += line + '\n';
    }
    f.close();

    return regex_search(text, regex("int main"));
}

vector<string> makeObjs()
{
    cout << tags::green_bold
         << "Preparing to compile...\n"
         << tags::reset;

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

            if (isMain(line, ARGS))
            {
                cout << tags::violet_bold << "File " << line << " has a main.\n"
                     << tags::reset;

                line = regex_replace(line, regex(".*/"), "");
                objs.push_back(".build/__MAIN__" + regex_replace(line, regex("\\.cpp"), ".o"));
            }
            else
            {
                cout << tags::violet_bold << "File " << line << " has no main.\n"
                     << tags::reset;

                objs.push_back(".build/" + regex_replace(line, regex("\\.cpp"), ".o"));
            }

            smartSystem(CC + " -c " + line + " -o " + objs[objs.size() - 1]);
        }
    }

    fin.close();
    system("rm .build/temp.txt");

    cout << tags::green_bold
         << "Compiled " << objs.size() << " objects.\n"
         << tags::reset;

    return objs;
}

void link(const vector<string> objs)
{
    cout << tags::green_bold
         << "Linking...\n"
         << tags::reset;

    string command = CC + ' ' + ARGS + ' ';

    vector<string> mainObjs;

    for (string o : objs)
    {
        if (regex_search(o, regex("/__MAIN__")))
            mainObjs.push_back(o);
        else
            command += o + ' ';
    }

    for (string mainO : mainObjs)
    {
        string name = regex_replace(mainO, regex(".*/__MAIN__"), "");
        name = regex_replace(name, regex("\\.o"), ".out");

        smartSystem(command + mainO + " -o bin/" + name);
    }

    cout << tags::green_bold
         << "Successfully linked.\n"
         << tags::reset;

    return;
}

//////////////////////////////////

int main(const int argc, const char *argv[])
{
    try
    {
        cout << tags::blue_bold
             << "~~~~~~~~~~~~~~~~~~~~\n"
             << "SmartCompile for C++\n"
             << "~~~~~~~~~~~~~~~~~~~~\n"
             << tags::reset;

        string tag;
        for (int i = 1; i < argc; i++)
        {
            tag = string(argv[i]);
            if (tag == "-u" || tag == "--use")
            {
                i++;
                CC = argv[i];
            }
            else if (tag == "-org")
            {
                smartSystem("mkdir -p headers && mv *.h headers");
                smartSystem("mkdir -p source && mv *.cpp source");
                smartSystem("mkdir -p bin && mv *.out bin");
                smartSystem("mkdir -p .build && mv *.o .build");
            }
            else if (tag == "-c" || tag == "--clean")
            {
                smartSystem("rm -f .build/*.o");
            }
            else if (tag == "-p" || tag == "--purge")
            {
                smartSystem("rm -f .build/*.o bin/*.out");
            }
            else if (tag == "-jc" || tag == "--justclean")
            {
                smartSystem("rm -f .build/*.o");
                return 0;
            }
            else if (tag == "-jp" || tag == "--justpurge")
            {
                smartSystem("rm -f .build/*.o bin/*.out");
                return 0;
            }
            else if (tag == "-h" || tag == "--help")
            {
                cout << tags::violet_bold
                     << "Short version | Long version | Description\n"
                     << "------------------------------------------\n"
                     << "-u            | --use <CC>   | changes compilers\n"
                     << "              | -org         | organizes files\n"
                     << "-h            | --help       | shows this page\n"
                     << "              | -<argname>   | adds an argument to the compiler call\n"
                     << "-c            | --clean      | removes .o files\n"
                     << "-p            | --purge      | removes .o and .out files\n"
                     << "-jc           | --justremove | cleans without compiling or linking\n"
                     << "-jp           | --justpurge  | purges without compiling or linking\n"
                     << "\nAutotagging:\n"
                     << "   Insert \"// SC_ARGS <tags go here>\" as the beginning of\n"
                     << "   a .cpp file to activate autotagging.\n"
                     << "\nAutolinking:\n"
                     << "   SmartCompile will automatically detect .cpp files with\n"
                     << "   main functions and link them appropriately.\n"
                     << "\n(Jorb Dehmel, 2022, jdehmel@outlook.com)\n"
                     << tags::reset;

                return 0;
            }
            else if (tag[0] == '-')
            {
                ARGS += ' ' + tag;
            }
        }

        smartSystem("mkdir -p bin .build");

        link(makeObjs());

        cout << tags::green_bold << '\n'
             << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
             << "Completed. (Output .out file(s) can be found in ./bin/ )\n"
             << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
             << tags::reset;
    }
    catch (runtime_error &e)
    {
        cout << tags::red_bold
             << "\nFatal runtime error encountered; Compilation canceled.\n"
             << tags::reset
             << e.what()
             << '\n';

        return 1;
    }
    catch (...)
    {
        cout << tags::red_bold
             << "\nFatal error encountered; Compilation canceled.\n"
             << tags::reset;

        return 1;
    }

    return 0;
}
