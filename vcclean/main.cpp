#include<iostream>
#include<string>
#include<regex>  
#include <sstream>
#include <fstream>
#include <ctime>

int main(int argc, char *argv[])
{    
    if(argc == 1)
    {
            std::cout << "No file." << std::endl << "Press [ENTER] to exit.";
            std::cin.ignore();
            return 0;
    }

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

        std::string input_file = argv[i];

        int start = 0;
        int end = input_file.find("/");
        while (end != std::string::npos)
        {
            start = end + 1;
            end = input_file.find("/", start);
        }

        std::string filename = input_file.substr(start, end - start);
        std::string filename_no_extension = filename.substr(0, filename.find("."));

        std::time_t t = std::time(0); 

        std::ofstream file;
        file.open(filename_no_extension + "_parsed_" + std::to_string(t) + ".txt", std::ofstream::binary);
        file.write(str.str().c_str(), str.str().length());
        file.close();  
        
    }

    return 0;
}
