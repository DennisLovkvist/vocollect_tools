#include <SFML/Graphics.hpp>
#include <windows.h>
#include<string>
#include<iostream>
#include <cmath>
#include <sstream>
#include <fstream>

static const int PP_WIDTH = 8;
static const int PP_HEIGHT = 8;
static const int AISLE_WIDTH_INNER = 32;
static const int AISLE_WIDTH_OUTER = PP_HEIGHT + PP_HEIGHT + AISLE_WIDTH_INNER;
static const float PI = 3.141;
static const int SPACING = (int)(PP_WIDTH*0.2);
static const sf::Color COLOR_AP_SOLID = sf::Color(180,180,180);
static const sf::Color COLOR_AP_TEXT = sf::Color(0,0,0);
static const sf::Color COLOR_AISLE_SOLID = sf::Color(150,150,150);
static const sf::Color COLOR_AISLE_TEXT = sf::Color(0,0,0);
static const sf::Color COLOR_CLEAR = sf::Color(50,50,50);

struct Overlay
{
    sf::RectangleShape pick_position_overlay_background;    
    sf::Text pick_position_overlay_label;

    sf::RectangleShape access_point_overlay_background;    
    sf::Text access_point_overlay_label;
};
struct AccessPoint
{
    std::string id;
    std::string address;
    int x;int y;int r;
    sf::Text label; 
    bool hover;
    bool toggle_overlay;   
};
struct Aisle 
{
    int pick_positions;
    int x;int y;
    sf::Text label;
    std::string id;
};
struct PickEvent 
{
    int timestamp;
    std::string aisle;
    int pick_position;
    bool valid;             
    Aisle* m_aisle;
    bool aisle_valid;    

    bool valid_ap[2];
    AccessPoint* ap[2];

    std::string mac_address_ap[2];
    std::string signal_strength_ap[2];
    int timestamp_ap[2];  
};
struct InputState
{
    bool mouse_right_down;
    bool mouse_left_down;
    sf::Vector2i mouse_position_delta;
    sf::Vector2i mouse_position_old;
    sf::Vector2i mouse_position;

};
void MapPickEvents(std::vector<PickEvent> &pick_events,std::vector<Aisle> &aisles,std::vector<AccessPoint> &access_points)
{

    for (size_t i = 0; i < pick_events.size(); i++)
    {
        PickEvent* pick_event = &pick_events[i];
        sf::Vector2f position;
        for (int j = 0; j < aisles.size(); j++)
        {
            Aisle* aisle = &aisles[j];

            if(pick_event->aisle == aisle->id)
            {
                pick_event->m_aisle = aisle;
                pick_event->aisle_valid = true;
                break;
            }
        }   
        int flag = 0;
        
        for (int j = 0; j < access_points.size() && flag < 2; j++)
        {
            AccessPoint* access_point = &access_points[j];
            for (size_t k = 0; k < 2; k++)
            {
                if(pick_event->mac_address_ap[k] == access_point->address)
                {      
                    pick_event->valid_ap[k] = true;
                    pick_event->ap[k] = access_point;
                    flag += 1;
                }
            }     
        }
    }
}

