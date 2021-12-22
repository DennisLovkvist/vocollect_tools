#include<iostream>
#include<string>
#include<regex>  
#include <sstream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <algorithm>

struct PickEvent 
{
    int timestamp;
    std::string aisle;
    std::string mac_address_ap[2];
    std::string signal_strength_ap[2];
    int timestamp_ap[2];  
};
void LoadPickEvents(std::vector<PickEvent> &pick_events, std::string filename)
{
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line))
    {
        std::stringstream ss(line);
        std::vector<std::string> data;
        while( ss.good() )
        {
            std::string substr;
            getline(ss, substr, ';' );
            data.push_back(substr);
        }
        if(data.size() == 9)
        {
            PickEvent pick_event;
            pick_event.timestamp = std::stoi(data[0]);
            pick_event.aisle = data[1];

            pick_event.mac_address_ap[0] = data[3];
            pick_event.signal_strength_ap[0] = data[4];
            pick_event.timestamp_ap[0] = std::stoi(data[5]);

            pick_event.mac_address_ap[1] = data[6];
            pick_event.signal_strength_ap[1] = data[7];
            pick_event.timestamp_ap[1] = std::stoi(data[8]);
            pick_events.push_back(pick_event);
        }
    }
}
float round(float value, unsigned char prec)
{
  float pow_10 = std::pow(10.0f, (float)prec);
  return std::round(value * pow_10) / pow_10;
}
int main(int argc, char *argv[])
{   
    if(argc == 1)
    {
        std::cout << "No file." << std::endl << "Press [ENTER] to exit.";
        std::cin.ignore();
        return 0;
    }
    std::regex keyword_filename = std::regex("^(.+?)(.txt)$");
    std::vector<PickEvent> *pick_events = new std::vector<PickEvent>();
    std::vector<std::string> *mac_addresses = new std::vector<std::string>();
    std::vector<int> *mac_addresses_count = new std::vector<int>();
    int total = 0;
    std::smatch m;

    std::vector<std::string> files;
    std::string file_path = "C:/temp";

    for (size_t i = 1; i < argc; i++)
    {
        file_path = (std::string)argv[i];  

        if(regex_search(file_path,m, keyword_filename))
        {
            files.push_back(file_path);
        } 
    }
    for (size_t i = 0; i < files.size(); i++)
    {   
        LoadPickEvents(*pick_events, files[i]);
        
        for (size_t j = 0; j < pick_events->size(); j++)
        {
            PickEvent *pick_event = &(*pick_events)[j];

            for (size_t k = 0; k < 2; k++)
            {
                std::string mac_address = pick_event->mac_address_ap[k];
            
                bool flag = false;

                for (size_t k = 0; k < mac_addresses->size(); k++)
                {
                    std::string address = (*mac_addresses)[k];
            
                    if(address == mac_address)
                    {
                        (*mac_addresses_count)[k]+=1;
                        total++;
                        flag = true;
                        break;
                    }
                }
                if(!flag)
                {
                    mac_addresses->push_back(mac_address);
                    mac_addresses_count->push_back(1);                
                    total++;
                }    
            }        
        }
    }

    std::stringstream string_stream(std::stringstream::out|std::stringstream::binary);

    char* display = NULL;
    int rows = mac_addresses->size();

    for (size_t i = 0; i < mac_addresses->size(); i++)
    {        
        string_stream << (*mac_addresses)[i] << " [";

        float p = ((float)(*mac_addresses_count)[i]/(float)total);
        int len = 50*p;
        len = (len <= 0) ? 1:len;
        for (size_t j = 1; j < len; j++)
        {
            string_stream << '#';
        }
        for (size_t j = len; j < 51; j++)
        {
            string_stream << ' ';
        }
        
        string_stream << "] " << round(p*100,1) << "%" << std::endl;        
    }

    std::cout << string_stream.str();       
}