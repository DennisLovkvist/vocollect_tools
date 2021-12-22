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

        std::regex keyword_pick_moment = std::regex("prTaskLUTPicked");
        std::regex keyword_pick_lane = std::regex("##Gång");
        std::regex keyword_pick_position = std::regex("\\d{3}\\sa|\\d{3}\\sb");
        std::regex keyword_pick_lane_value = std::regex("##\\(.*\\);pp");

        std::regex keyword_survey = std::regex("SURVEY:");        
        std::regex keyword_mac_address = std::regex("[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}");
        std::regex keyword_signal_strength = std::regex("\\d{3}%|\\d{2}%|\\d{1}%");
        
        std::string line;
        std::stringstream str(std::stringstream::out|std::stringstream::binary);

        std::regex keyword_filename = std::regex("^(.+?)(.txt)$");
        std::vector<std::string> files;
        std::string file_path = "C:/temp";

        for (size_t i = 1; i < argc; i++)
        {
                
                file_path = (std::string)argv[i];  
                std::smatch m;
                if(regex_search(file_path,m, keyword_filename))
                {
                        files.push_back(file_path);
                } 
        }
        for (size_t i = 0; i < files.size(); i++)
        {     
                std::ifstream infile(argv[i]);            

                std::string last_mac_address = "00:00:00:00:00:00";
                std::string last_signal_strength = "0%";
                std::string last_timestamp = "9999999";
                std::string last_pick_lane = "";
                std::string last_pick_position = "";
                std::string timestamp = "0";

                bool queue_pick_event = false;

                std::vector<std::string> queue;

                while (std::getline(infile, line))
                {                
                        std::smatch m;

                        if(regex_search(line,m, keyword_pick_moment))
                        {
                                size_t s = line.find(" - ");
                                size_t e = line.find(": ", s+3);                                
                                timestamp = line.substr(s+3, e - (s+3));   
                                queue_pick_event = true;
                                queue.push_back(line.substr(s+3, e - (s+3)) + ";" + last_pick_lane + ";" + last_pick_position + ";" + last_mac_address + ";" + last_signal_strength + ";" + last_timestamp);
                                
                        }
                        else if(regex_search(line,m, keyword_pick_position))
                        {
                                last_pick_position = m.str(0).substr(0,3);
                        }
                        else if(regex_search(line,m, keyword_pick_lane))
                        {
                                size_t s = line.find("##");
                                size_t e = line.find(";pp", s+2);
                                std::string temp = line.substr(s + 2, e - s -4);
                                std::string phonetic;
                                std::stringstream ss(line);
                                std::string area_code = "";
                                while( ss.good() )
                                {
                                        getline(ss, phonetic, ' ' );

                                        if(phonetic == "tre")
                                        {
                                                area_code = "3";
                                        }
                                        else if(phonetic == "två")
                                        {
                                                area_code = "2";
                                        }
                                        else if(phonetic == "fem")
                                        {
                                                area_code = "5";
                                        }
                                        else if(phonetic == "ett")
                                        {
                                                area_code = "1";
                                        }
                                }
                                last_pick_lane = area_code + phonetic.substr(0,1);
                        }
                        else if(regex_search(line,m, keyword_survey))
                        {
                                if(regex_search(line,m, keyword_mac_address))
                                {
                                        last_mac_address = m.str(0);
                                        size_t s = line.find(" - ");
                                        size_t e = line.find(": ", s+3);
                                        last_timestamp = line.substr(s+3, e - (s+3));                                
                                }
                                else if(regex_search(line,m, keyword_signal_strength))
                                {
                                        last_signal_strength = m.str(0);
                                }
                                if(queue_pick_event)
                                {
                                        for (size_t j = 0; j < queue.size(); j++)
                                        {
                                                queue[j] += ";" + last_mac_address + ";" + last_signal_strength + ";" + last_timestamp;
                                                str << queue[j] << std::endl;
                                        }
                                        queue.clear();
                                        queue_pick_event = false;                                                                                
                                }
                                
                        }   
                }           

                if(queue_pick_event)
                {
                        for (size_t i = 0; i < queue.size(); i++)
                        {
                                queue[i] += ";00:00:00:00:00:00;0%;0";
                                str << queue[i] << std::endl;
                        }
                        queue_pick_event = false;                                                                                
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
                file.open("pick_moments_from_" + filename_no_extension + "_" + std::to_string(t) + ".txt", std::ofstream::binary);
                file.write(str.str().c_str(), str.str().length());
                file.close();  

                str.str("");
                str.clear();

        }

        return 0;
}
