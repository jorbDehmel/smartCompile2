// SC_ARGS -lboost_regex
// Jordan Dehmel, 2022, jdehmel@outlook.com, github.com/jorbDehmel

#include <iostream>
#include <string>
#include <fstream>
#include <boost/regex.hpp>

#include "tags.hpp"

using namespace std;

using boost::regex;
using boost::regex_replace;

//////////////////////////////////

string CC = "clang++";
string ARGS = "";

//////////////////////////////////

void tassert(bool in)
{
    if (!in)
        throw runtime_error("tassert failed");
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
    // Open file safely
    ifstream f(path);
    if (!f.is_open())
    {
        cout << tags::red_bold << "Could not open file.\n"
             << tags::reset;
        return false;
    }

    // Check for SC_ARGS as opening line
    string line;
    getline(f, line);
    if (line.substr(0, 11) == "// SC_ARGS ")
    {
        cout << tags::violet_bold << "Found arguments " << line.substr() << ".\n"
             << tags::reset;

        args += ' ' + line.substr(11);
        cout << "New CC: " << CC << args << '\n';
    }

    // Scan for main function
    bool hasMain = (line.find("int main") != string::npos);
    while (getline(f, line) && !hasMain)
    {
        if (line.find("int main") != string::npos)
        {
            hasMain = true;
        }
    }

    f.close();

    return hasMain;
}

//////////////////////////////////

vector<string> makeObjs()
{
    cout << tags::green_bold
         << "Preparing to compile...\n"
         << tags::reset;

    // Print all files within the current directory to .build/temp.txt
    // and create .build/temp2.txt for dating files
    smartSystem("find -print >.build/temp.txt; touch .build/temp2.txt");

    // Open list of files and prepare to sort them by type
    ifstream fin(".build/temp.txt");
    tassert(fin.is_open());

    vector<string> cpps;
    vector<string> objs;
    string line;

    while (getline(fin, line))
    {
        // Check if file is a .cpp
        if (line.size() > 4 && line.substr(line.size() - 4, 4) == ".cpp")
        {
            cpps.push_back(line);

            // Check if file contains a main function
            if (isMain(line, ARGS))
            {
                cout << tags::violet_bold << "File " << line << " has a main.\n"
                     << tags::reset;

                // Append to list of obj files to create with __MAIN__ prefex
                string lineTemp = regex_replace(line, regex(".*/"), "");
                objs.push_back(".build/__MAIN__" + regex_replace(lineTemp, regex("\\.cpp"), ".o"));
            }
            else
            {
                cout << tags::violet_bold << "File " << line << " has no main.\n"
                     << tags::reset;

                // Append to list of obj files to create
                string lineTemp = regex_replace(line, regex(".*/"), "");
                objs.push_back(".build/" + regex_replace(lineTemp, regex("\\.cpp"), ".o"));
            }

            // Get the ages of the desired .o file and the source .cpp file
            long int objTime, cppTime;
            try
            {
                smartSystem("stat --format='%Y' " + objs[objs.size() - 1] + " " + cpps[cpps.size() - 1] + " >.build/temp2.txt");
                ifstream ages(".build/temp2.txt");
                tassert(ages.is_open());
                ages >> objTime >> cppTime;
                ages.close();
            }
            catch (const runtime_error &e)
            {
                cout << "(Cannot stat a nonexistant file, moving on)\n";
                objTime = 0;
                cppTime = 10000;
            }

            // If the .o is older than the .cpp, update .o
            if (objTime < cppTime)
            {
                smartSystem(CC + ARGS + " -c " + line + " -o " + objs[objs.size() - 1]);
            }
            else
            {
                cout << tags::green_bold
                     << objs[objs.size() - 1]
                     << " is up to date\n"
                     << tags::reset;
            }
        }
    }

    // Clean up
    fin.close();
    system("rm .build/temp.txt .build/temp2.txt");

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

    // Create base commands for compiling and creating a library
    string command = CC + ARGS + ' ';
    string libCommand = "ar bin/lib.a ";

    // Sort object into main-having and not
    vector<string> mainObjs;
    for (string o : objs)
    {
        if (o.find("/__MAIN__") != string::npos)
            mainObjs.push_back(o);
        else
        {
            command += o + ' ';
            libCommand += o + ' ';
        }
    }

    cout << "Sorted.\n";

    bool hasUpdated = false;
    for (string mainO : mainObjs)
    {
        cout << "Linking from " << mainO << '\n';

        string name = regex_replace(mainO, regex(".*/__MAIN__"), "");
        name = regex_replace(name, regex("\\.o"), ".out");

        cout << "Fixed name: " << name << '\n';

        // Get the ages of the desired .out file and it's source .o file(s)
        long int objTime, outTime;
        try
        {
            cout << "212\t" << mainObjs.size() << '\n';
            smartSystem("stat --format='%Y' " + mainObjs[objs.size() - 1] + " bin/" + name + " >.build/temp2.txt");
            cout << "214\n";
            ifstream ages(".build/temp2.txt");
            tassert(ages.is_open());
            ages >> objTime >> outTime;
            ages.close();
        }
        catch (const runtime_error &e)
        {
            cout << "(Cannot stat a nonexistant file, moving on)\n";
            objTime = 10000;
            outTime = 0;
        }

        cout << "Linked.\n";

        // Clean up
        smartSystem("rm .build/temp2.txt");

        cout << "Cleaned up.\n";

        // If update is needed, update
        if (outTime < objTime)
        {
            hasUpdated = true;
            smartSystem(command + mainO + " -o bin/" + name);
        }
        else
        {
            cout << tags::green_bold
                 << name
                 << " is up to date\n"
                 << tags::reset;
        }

        cout << "Archive finished.\n";
    }

    // Determine if creating a library is necessary
    if (system("stat bin/lib.a") != 0)
    {
        hasUpdated = true;
    }

    // If needed, update library
    if (hasUpdated && libCommand.size() > 13)
    {
        cout << tags::green_bold
             << "Creating library...\n"
             << tags::reset;

        smartSystem(libCommand);

        cout << tags::green_bold
             << "Library created.\n"
             << tags::reset;
    }
    else
    {
        cout << tags::green_bold
             << "Project library is up to date or is not needed.\n"
             << tags::reset;
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

        // Parse command line arguments
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

        // Create architecture
        smartSystem("mkdir -p bin .build");

        // Do all the real work
        link(makeObjs());

        cout << tags::green_bold << '\n'
             << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
             << "Completed. (Output .out files can be found in ./bin)\n"
             << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
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