bool Interact(InputState &input_state, sf::Vector2f mouse_position_coord,std::vector<Aisle> &aisles,std::vector<AccessPoint> &access_points, int &move_index)
{    
    bool refresh = false;
    for (size_t i = 0; i < access_points.size(); i++)
    {
        AccessPoint* access_point = &access_points[i];

        int dist = std::sqrt(std::pow(access_point->x - mouse_position_coord.x, 2) + std::pow(access_point->y - mouse_position_coord.y, 2) * 1.0);
        bool t0 = access_point->hover;
        if(dist < 16)
        {
            access_point->hover = true;   
            if(input_state.mouse_left_down)    
            {
                move_index = i;
            }
        }
        else
        {
            access_point->hover = false;
        }
        bool t1 = access_point->hover;
        if(t0!=t1)
        {
            refresh = true;
        }
    }

    if(!input_state.mouse_left_down)    
    {
        move_index = -1;
    }

    return refresh;
    
}
void UpdateInputState(sf::Event &event,sf::Window &window, InputState &input_state)
{
    input_state.mouse_position = sf::Mouse::getPosition(window);

    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Right)
        {                    
            input_state.mouse_right_down = true;
        }
        else if (event.mouseButton.button == sf::Mouse::Left)
        {                    
            input_state.mouse_left_down = true;
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Right)
        {                    
            input_state.mouse_right_down = false;
        }
        else if (event.mouseButton.button == sf::Mouse::Left)
        {                    
            input_state.mouse_left_down = false;
        }
    } 

    input_state.mouse_position_delta = input_state.mouse_position_old-input_state.mouse_position;
    
    input_state.mouse_position_old = input_state.mouse_position;

}
bool UpdateControls(InputState &input_state, sf::View &view)
{
    if(input_state.mouse_right_down)
    {          
        view.move(input_state.mouse_position_delta.x, input_state.mouse_position_delta.y); 
        return true;    
    }  

    return false;
}
void DefineAisle(std::vector<Aisle> &aisles, Aisle &aisle,std::string id,int x, int y, int pick_positions)
{
    aisle.id = id;
    aisle.x = x;
    aisle.y = y;    
    aisle.pick_positions = pick_positions;
    aisles.push_back(aisle);
}
void DefineAccessPoint(std::vector<AccessPoint> &access_points, AccessPoint &access_point,std::string address,std::string id,int x, int y)
{
    access_point.toggle_overlay = access_point.hover = false;
    access_point.address = address;
    access_point.id = id;
    access_point.x = x;
    access_point.y = y;
    access_point.r = 12;
    access_points.push_back(access_point);
}
void LoadDefaults(std::vector<Aisle> &aisles, std::vector<AccessPoint> &access_points)
{
    Aisle aisle;
    AccessPoint access_point;
    DefineAisle(aisles,aisle,"3A",0,225,102);
    DefineAisle(aisles,aisle,"3B",48,225,102);
    DefineAisle(aisles,aisle,"3C",96,175,114);
    DefineAisle(aisles,aisle,"3D",144,175,114);
    DefineAisle(aisles,aisle,"3E",192,0,156);
    DefineAisle(aisles,aisle,"3F",240,0,156);
    DefineAisle(aisles,aisle,"3G",288,0,156);
    DefineAisle(aisles,aisle,"3H",336,0,156);
    DefineAisle(aisles,aisle,"3I",384,0,156);
    DefineAisle(aisles,aisle,"3K",432,0,156);
    DefineAisle(aisles,aisle,"3L",480,0,156);
    DefineAisle(aisles,aisle,"3M",528,0,156);
    DefineAisle(aisles,aisle,"3N",576,0,156);
    DefineAisle(aisles,aisle,"3O",624,0,156);
    DefineAisle(aisles,aisle,"3P",672,0,156);
    DefineAisle(aisles,aisle,"3R",720,0,156);
    DefineAisle(aisles,aisle,"3S",768,225,102);
    DefineAisle(aisles,aisle,"3T",816,225,102);
    DefineAisle(aisles,aisle,"3U",864,0,156);
    DefineAisle(aisles,aisle,"3V",912,0,156);
    DefineAisle(aisles,aisle,"3X",960,0,156);
    DefineAisle(aisles,aisle,"3Y",1008,0,156);
    DefineAisle(aisles,aisle,"3Z",1056,0,156);

    DefineAisle(aisles,aisle,"5D",192,-800,156);
    DefineAisle(aisles,aisle,"5E",288,-800,156);
    DefineAisle(aisles,aisle,"5F",384,-800,156);
    DefineAisle(aisles,aisle,"5G",480,-800,156);
    DefineAisle(aisles,aisle,"5H",576,-800,156);

    DefineAisle(aisles,aisle,"H",-48,225,102);

    DefineAisle(aisles,aisle,"4A",-112,380,60);
    DefineAisle(aisles,aisle,"4A",-112,380,60);
    DefineAisle(aisles,aisle,"4A",-160,380,60);
    DefineAisle(aisles,aisle,"4A",-208,380,60);
    DefineAisle(aisles,aisle,"4A",-256,380,60);
    DefineAisle(aisles,aisle,"4A",-304,380,60);
    DefineAisle(aisles,aisle,"4A",-352,380,60);
    DefineAisle(aisles,aisle,"4A",-400,380,60);
    DefineAisle(aisles,aisle,"4A",-448,380,60);

    DefineAisle(aisles,aisle,"1V",-512,225,102);
    DefineAisle(aisles,aisle,"1U",-560,225,102);
    DefineAisle(aisles,aisle,"1T",-608,225,102);
    DefineAisle(aisles,aisle,"1S",-656,225,102);
    DefineAisle(aisles,aisle,"1R",-704,225,102);
    DefineAisle(aisles,aisle,"1P",-752,225,102);
    DefineAisle(aisles,aisle,"1O",-800,225,102);
    DefineAisle(aisles,aisle,"1N",-848,225,102);
    DefineAisle(aisles,aisle,"1M",-896,225,102);
    DefineAisle(aisles,aisle,"1L",-944,225,102);
    DefineAisle(aisles,aisle,"1K",-992,225,102);
    DefineAisle(aisles,aisle,"1J",-1040,225,102);
    DefineAisle(aisles,aisle,"1H",-1088,225,102);
    DefineAisle(aisles,aisle,"1G",-1136,225,102);
    DefineAisle(aisles,aisle,"1F",-1184,225,102);
    DefineAisle(aisles,aisle,"1E",-1232,225,102);
    DefineAisle(aisles,aisle,"1D",-1280,225,102);
    DefineAisle(aisles,aisle,"1C",-1328,225,102);
    DefineAisle(aisles,aisle,"1B",-1376,225,102);
    DefineAisle(aisles,aisle,"1A",-1424,225,102);

    DefineAisle(aisles,aisle,"2T",-560,900,48);
    DefineAisle(aisles,aisle,"2S",-608,900,48);
    DefineAisle(aisles,aisle,"2R",-656,900,102);
    DefineAisle(aisles,aisle,"2P",-704,900,102);
    DefineAisle(aisles,aisle,"2O",-752,900,102);
    DefineAisle(aisles,aisle,"2N",-800,900,102);
    DefineAisle(aisles,aisle,"2M",-848,900,102);
    DefineAisle(aisles,aisle,"2L",-896,900,102);
    DefineAisle(aisles,aisle,"2F",-944,900,102);
    DefineAisle(aisles,aisle,"2E",-992,900,102);
    DefineAisle(aisles,aisle,"2D",-1040,900,102);
    DefineAisle(aisles,aisle,"2C",-1088,900,102);
    DefineAisle(aisles,aisle,"2B",-1136,900,102);
    DefineAisle(aisles,aisle,"2A",-1184,900,102);

    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:27:F2","39",12,0);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:14:6B:52","26",114,49);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D2:C4:D2","40",212,-90);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:29:D2","41",307,-90);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:22:32","42",403,-88);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:3C:72","43",497,-90);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:43:D2","44",595,-90);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:36:F2","45",690,-90);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:46:92","46",790,145);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D2:EE:D2","47",934,-90);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D2:F0:D2","48",1029,-90);
    DefineAccessPoint(access_points, access_point,"70:3A:0E:8E:9A:52","27",68,680);
    DefineAccessPoint(access_points, access_point,"70:3A:0E:8E:98:52","28",164,680);
    DefineAccessPoint(access_points, access_point,"70:3A:0E:8E:94:72","29",261,681);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:2B:D2","30",357,680);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:29:92","31",453,679);
    DefineAccessPoint(access_points, access_point,"70:3A:0E:8E:99:F2","32",548,679);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:46:12","33",646,680);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:47:32","34",740,680);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:37:B2","35",837,679);
    DefineAccessPoint(access_points, access_point,"70:3A:0E:8E:9C:12","37",885,680);
    DefineAccessPoint(access_points, access_point,"70:3A:0E:8E:9A:92","36",981,680);
    DefineAccessPoint(access_points, access_point,"70:3A:0E:8E:9A:32","38",1078,680);
    DefineAccessPoint(access_points, access_point,"34:FC:B9:F5:2C:D2","53",217,757);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:26:12","51",474,783);
    DefineAccessPoint(access_points, access_point,"34:FC:B9:F5:2C:72","54",423,808);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:3B:B2","49",862,731);
    DefineAccessPoint(access_points, access_point,"84:D4:7E:D3:2E:B2","50",836,834);
    DefineAccessPoint(access_points, access_point,"34:FC:B9:F5:2D:72","52",981,783);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7D:2E:93","200",212,-213);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7C:B8:13","201",212,-563);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7D:3D:93","202",308,-739);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7C:CF:13","203",308,-488);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7D:2D:33","204",497,-118);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7D:3E:53","205",404,-214);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7D:3C:D3","206",404,-588);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7D:2D:F3","207",404,-888);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7D:2E:73","208",500,-738);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7D:39:53","209",500,-488);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7D:31:93","210",596,-663);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7D:35:73","211",812,-388);
    DefineAccessPoint(access_points, access_point,"6C:F3:7F:7D:2D:53","212",812,-218);
    DefineAccessPoint(access_points, access_point,"94:B4:0F:81:E9:53","213",812,-548);
    DefineAccessPoint(access_points, access_point,"94:B4:0F:91:19:53","214",307,-119);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:14:A4:D2","55",-1328,707);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:14:A4:52","56",-579,1271);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:14:A6:52","57",-539,1188);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:10:1B:D2","58",-1164,1337);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:10:1A:F2","59",-1066,1337);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:10:1C:92","60",-973,1337);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:10:1A:12","61",-876,1337);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:10:1F:92","62",-780,1337);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:10:1C:B2","63",-1068,763);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:10:1C:F2","64",-973,763);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:10:1C:72","65",-880,761);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:10:1B:52","66",-780,837);
    DefineAccessPoint(access_points, access_point,"B4:5D:50:10:1B:B2","67",-684,836);
    DefineAccessPoint(access_points, access_point,"34:FC:B9:F5:2C:92","68",-1161,726);
    DefineAccessPoint(access_points, access_point,"34:FC:B9:F5:2D:52","69",-527,1312);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","1",-192,67);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","2",-509,70);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","3",-825,72);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","4",-1114,65);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","5",-1405,670);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","6",-1309,673);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","7",-1212,671);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","8",-1117,673);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","9",-1020,670);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","10",-925,669);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","11",-829,668);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","12",-732,672);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","13",-637,672);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","14",-540,674);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","15",-1356,147);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","16",-1258,148);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","17",-1165,147);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","18",-1068,148);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","19",-973,148);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","20",-877,149);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","21",-781,148);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","22",-687,150);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","23",-589,150);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","24",-493,148);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","25",-1291,34);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","70",-333,275);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","71",-140,277);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","72",-378,748);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","73",-181,752);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","74",-373,179);
    DefineAccessPoint(access_points, access_point,"00:00:00:00:00:00","75",-191,177);

}
void LoadAisles(std::vector<Aisle> &aisles, std::string filename)
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
        if(data.size() == 4)
        {
            Aisle aisle;
            aisle.id = data[0];
            aisle.x = std::stoi(data[1]);
            aisle.y = std::stoi(data[2]);    
            aisle.pick_positions = std::stoi(data[3]);
            aisles.push_back(aisle);
        }
    }
}
void LoadAccessPoints(std::vector<AccessPoint> &access_points, std::string filename)
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
        if(data.size() == 4)
        {
            AccessPoint access_point;
            access_point.toggle_overlay = access_point.hover = false;
            access_point.address = data[1];
            access_point.id = data[0];
            access_point.x = std::stoi(data[2]);
            access_point.y = std::stoi(data[3]);
            access_point.r = 12;
            access_points.push_back(access_point);
        }
    }
}
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
            pick_event.valid = false;
            pick_event.timestamp = std::stoi(data[0]);
            pick_event.aisle = data[1];
            pick_event.pick_position = std::stoi(data[2]);

            pick_event.mac_address_ap[0] = data[3];
            pick_event.signal_strength_ap[0] = data[4];
            pick_event.timestamp_ap[0] = std::stoi(data[5]);

            pick_event.mac_address_ap[1] = data[6];
            pick_event.signal_strength_ap[1] = data[7];
            pick_event.timestamp_ap[1] = std::stoi(data[8]);
                         
            pick_event.valid_ap[0] = pick_event.valid_ap[1] = pick_event.aisle_valid = false;

            pick_events.push_back(pick_event);
        }
    }
}
void ShowPickEvent(PickEvent *pe, bool flag)
{
    for (size_t i = 0; i < 2; i++)
    {
        if(pe->valid_ap[i])
        {
            pe->ap[i]->toggle_overlay = flag;
        }
    }
}
int main(int argc, char *argv[])
{

    #pragma region
    //Handle standard input.
    std::string path_aisle_data = "";
    std::string path_access_point_data = "";
    std::string path_pick_event_data = "";

    char result[ MAX_PATH ];
    std::string executable_file_path = std::string( result, GetModuleFileName( NULL, result, MAX_PATH ) );   
    std::string executable_path = executable_file_path.substr(0, executable_file_path.find_last_of("\\"));

    bool load_defaults = false;
    if(argc == 4)
    {        
        path_pick_event_data = argv[1];
        path_aisle_data = argv[2];
        path_access_point_data = argv[3];
    }
    else if(argc == 2)
    {
        path_pick_event_data = argv[1];
        load_defaults = true;
    }
    else
    {
        std::cout << "Expected at least 1 argument." << std::endl;
        std::cout << "      Example: .\\optiscan_network_map.exe aisles.txt access_points.txt pick_events.txt" << std::endl << std::endl << "Press [ENTER] to exit." << std::endl;
        std::cin.ignore();
        return 0;     
    }    
    #pragma endregion
    
    #pragma region
    //Variables
    sf::Clock clock;
	float dt = 1.f / 60.f;
	float accumulator = 0.f;

    bool next_event = false;
    bool prev_event = false;
    int selected_event = 0;
    int move_index = -1;

    bool render_screen = true;
    sf::ContextSettings settings;   
    settings.antialiasingLevel = 16;
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Optiscan network map",sf::Style::Default, settings);
    sf::View view = window.getDefaultView();
    view.zoom(1);
    
    InputState input_state;  
    input_state.mouse_right_down = false;   
    input_state.mouse_left_down = false; 
    input_state.mouse_position_delta.x = 0;
    input_state.mouse_position_delta.y = 0;
    input_state.mouse_position_old.x = 0;    
    input_state.mouse_position_old.y = 0;
    input_state.mouse_position.x = 0;
    input_state.mouse_position.y = 0;

    sf::VertexArray pick_event_ap_connection(sf::Quads, 4);
    #pragma endregion

    #pragma region
    //Loads the required files
    sf::Font font;
    if (!font.loadFromFile(executable_path  + "/arial.ttf"))
    {
        window.close();
        std::cout << "Cannot find font at " << (executable_path  + "/arial.ttf") << std::endl << "Press [ENTER] to exit." << std::endl;
        std::cin.ignore(); 
        return 0;
    }

    std::vector<Aisle> *aisles = new std::vector<Aisle>();
    std::vector<AccessPoint> *access_points = new std::vector<AccessPoint>();
    std::vector<PickEvent> *pick_events = new std::vector<PickEvent>();

    if(!load_defaults)
    {
        LoadAisles(*aisles,path_aisle_data);   
        LoadAccessPoints(*access_points,path_access_point_data);     
        LoadPickEvents(*pick_events, path_pick_event_data); 
    }
    else
    {
        LoadDefaults(*aisles,*access_points);
        LoadPickEvents(*pick_events, path_pick_event_data); 
    }

         
    #pragma endregion 

    #pragma region
    /*Sets up everything to visually represent the Aisles.
    Every shape is put into a VertexArray to minimize drawing calls.
    */
    int n = 0;
    for (size_t i = 0; i < aisles->size(); i++)
    {
        Aisle aisle = (*aisles)[i];
        n+= (aisle.pick_positions*4) + 4;        
    }
    sf::VertexArray map = sf::VertexArray(sf::Quads,n);
    int index = 0; 
    for (size_t i = 0; i < aisles->size(); i++)
    {
        Aisle *aisle = &(*aisles)[i];

        map[index].position.x = aisle->x;
        map[index].position.y = aisle->y - AISLE_WIDTH_INNER - PP_HEIGHT - 6;
        map[index].color = COLOR_AISLE_SOLID;
        index++;
        map[index].position.x = aisle->x + AISLE_WIDTH_INNER + PP_WIDTH;
        map[index].position.y = aisle->y - AISLE_WIDTH_INNER-PP_HEIGHT - 6;
        map[index].color = COLOR_AISLE_SOLID;
        index++;
        map[index].position.x = aisle->x + AISLE_WIDTH_INNER + PP_WIDTH;
        map[index].position.y = aisle->y - AISLE_WIDTH_INNER-PP_HEIGHT + AISLE_WIDTH_INNER + PP_HEIGHT - 6;
        map[index].color = COLOR_AISLE_SOLID;
        index++;        
        map[index].position.x = aisle->x;
        map[index].position.y = aisle->y - AISLE_WIDTH_INNER-PP_HEIGHT + AISLE_WIDTH_INNER + PP_HEIGHT - 6;
        map[index].color = COLOR_AISLE_SOLID;
        index++;

        for (int j = aisle->pick_positions-1;j >= 0; j--)
        {   
            int even = (j%2==0);
            int x = aisle->x + AISLE_WIDTH_INNER * even;
            int y = aisle->y + (PP_WIDTH * (j/2)) + (SPACING * ((j/2)/3));
            
            map[index].position.x = x;
            map[index].position.y = y;
            map[index].color = COLOR_AISLE_SOLID;
            index++;
            map[index].position.x = x+PP_WIDTH;
            map[index].position.y = y;
            map[index].color = COLOR_AISLE_SOLID;
            index++;   
            map[index].position.x = x+PP_WIDTH;
            map[index].position.y = y+PP_HEIGHT;
            map[index].color = COLOR_AISLE_SOLID;
            index++;
            map[index].position.x = x;
            map[index].position.y = y+PP_HEIGHT;
            map[index].color = COLOR_AISLE_SOLID;
            index++;
        }        

        aisle->label.setFont(font);
        aisle->label.setString(aisle->id);
        aisle->label.setCharacterSize(16);
        aisle->label.setFillColor(COLOR_AISLE_TEXT);
        sf::FloatRect bounds = aisle->label.getGlobalBounds();

        int n = (AISLE_WIDTH_INNER+(PP_HEIGHT*2));
        int x = aisle->x + (n/2) - (bounds.width/2)-4;
        int y = aisle->y - n + (n/2) - (bounds.height);
        aisle->label.setPosition(x,y);
        
    }    
    #pragma endregion

    #pragma region
    /*Sets up everything to visually represent the Access Points.  
    Every circle is composed of 16 triangles. All triangles are stored in the same VertexArray to minimize draw calls.
    */ 
    int edges = 24;
    sf::VertexArray circles = sf::VertexArray(sf::Triangles,access_points->size()*(edges*3));
    index = 0;
    float R = (PI*2)/edges;
    for (size_t i = 0; i < access_points->size(); i++)
    {
        AccessPoint *ap = &(*access_points)[i];

        float r = 0;
        for (size_t j = 0; j < edges; j++)
        {
            circles[index].position.x = ap->x;
            circles[index].position.y = ap->y;
            circles[index].color = COLOR_AP_SOLID;
            index ++;
            circles[index].position.x = ap->x + std::cos(r) * ap->r;
            circles[index].position.y = ap->y + std::sin(r) * ap->r;            
            circles[index].color = COLOR_AP_SOLID;
            index ++;
            r+=R;
            circles[index].position.x = ap->x + std::cos(r) * ap->r;
            circles[index].position.y = ap->y + std::sin(r) * ap->r;            
            circles[index].color = COLOR_AP_SOLID;
            index ++;
        }
        ap->label.setFont(font);
        ap->label.setString(ap->id);
        ap->label.setCharacterSize(12);
        ap->label.setFillColor(COLOR_AP_TEXT);
        sf::FloatRect bounds = ap->label.getLocalBounds();
        ap->label.setPosition(ap->x - (bounds.width/2),ap->y - (bounds.height*2)/2);

    }
    #pragma endregion


    #pragma region
    Overlay overlay;
    overlay.pick_position_overlay_background.setSize(sf::Vector2f(48,24));
    overlay.pick_position_overlay_background.setFillColor(sf::Color(10,10,10,150));
    overlay.pick_position_overlay_label.setFont(font);
    overlay.pick_position_overlay_label.setString("test");
    overlay.pick_position_overlay_label.setCharacterSize(16);
    overlay.pick_position_overlay_label.setFillColor(sf::Color::White);
    overlay.access_point_overlay_label.setFont(font);
    overlay.access_point_overlay_label.setCharacterSize(16);
    overlay.access_point_overlay_label.setFillColor(sf::Color::White);
    overlay.access_point_overlay_background.setFillColor(sf::Color(10,10,10,100));
    #pragma endregion

    MapPickEvents(*pick_events,*aisles,*access_points);

    if(selected_event < pick_events->size() && selected_event >= 0)
    {
        ShowPickEvent(&(*pick_events)[selected_event], true);
    }    

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {                
                window.close();
            }

            UpdateInputState(event,window, input_state);

            if(UpdateControls(input_state,view))
            {
                render_screen = true;
            }

        }
        
        accumulator += clock.getElapsedTime().asSeconds();

		clock.restart();
        while (accumulator >= dt)
		{	
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !prev_event)
            {
                if(selected_event < pick_events->size() && selected_event >= 0)
                {
                    ShowPickEvent(&(*pick_events)[selected_event], false);
                    prev_event = true;
                    selected_event = (selected_event -1 < 0) ? selected_event=pick_events->size()-1:selected_event -1;
                    ShowPickEvent(&(*pick_events)[selected_event], true);
                    render_screen = true;
                }
            }
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                prev_event = false;               
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !next_event)
            {
                if(selected_event < pick_events->size() && selected_event >= 0)
                {
                    ShowPickEvent(&(*pick_events)[selected_event], false);
                    next_event = true;
                    selected_event = (selected_event +1 >= pick_events->size()) ? 0:selected_event +1; 
                    ShowPickEvent(&(*pick_events)[selected_event], true);
                    render_screen = true;
                }     
            }
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                next_event = false;               
            }  
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::T))
            { std::cout << std::endl<< std::endl<< std::endl<< std::endl<< std::endl;
                for (size_t i = 0; i < access_points->size(); i++)
                {
                    AccessPoint *ap = &(*access_points)[i];
                    std::cout << ap->id << ";" << ap->address << ";" << ap->x << ";" << ap->y << std::endl;
                }
            }

            sf::Vector2f mp_v = window.mapPixelToCoords(input_state.mouse_position,view);      
