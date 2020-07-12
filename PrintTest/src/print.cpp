#include "print.h"

int main()
{
    const std::size_t Nx = 1000 * 1000 * 10;
    std::vector<double> myDouble(Nx);
    std::iota(myDouble.begin(), myDouble.end(), 0.0);
    const int numIt = 5 * 10;

    Runner r;
    r.timing("ADIOS2", myDouble, numIt, false);
    //r.timing("ADIOS2_HI_API", myDouble, numIt, false);
    //r.timing("OFSTREAM", myDouble, numIt, false);
    //r.timing("FWRITE", myDouble, numIt, false);
    r.print();
    return 0;
}
