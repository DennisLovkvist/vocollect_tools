#include<iostream>
#include<string>
#include<regex>  
#include <sstream>
#include <fstream>

int main(int argc, char *argv[])
{    
    for (size_t i = 1; i < argc; i++)
    {   
        std::ifstream infile(argv[i]);
        std::string line;

        std::regex keywords_cleanup[4] = {std::regex(": \\^\\^"),std::regex(": ##"),std::regex(": SURVEY"),std::regex("prTaskLUTPicked")};
        std::regex filter_out_commands[2] = {std::regex("\\<sil\\>"),std::regex("\\<garbage00\\>")};
        std::string delimiters[4] = {"QUEUING","IGNORING","REJECTED","@@"};

        std::stringstream str(std::stringstream::out|std::stringstream::binary);

        while (std::getline(infile, line))
        {
            std::smatch m;
            for (size_t i = 0; i < 4; i++)
            {
                if(regex_search(line,m, keywords_cleanup[i]))
                {
                    bool flag_filter = true;
                    for (size_t j = 0; j < 2; j++)
                    {
                        if(regex_search(line,m, filter_out_commands[j]))
                        {
                            flag_filter = false;
                            break;
                        }
                    }    
                    if(flag_filter)
                    {
                        if(regex_search(line,m, keywords_cleanup[i]))
                        {
                            bool flag = false;
                            for (size_t j = 0; j < 4; j++)
                            {
                                std::string delimiter = delimiters[j];                        
                                size_t found = line.find(delimiter);
                                if(found != 4294967295)
                                {   
                                    str << line.substr(0,found-delimiter.length()) <<std::endl;
                                    flag = true;
                                    break;
                                }
                            }
                            if(!flag)
                            {
                                str << line <<std::endl;
                            }
                            
                            break;
                        }
                    }
                }
            }
        }
        std::ofstream datFile;
        datFile.open("test.txt", std::ofstream::binary);
        datFile.write(str.str().c_str(), str.str().length());
        datFile.close();     
    }

    return 0;
}