/*
            if(move_index >= 0 && move_index < access_points->size())
            {
                AccessPoint *ap = &(*access_points)[move_index];
                ap->x = mp_v.x;
                ap->y = mp_v.y;

                index = 0;
                float R = (PI*2)/16;
                for (size_t i = 0; i < access_points->size(); i++)
                {
                    AccessPoint *ap = &(*access_points)[i];

                    float r = 0;
                    for (size_t j = 0; j < 16; j++)
                    {
                        circles[index].position.x = ap->x;
                        circles[index].position.y = ap->y;
                        circles[index].color = COLOR_AP_SOLID;
                        index ++;
                        circles[index].position.x = ap->x + std::cos(r) * ap->r;
                        circles[index].position.y = ap->y + std::sin(r) * ap->r;            
                        circles[index].color = COLOR_AP_SOLID;
                        index ++;
                        r+=R;
                        circles[index].position.x = ap->x + std::cos(r) * ap->r;
                        circles[index].position.y = ap->y + std::sin(r) * ap->r;            
                        circles[index].color = COLOR_AP_SOLID;
                        index ++;
                    }
                    ap->label.setFont(font);
                    ap->label.setString(ap->id);
                    ap->label.setCharacterSize(12);
                    ap->label.setFillColor(COLOR_AP_TEXT);
                    sf::FloatRect bounds = ap->label.getLocalBounds();
                    ap->label.setPosition(ap->x - (bounds.width/2),ap->y - (bounds.height*2)/2);

                }
                render_screen = true;
            }      
            */
            
           

            if(Interact(input_state,mp_v,*aisles, *access_points, move_index))
            {
                render_screen = true;
            }
            accumulator -= dt;
        }
    

        if (!render_screen)
        {
            sf::sleep(sf::seconds(0.01));
        }

        #pragma region

        if(render_screen)
        {
            window.clear(COLOR_CLEAR);            
            window.draw(map);

            for (size_t i = 0; i < aisles->size(); i++)
            {
                Aisle aisle = (*aisles)[i];
                window.draw(aisle.label);
            }
            
            if(selected_event < pick_events->size() && selected_event >= 0)
            {                
                PickEvent pick_event = (*pick_events)[selected_event];

                if(pick_event.aisle_valid)
                {
                    Aisle &aisle = *pick_event.m_aisle;
                    overlay.pick_position_overlay_label.setString(aisle.id + " " + std::to_string(pick_event.pick_position));
                    
                    int j = aisle.pick_positions-pick_event.pick_position;
                    sf::Vector2f position;
                    position.x = aisle.x + (AISLE_WIDTH_INNER * (int)(j%2==0)) + (PP_WIDTH/2);
                    position.y = aisle.y + (PP_WIDTH * (j/2)) + ((int)(PP_WIDTH*0.2) * ((j/2)/3)) + (PP_HEIGHT/2);  
                    
                    sf::FloatRect bounds = overlay.pick_position_overlay_label.getLocalBounds();
                    sf::Vector2f size;
                    size.x = bounds.width*1.1;
                    size.y = bounds.height*2;     
                    overlay.pick_position_overlay_background.setSize(size);
                    overlay.pick_position_overlay_background.setPosition(position);                
                    overlay.pick_position_overlay_label.setPosition(position + sf::Vector2f(size.x*0.05,size.y*0.1));

                    for (size_t i = 0; i < 2; i++)
                    {
                        if(pick_event.valid_ap[i])
                        {
                            AccessPoint* ap = pick_event.ap[i];

                            float angle = std::atan2(position.y - ap->y, position.x - ap->x);
                            float normal = angle - PI/2;

                            int delta = std::abs(pick_event.timestamp - pick_event.timestamp_ap[i]);
                            delta = (delta >= 5000) ? 5000:delta;
                            delta = (delta == 0) ? 1:delta;

                            float f = (float)(1.0-(delta / 10000.0));
                            float cos_n_4 = std::cos(normal) * 2;
                            float sin_n_4 = std::sin(normal) * 2;

                            pick_event_ap_connection[0].position.x = position.x + cos_n_4;
                            pick_event_ap_connection[0].position.y = position.y + sin_n_4;
                            pick_event_ap_connection[1].position.x = ap->x + cos_n_4;
                            pick_event_ap_connection[1].position.y = ap->y + sin_n_4;
                            pick_event_ap_connection[2].position.x = ap->x - cos_n_4;
                            pick_event_ap_connection[2].position.y = ap->y - sin_n_4;
                            pick_event_ap_connection[3].position.x = position.x - cos_n_4;
                            pick_event_ap_connection[3].position.y = position.y - sin_n_4;
                        
                            sf::Color color(166, 180, 1);                        
                            pick_event_ap_connection[0].color = pick_event_ap_connection[1].color = pick_event_ap_connection[2].color = pick_event_ap_connection[3].color = color;
                            window.draw(pick_event_ap_connection);
                        }
                    }  
                    window.draw(overlay.pick_position_overlay_background);
                    window.draw(overlay.pick_position_overlay_label);
                }
            }            
            
            window.draw(circles);

            for (size_t i = 0; i < access_points->size(); i++)
            {           
                AccessPoint access_point = (*access_points)[i];

                window.draw(access_point.label);

                if(access_point.hover || access_point.toggle_overlay)
                {                     
                    overlay.access_point_overlay_label.setString(access_point.address);

                    sf::FloatRect bounds = overlay.access_point_overlay_label.getLocalBounds();
                    sf::Vector2f size = sf::Vector2f(bounds.width*1.1,bounds.height*2);                
                    sf::Vector2f position = sf::Vector2f(access_point.x+12,access_point.y-28);

                    overlay.access_point_overlay_background.setSize(size);
                    overlay.access_point_overlay_background.setPosition(position);                
                    overlay.access_point_overlay_label.setPosition(position + sf::Vector2f(size.x*0.05,size.y*0.1));
                    window.draw(overlay.access_point_overlay_background);
                    window.draw(overlay.access_point_overlay_label);
                }
            }            

            window.setView(view);
            window.display();
            
            render_screen = false;
        }

        #pragma endregion
        
    }

    return 0;
}