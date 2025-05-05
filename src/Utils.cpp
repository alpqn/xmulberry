#include "Utils.hpp"

#include <sstream>
#include <iostream>

QString SH(const QString& c)
{
    std::ostringstream output;
    FILE* stream{ popen(c.toStdString().c_str(), "r") };
    if (!stream) { std::cerr << "Couldn't run command: "<< c.toStdString(); exit(1); }
    char ch;
    while ((ch = fgetc(stream)) != EOF) { output.put(ch); }
    pclose(stream);
    return QString::fromStdString(output.str());
}
