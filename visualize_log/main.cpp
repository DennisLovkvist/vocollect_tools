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

static const int aisle_count = 24;
static const int access_point_count = 29;

struct Overlay
{
    sf::RectangleShape pick_position_overlay_background;    
    sf::Text pick_position_overlay_label;

    sf::RectangleShape access_point_overlay_background;    
    sf::Text access_point_overlay_label;
};
//3m = 300 px
struct AccessPoint
{
    std::string id;
    std::string address;
    int x;int y;int r;
    sf::CircleShape circle;
    sf::Text label; 
    bool hover;
    bool toggle_overlay;   
};
struct PickEvent 
{
        int timestamp;
        std::string aisle;
        int pick_position;
        bool valid;

        std::string first_sample_access_point_mac_address;
        std::string first_sample_access_point_signal_strength;
        int first_sample_access_point_timestamp;
        sf::Vertex first_sample_line[2];  

        std::string second_sample_access_point_mac_address;
        std::string second_sample_access_point_signal_strength;
        int second_sample_access_point_timestamp;  
        sf::Vertex second_sample_line[2];
};
struct Aisle 
{
    /* data */
    int pick_positions;
    int x;int y;
    std::vector<sf::RectangleShape> rectangles;
    sf::RectangleShape rect;
    sf::Text label;
    std::string id;
    
};
struct InputState
{
    bool mouse_right_down;
    sf::Vector2i mouse_position_delta;
    sf::Vector2i mouse_position_old;
    sf::Vector2i mouse_position;
};
void DefineAccessPoint(AccessPoint &access_point,sf::Font &font,std::string address,std::string id, int x, int y,int r)
{
    
    sf::Color color = sf::Color(200, 200, 200);

    access_point.toggle_overlay = access_point.hover = false;
    access_point.address = address;
    access_point.id = id;
    access_point.x = x;
    access_point.y = y;
    access_point.r = r;

    access_point.circle.setPosition(x,y);
    access_point.circle.setRadius(r);
    access_point.circle.setFillColor(color);

    access_point.label.setFont(font);
    access_point.label.setString(id);
    access_point.label.setCharacterSize(16);
    access_point.label.setFillColor(sf::Color::Black);
    sf::FloatRect bounds = access_point.label.getLocalBounds();
    access_point.label.setPosition(access_point.x + 3,access_point.y+3);
}
void DefineAisle(Aisle &aisle,std::string label,sf::Font &font, int origin_x, int origin_y, int pick_positions)
{
    aisle.id = label;
    aisle.pick_positions = pick_positions;
    aisle.x = origin_x;
    aisle.y = origin_y;

    int spacing = (int)(PP_WIDTH*0.2);
    
    int y = origin_y;
    bool reverse = true;

    int increment = reverse ? -1:1;
    sf::Color color = sf::Color(200, 200, 200);
    for (int i = (reverse ? aisle.pick_positions-1:0); (reverse ? i >= 0:i < aisle.pick_positions); i+=increment)
    {
        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(PP_WIDTH, PP_HEIGHT));
        rect.setFillColor(color);
        int even = (i%2==0);
        int x = origin_x + AISLE_WIDTH_INNER * even;
        int lol = (int)(reverse?!even:even);
        y +=  (lol*(PP_WIDTH)) + (lol * (int)(i%3==(reverse?2:0)) * spacing);        
        rect.setPosition(x,y);
        aisle.rectangles.push_back(rect);
    }
    
    aisle.rect.setPosition(aisle.x,aisle.y-AISLE_WIDTH_INNER-PP_HEIGHT);
    aisle.rect.setSize(sf::Vector2f(AISLE_WIDTH_INNER + PP_WIDTH, AISLE_WIDTH_INNER + PP_HEIGHT));
    aisle.rect.setFillColor(color);

    aisle.label.setFont(font);
    aisle.label.setString(label);
    aisle.label.setCharacterSize(16);
    aisle.label.setFillColor(sf::Color::Black);
    sf::FloatRect bounds = aisle.label.getLocalBounds();
    aisle.label.setPosition(aisle.x + (AISLE_WIDTH_INNER/2),aisle.y - AISLE_WIDTH_INNER/2 - bounds.height);
    
}
void MapPickEvents(std::vector<PickEvent> &pick_events,std::vector<Aisle> &aisles,std::vector<AccessPoint> &access_points)
{

    for (size_t i = 0; i < pick_events.size(); i++)
    {
        PickEvent* pick_event = &pick_events[i];

        for (int j = 0; j < aisles.size(); j++)
        {
            Aisle* aisle = &aisles[j];

            if(pick_event->aisle == aisle->id)
            {
                sf::Vector2f position = aisle->rectangles[aisle->pick_positions - pick_event->pick_position].getPosition() + sf::Vector2f(PP_WIDTH/2,PP_HEIGHT/2);
                pick_event->first_sample_line[0].position = pick_event->first_sample_line[1].position = position;
                pick_event->second_sample_line[0].position = pick_event->second_sample_line[1].position = position;


                pick_event->first_sample_line[0].color  = sf::Color::Red;
                pick_event->second_sample_line[0].color  = sf::Color::Green;
                break;
            }
        }   
        int flag = 0;
        for (int j = 0; j < access_points.size() && flag < 2; j++)
        {
            AccessPoint* access_point = &access_points[j];
            if(pick_event->first_sample_access_point_mac_address == access_point->address)
            {
                pick_event->first_sample_line[1].position = sf::Vector2f(access_point->x + 12, access_point->y + 12);
                pick_event->first_sample_line[1].color  = sf::Color::Red;
                access_point->toggle_overlay = true;
                flag += 1;
            }
            else if(pick_event->second_sample_access_point_mac_address == access_point->address)
            {
                pick_event->second_sample_line[1].position = sf::Vector2f(access_point->x + 12, access_point->y + 12);
                pick_event->second_sample_line[1].color  = sf::Color::Red;
                access_point->toggle_overlay = true;
                flag += 1;
            }
        }
        pick_event->valid = (flag > 0);
    }
}

