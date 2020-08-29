#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <ctime>

#include <fstream>
#include <adios2.h>
#include <direct.h>

struct Runner
{
    const std::vector<double> &myDouble;
    int numIt;
    bool createNewFile;
    Runner(const std::vector<double> &myDouble, const int numIt, const bool createNewFile): 
           myDouble(myDouble), numIt(numIt), createNewFile(createNewFile) {}
    void run(std::string s)
    {
        this->method = s;
        createFolder();
        int t1 = clock();
        runMethod();
        times[s] = clock() - t1;
        deleteFolder();
        //std::cin.ignore();
    }
    void printTimes()
    {
        for (auto tmp : times)
            std::cout << tmp.first << " " << tmp.second << " milliseconds\n";
    }
    void ADIOS2()
    {
        const std::size_t Nx = myDouble.size();

        adios2::ADIOS adios;
        adios2::IO bpIO = adios.DeclareIO("BPFile_N2N");
        bpIO.SetEngine("BP4");
        //bpIO.SetParameter("Threads", "6");
        bpIO.SetParameter("FlushStepsCount ", "5");
        bpIO.SetParameter("StatsLevel", "0");
        bpIO.SetParameter("Profile", "OFF");

        adios2::Variable<double> bpDouble = bpIO.DefineVariable<double>("bpFDouble", {}, {}, { Nx }, adios2::ConstantDims);
        if (createNewFile == false)
        {
            adios2::Engine bpWriter = bpIO.Open("adios", adios2::Mode::Write);
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
                adios2::Engine bpWriter = bpIO.Open("adios//" + std::to_string(i), adios2::Mode::Write);
                bpWriter.BeginStep();
                //bpWriter.Put<double>(bpDouble, myDouble.data());
                bpWriter.Put(bpDouble, myDouble.data());
                //bpWriter.PerformPuts();
                bpWriter.EndStep();
                bpWriter.Close();
            }
        }
    }
    void ADIOS2_HI_API()
    {
        const adios2::Dims shape{};
        const adios2::Dims start{};
        const adios2::Dims count{ myDouble.size() };
        if (createNewFile == false)
        {
            adios2::fstream oStream("adios", adios2::fstream::out);
            for (int i = 0; i < numIt; i++)
            {
                oStream.write("Data", myDouble.data(), shape, start, count, adios2::end_step);
            }
        }
        else
        {
            for (int i = 0; i < numIt; i++)
            {
                adios2::fstream oStream("adios", adios2::fstream::out);
                oStream.write("Data", myDouble.data(), shape, start, count, adios2::end_step);
                oStream.close();
            }
        }
    }
    void OFSTREAM()
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
    void FWRITE()
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
private:
    void runMethod()
    {
        if (method == "ADIOS2") ADIOS2();
        else if (method == "ADIOS2_HI_API") ADIOS2_HI_API();
        else if (method == "OFSTREAM") OFSTREAM();
        else if (method == "FWRITE") FWRITE();
        else throw "error, unknown method";
    }

    std::string method;
    std::string path = "adios";
    std::map<std::string, int> times;
    void createFolder() { mkdir(path.c_str()); }
    void deleteFolder()
    {
        std::string tmp;
        #ifdef __WINDOWS__
            tmp = "rd " + path + " /s /q";
        #else
            tmp = "rm " + path + " /s /q";
        #endif
        system(tmp.c_str());
    }
};