#pragma once
#include <string>

class AuthGate {
public:
    static void authenticate();
    static std::string getSession();

private:
    static std::string m_session;
};