#include "dotenv.h"

#include "environ.h"
#include "Parser.h"

#include <fstream>
#include <utility>


using namespace std;


// Typedef to work around the fact that class name matches namespace name
// This is a known C++ limitation when defining out-of-line members
typedef class ::dotenv::dotenv Dotenv_Type;


Dotenv_Type& ::dotenv::dotenv::load_dotenv(const string& dotenv_path, const bool overwrite, const bool interpolate)
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


auto ::dotenv::dotenv::operator[](const key_type& k) const -> const string
{
    return getenv(k).second;
}


Dotenv_Type& ::dotenv::dotenv::instance()
{
    return _instance;
}


// Static members are now inline in the header (C++17)
::dotenv::dotenv& ::dotenv::env = ::dotenv::dotenv::instance();
