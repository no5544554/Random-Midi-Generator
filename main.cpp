#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <stdint.h>
#include <time.h>

constexpr uint8_t QUARTER_NOTE = 0x60;

constexpr uint8_t PROGRAM_CHANGE = 0xC0;
constexpr uint8_t NOTE_ON = 0x90;
constexpr uint8_t NOTE_OFF = 0x80;

// constexpr uint8_t NYLON_GUITAR = 98;


struct MidiEvent
{
    std::vector<uint8_t> delta_time;
    uint8_t event_type = 0;
    std::vector<uint8_t> event_data;

    void reset(void)
    {
        delta_time.clear();
        event_type = 0;
        event_data.clear();
    }
};


std::vector<MidiEvent> events;
unsigned int track_chunk_length = 0;


void add_event(std::vector<uint8_t> delta_time, uint8_t event_type, std::vector<uint8_t> event_data);


int main(int argc, char * argv[])
{
    srand((unsigned int)time(nullptr));

    std::ofstream outfile;
    outfile.open("outfile.mid", std::ios::binary | std::ios::out);

    // bytes to write
    char header_bytes[] = { 
        'M', 'T', 'h', 'd',         // header label
        0, 0, 0, 6,                 // header length
        0, 0,                       // midi format
        0, 1,                       // num tracks
        0, QUARTER_NOTE             // timing
    };

    outfile.write(header_bytes, sizeof(header_bytes));

    // write track
    outfile.write("MTrk", 4);

    // time signature
    add_event({ 0x00 }, 0xFF, { 0x58, 0x04, 0x04, 0x02, 0x18, 0x08 });

    // tempo
    add_event({ 0x00 }, 0xFF, { 0x51, 0x03, 0x05, 0xB8, 0xD8 });

    

    // add a bunch of random notes
    for (unsigned int i = 0; i < 30000; i++)
    {
        uint8_t note = 32 + rand() % 64;
        uint8_t velocity = 32 + rand() % 63;

        uint8_t note2 = 32 + rand() % 64;
        uint8_t velocity2 = 32 + rand() % 63;

        uint8_t interval = 2;

        // set to instrument
        add_event({ 0x00 }, PROGRAM_CHANGE, { (uint8_t)(rand() % 128)});

        add_event({ 0x00 }, NOTE_ON, { note, velocity });
        add_event({ 0x00 }, NOTE_ON, { note2, velocity2 });

        int choose = rand() % 3;
        switch (choose)
        {
            case 0: interval = 2; break;
            case 1: interval = 4; break;
            case 2: interval = 1; break;
        }

        add_event({ (uint8_t)(QUARTER_NOTE / interval) }, NOTE_OFF, { note, 64 });
        add_event({ 0x00 }, NOTE_OFF, { note2, 64 });
    }

    // track end event
    add_event({ 0 }, 0xFF, { 0x2F, 0x00 });

    unsigned int big_endian_length = 0;
    big_endian_length = ((track_chunk_length >> 24) & 0x000000FF |
        (track_chunk_length << 8) & 0x00FF0000 |
        (track_chunk_length >> 8) & 0x0000FF00 |
        (track_chunk_length << 24) & 0xFF000000);


    outfile.write((char *)&big_endian_length, 4);

    for (unsigned int i = 0; i < events.size(); i++)
    {
        for (unsigned int j = 0; j < events[i].delta_time.size(); j++)
        {
            outfile.write((char *)&events[i].delta_time[j], 1);
        }

        outfile.write((char *)&events[i].event_type, 1);

        for (unsigned int j = 0; j < events[i].event_data.size(); j++)
        {
            outfile.write((char *)&events[i].event_data[j], 1);
        }
    }

    outfile.close();

    return 0;
}


void add_event(std::vector<uint8_t> delta_time, uint8_t event_type, std::vector<uint8_t> event_data)
{
    MidiEvent e;
    e.delta_time = delta_time;
    e.event_type = event_type;
    e.event_data = event_data;

    track_chunk_length += (unsigned int)e.delta_time.size() + 1 + (unsigned int)e.event_data.size();

    events.push_back(e);
}