void Interact(InputState &input_state, sf::Vector2f mouse_position_coord,std::vector<Aisle> &aisles,std::vector<AccessPoint> &access_points)
{    
    for (size_t i = 0; i < access_points.size(); i++)
    {
        AccessPoint* access_point = &access_points[i];

        int dist = std::sqrt(std::pow(access_point->x + access_point->r - mouse_position_coord.x, 2) + std::pow(access_point->y + access_point->r - mouse_position_coord.y, 2) * 1.0);
        if(dist < 16)
        {
            access_point->hover = true;
        }
        else
        {
            access_point->hover = false;
        }
    }
    
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
    }
    else if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Right)
        {                    
            input_state.mouse_right_down = false;
        }
    } 

    input_state.mouse_position_delta = input_state.mouse_position_old-input_state.mouse_position;
    
    input_state.mouse_position_old = input_state.mouse_position;
}
void UpdateControls(InputState &input_state, sf::View &view)
{
    if(input_state.mouse_right_down)
    {          
        view.move(input_state.mouse_position_delta.x, input_state.mouse_position_delta.y);     
    }    
}

void LoadAisles(sf::Font &font,std::vector<Aisle> &aisles, std::string filename)
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
            DefineAisle(aisle,data[0],font,std::stoi(data[1]),std::stoi(data[2]),std::stoi(data[3]));
            aisles.push_back(aisle);
        }
    }
}
void LoadAccessPoints(sf::Font &font,std::vector<AccessPoint> &access_points, std::string filename)
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
            DefineAccessPoint(access_point,font,data[1],data[0],std::stoi(data[2]),std::stoi(data[3]),12);
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

            pick_event.first_sample_access_point_mac_address = data[3];
            pick_event.first_sample_access_point_signal_strength = data[4];
            pick_event.first_sample_access_point_timestamp = std::stoi(data[5]);

            pick_event.second_sample_access_point_mac_address = data[6];
            pick_event.second_sample_access_point_signal_strength = data[7];
            pick_event.second_sample_access_point_timestamp = std::stoi(data[8]);

            pick_events.push_back(pick_event);
        }
    }
}
void ConfigureOverlay(Overlay &overlay,sf::Font &font)
{    
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
}

