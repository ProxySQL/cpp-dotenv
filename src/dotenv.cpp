#include "dotenv.h"

#include "environ.h"
#include "Parser.h"

#include <fstream>
#include <utility>


using namespace std;


// Type alias to work around the fact that class name matches namespace name
// This is a known C++ limitation when defining out-of-line members
typedef class ::dotenv::dotenv DotenvClass;


DotenvClass& ::dotenv::dotenv::load_dotenv(const string& dotenv_path, const bool overwrite, const bool interpolate)
{
    ifstream env_file;
    env_file.open(dotenv_path);

    if (env_file.good())
    {
        Parser parser;
        parser.parse(env_file, overwrite, interpolate);
        env_file.close();
    }

    return *this;
}


const string DotenvClass::operator[](const key_type& k) const
{
    return getenv(k).second;
}


DotenvClass& ::dotenv::dotenv::instance()
{
    return _instance;
}


// Define the global 'env' reference. Static members are inline in the header (C++17).
::dotenv::dotenv& ::dotenv::env = ::dotenv::dotenv::instance();
