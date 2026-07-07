#pragma once


#include <string>


namespace dotenv
{
    class dotenv
    {
    public:

        using   key_type = std::string;
        using value_type = std::string;

    public:

        dotenv& load_dotenv(const std::string& dotenv_path = ".env",
                            const bool overwrite = false,
                            const bool interpolate = true);

        const value_type operator[](const key_type& k) const;

    public:

        virtual ~dotenv() = default;
        dotenv(const dotenv&) = delete;
        void operator=(const dotenv&) = delete;

        static dotenv& instance();

        bool loaded = false;

    private:

        dotenv() = default;

    private:

        // Declare static members (definitions are inline after the class)
        // NOTE: env_filename is kept for backward compatibility; external code
        // may reference dotenv::dotenv::env_filename directly.
        static const std::string env_filename;
        static dotenv _instance;

    };


    // C++17 inline variable definitions after class is complete
    // Note: _instance cannot be inline inside the class due to incomplete type
    inline const std::string dotenv::env_filename = ".env";
    inline dotenv dotenv::_instance;


    extern dotenv& env;
}
