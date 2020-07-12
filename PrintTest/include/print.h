#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <ctime>

#include <fstream>
#include <stdio.h>
#include <adios2.h>

// 2 sec - 2.3 sec
void ADIOS2(std::vector<double> &myDouble, const int numIt, bool createNewFile = false)
{
    const std::size_t Nx = myDouble.size();

    adios2::ADIOS adios;
    adios2::IO bpIO = adios.DeclareIO("BPFile_N2N");
    bpIO.SetEngine("BP4");
    //bpIO.SetParameter("Threads", "6");
    bpIO.SetParameter("FlushStepsCount ", "5");
    bpIO.SetParameter("StatsLevel", "0");
    //bpIO.SetParameter("Profile", "OFF");

    adios2::Variable<double> bpDouble = bpIO.DefineVariable<double>("bpFDouble", {}, {}, { Nx }, adios2::ConstantDims);
    if (createNewFile == false)
    {
        adios2::Engine bpWriter = bpIO.Open("adios.bp", adios2::Mode::Write);
        for (int i = 0; i < numIt; i++)
        {
            bpWriter.BeginStep();
            bpWriter.Put(bpDouble, myDouble.data());
            bpWriter.EndStep();
        }
        bpWriter.Close();
    }
    else
    {
        for (int i = 0; i < numIt; i++)
        {
            adios2::Engine bpWriter = bpIO.Open("adios.bp//" + std::to_string(i), adios2::Mode::Write);
            bpWriter.BeginStep();
            //bpWriter.Put<double>(bpDouble, myDouble.data());
            bpWriter.Put(bpDouble, myDouble.data());
            //bpWriter.PerformPuts();
            bpWriter.EndStep();
            bpWriter.Close();
        }
    }
}

void ADIOS2_HI_API(std::vector<double> &myDouble, const int numIt, bool createNewFile = false)
{
    const adios2::Dims shape{};
    const adios2::Dims start{};
    const adios2::Dims count{ myDouble.size() };
    if (createNewFile == false)
    {
        adios2::fstream oStream("adios.bp", adios2::fstream::out);
        for (int i = 0; i < numIt; i++)
        {
            oStream.write("Data", myDouble.data(), shape, start, count, adios2::end_step);
        }
    }
    else
    {
        for (int i = 0; i < numIt; i++)
        {
            adios2::fstream oStream("adios.bp", adios2::fstream::out);
            oStream.write("Data", myDouble.data(), shape, start, count, adios2::end_step);
            oStream.close();
        }
    }
}

// 2.3 sec
void OFSTREAM(std::vector<double> &myDouble, const int numIt, bool createNewFile = false)
{
    if (createNewFile == false)
    {
        std::ofstream outfile("adios//res.bin", std::ofstream::binary);
        for (int i = 0; i < numIt; i++)
        {
            outfile.write((const char*)myDouble.data(), myDouble.size() * sizeof(double));
            //outfile.flush();
            //outfile.seekp(0);
        }
        outfile.close();
    }
    else
    {
        for (int i = 0; i < numIt; i++)
        {
            std::ofstream outfile("adios//" + std::to_string(i) + ".bin", std::ofstream::binary);
            outfile.write((const char*)myDouble.data(), myDouble.size() * sizeof(double));
            outfile.close();
        }
    }
}

// 1.8-2.1 sec
void FWRITE(std::vector<double> &myDouble, const int numIt, bool createNewFile = false)
{
    if (createNewFile == false)
    {
        FILE * outfile = fopen("adios//res.bin", "wb");
        //char buffer[sizeof(double)* 1000];
        //setbuf(outfile, buffer);
        for (int i = 0; i < numIt; i++)
        {
            fwrite(myDouble.data(), sizeof(double), myDouble.size(), outfile);
            fflush(outfile);
        }
        fclose(outfile);
    }
    else
    {
        for (int i = 0; i < numIt; i++)
        {
            FILE * outfile = fopen(("adios//" + std::to_string(i) + ".bin").c_str(), "wb");
            fwrite(myDouble.data(), sizeof(double), myDouble.size(), outfile);
            fclose(outfile);
        }
    }
}

struct Runner
{
    int t = 0;
    std::string s;
    void timing(std::string s, std::vector<double> &myDouble, const int numIt, bool createNewFile = false)
    {
        this->s = s;
        int t1 = clock();
        run(myDouble, numIt, createNewFile);
        t += clock() - t1;
    }
    void print()
    {
        std::cout << s << " " << t << " milliseconds\n";
    }
private:
    void run(std::vector<double> &myDouble, const int numIt, bool createNewFile = false)
    {
        if (s == "ADIOS2")
        {
            ADIOS2(myDouble, numIt, createNewFile);
        }
        else if (s == "ADIOS2_HI_API")
        {
            ADIOS2_HI_API(myDouble, numIt, createNewFile);
        }
        else if (s == "OFSTREAM")
        {
            OFSTREAM(myDouble, numIt, createNewFile);
        }
        else if (s == "FWRITE")
        {
            FWRITE(myDouble, numIt, createNewFile);
        }
        else throw "error";
    }
};