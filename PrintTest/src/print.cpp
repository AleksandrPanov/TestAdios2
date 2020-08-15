#include "print.h"
#include <experimental/filesystem>

int main()
{
    const std::size_t Nx = 1000 * 1000 * 10;
    std::vector<double> myDouble(Nx);
    std::iota(myDouble.begin(), myDouble.end(), 0.0);
    const int numIt = 5 * 10;
    bool createNewFile = true;

    Runner r(myDouble, numIt, createNewFile);
    //r.run("ADIOS2");
    
    //r.run("ADIOS2_HI_API");
    r.run("OFSTREAM");
    r.run("FWRITE");
    r.print();
    return 0;
}
