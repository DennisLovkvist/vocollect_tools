#include<iostream>
#include<string>
#include<regex>  
#include <sstream>
#include <fstream>

int main(int argc, char *argv[])
{    
      
        std::ifstream infile("test.txt");
        std::string line;

        std::stringstream str(std::stringstream::out|std::stringstream::binary);

        std::regex keyword_pick_moment = std::regex("prTaskLUTPicked");
        std::regex keyword_pick_lane = std::regex("##Gång");
        std::regex keyword_pick_position = std::regex("\\d{3}\\sa|\\d{3}\\sb");
        std::regex keyword_pick_lane_value = std::regex("##\\(.*\\);pp");

        std::regex keyword_survey = std::regex("SURVEY:");        
        std::regex keyword_mac_address = std::regex("[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}:[0-9A-F]{2}");
        std::regex keyword_signal_strength = std::regex("\\d{3}%|\\d{2}%|\\d{1}%");
        
        std::string last_mac_address = ";00:00:00:00:00:00";
        std::string last_signal_strength = "0%";
        std::string last_timestamp = "9999999";
        std::string last_pick_lane = "";
        std::string last_pick_position = "";

        bool initial = true;

        while (std::getline(infile, line))
        {                
                std::smatch m;

                if(regex_search(line,m, keyword_pick_moment))
                {
                        size_t s = line.find(" - ");
                        size_t e = line.find(": ", s+3);

                        if(!initial)
                        {
                                str << ";" << last_mac_address << ";" << last_signal_strength << ";" << last_timestamp << std::endl;
                        }
                        initial = false;

                        str << line.substr(s+3, e - (s+3)) << ";" << last_pick_lane << ";" << last_pick_position << ";" << last_mac_address << ";" << last_signal_strength << ";" << last_timestamp;
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
                        while( ss.good() )
                        {
                                getline(ss, phonetic, ' ' );
                        }
                        last_pick_lane = phonetic.substr(0,1);
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

                        
                }   
        }           
        str << ";00:00:00:00:00:00;0%;9999999" << std::endl;
        std::ofstream file;
        file.open("pick_moments.txt", std::ofstream::binary);
        file.write(str.str().c_str(), str.str().length());
        file.close();  

        return 0;
}