int main(int argc, char *argv[])
{
    std::string path_aisle_data = "";
    std::string path_access_point_data = "";
    std::string path_pick_event_data = "";

    char result[ MAX_PATH ];
    std::string executable_file_path = std::string( result, GetModuleFileName( NULL, result, MAX_PATH ) );   
    std::string executable_path = executable_file_path.substr(0, executable_file_path.find_last_of("\\"));

    if(argc == 4)
    {
        path_aisle_data = argv[1];
        path_access_point_data = argv[2];
        path_pick_event_data = argv[3];
    }
    else
    {
        std::cout << "Expected 3 arguments, only recieved " << argc << "." << std::endl;
        std::cout << "      Example: .\\optiscan_network_map.exe aisles.txt access_points.txt pick_events.txt" << std::endl;

        std::string temp;
        std::cin >> temp;        
    }
    sf::ContextSettings settings;
    settings.antialiasingLevel = 16;



    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Optiscan network map",sf::Style::Default, settings);
    sf::Font font;
    if (!font.loadFromFile(executable_path  + "/arial.ttf"))
    {
        window.close();
    }

    
    InputState input_state; 
    input_state.mouse_right_down = false;

    Overlay overlay;
    ConfigureOverlay(overlay,font);

    std::vector<Aisle> *aisles = new std::vector<Aisle>();
    std::vector<AccessPoint> *access_points = new std::vector<AccessPoint>();
    std::vector<PickEvent> *pick_events = new std::vector<PickEvent>();

    LoadAisles(font, *aisles,path_aisle_data);   
    LoadAccessPoints(font, *access_points,path_access_point_data);     
    LoadPickEvents(*pick_events, path_pick_event_data);    

    MapPickEvents(*pick_events,*aisles,*access_points);

    sf::View view = window.getDefaultView();
    view.zoom(1);

    sf::Vector2i mp_old = sf::Mouse::getPosition();


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

            UpdateControls(input_state,view);

            sf::Vector2f mp_v = window.mapPixelToCoords(input_state.mouse_position,view);

            Interact(input_state,mp_v,*aisles, *access_points);

            int denom = (AISLE_WIDTH_INNER+(PP_HEIGHT));

            

            int g = (int)mp_v.x;

            int n = (g + abs((g % denom) - denom))/denom;
           


        }

        window.clear(sf::Color(50,50,50));

        

        for (size_t i = 0; i < aisles->size(); i++)
        {
            Aisle aisle = (*aisles)[i];

            window.draw(aisle.rect);
            window.draw(aisle.label);
            for (size_t j = 0; j < aisle.pick_positions; j++)
            {                
                window.draw(aisle.rectangles[j]);
            }
        }

        for (size_t i = 0; i < aisles->size(); i++)
        {
            Aisle aisle = (*aisles)[i];

            for (size_t j = 0; j < aisle.pick_positions; j++)
            {
                if(aisle.id == "G" && j == aisle.pick_positions-123)
                {

                    overlay.pick_position_overlay_label.setString(aisle.id + " " + std::to_string(123));

                    sf::Vector2f pp = aisle.rectangles[j].getPosition();

                    sf::FloatRect bounds = overlay.pick_position_overlay_label.getLocalBounds();
                    sf::Vector2f size = sf::Vector2f(bounds.width*1.1,bounds.height*2);                
                    sf::Vector2f position = sf::Vector2f(pp.x+4,pp.y+4);

                    overlay.pick_position_overlay_background.setSize(size);
                    overlay.pick_position_overlay_background.setPosition(position);                
                    overlay.pick_position_overlay_label.setPosition(position + sf::Vector2f(size.x*0.05,size.y*0.1));
                   
                    //window.draw(overlay.pick_position_overlay_background);
                    //window.draw(overlay.pick_position_overlay_label);
                }
            }
        }
        
        for (size_t i = 0; i < pick_events->size(); i++)
        {
            PickEvent pick_event = (*pick_events)[i];
            if(pick_event.valid)
            {
                window.draw(pick_event.first_sample_line,2, sf::Lines);
                window.draw(pick_event.second_sample_line,2, sf::Lines);
            }
        }
        
        


        for (size_t i = 0; i < access_points->size(); i++)
        {           
            AccessPoint access_point = (*access_points)[i];

            window.draw(access_point.circle);
            window.draw(access_point.label);

            if(access_point.hover || access_point.toggle_overlay)
            {                     
                overlay.access_point_overlay_label.setString(access_point.address);

                sf::FloatRect bounds = overlay.access_point_overlay_label.getLocalBounds();
                sf::Vector2f size = sf::Vector2f(bounds.width*1.1,bounds.height*2);                
                sf::Vector2f position = sf::Vector2f(access_point.x+28,access_point.y-28);

                overlay.access_point_overlay_background.setSize(size);
                overlay.access_point_overlay_background.setPosition(position);                
                overlay.access_point_overlay_label.setPosition(position + sf::Vector2f(size.x*0.05,size.y*0.1));
                window.draw(overlay.access_point_overlay_background);
                window.draw(overlay.access_point_overlay_label);
            }
        }
        
        window.setView(view);
        window.display();
    }

    return 0;
}