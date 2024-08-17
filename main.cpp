#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <stdint.h>
#include <time.h>

struct MidiEvent
{
    std::vector<uint8_t> delta_time;
    uint8_t event_type;
    std::vector<uint8_t> event_data;

    void reset(void)
    {
        delta_time.clear();
        event_type = 0;
        event_data.clear();
    }
};


std::vector<MidiEvent> events;





int main(int argc, char * argv[])
{
    std::string outfile_path = "";

    srand(time(nullptr));


    std::ofstream outfile;
    outfile.open("output.mid", std::ios::binary | std::ios::out);

    // write header and header length
    outfile.write("MThd", 4);

    uint8_t num = 0;
    outfile.write((char*)&num, 1);
    outfile.write((char*)&num, 1);
    outfile.write((char*)&num, 1);

    num = 6;
    outfile.write((char*)&num, 1);


    // write format
    num = 0;
    outfile.write((char*)&num, 1);

    num = 0;
    outfile.write((char*)&num, 1);

    // write num tracks
    num = 0;
    outfile.write((char*)&num, 1);

    num = 1;
    outfile.write((char*)&num, 1);

    // write timing
    uint8_t tempo1 = 0x00;
    uint8_t tempo2 = 0x60;

    outfile.write((char*)&tempo1, 1);
    outfile.write((char*)&tempo2, 1);

    // write track
    outfile.write("MTrk", 4);
    unsigned int track_chunk_length = 0;

    MidiEvent e;

    // time signature
    e.delta_time.push_back(0x00);
    e.event_type = 0xFF;
    e.event_data.push_back(0x58);
    e.event_data.push_back(0x04);
    e.event_data.push_back(0x04);
    e.event_data.push_back(0x02);
    e.event_data.push_back(0x18);
    e.event_data.push_back(0x08);
    events.push_back(e);
    e.reset();

    track_chunk_length += 8;

    // tempo
    e.delta_time.push_back(0x00);
    e.event_type = 0xFF;
    e.event_data.push_back(0x51);
    e.event_data.push_back(0x03);
    e.event_data.push_back(0x05);
    e.event_data.push_back(0xB8);
    e.event_data.push_back(0xD8);
    events.push_back(e);
    e.reset();

    track_chunk_length += 7;

    // set to guitar
    e.delta_time.push_back(0x00);
    e.event_type = 0xC0;
    e.event_data.push_back(24);
    events.push_back(e);
    e.reset();

    track_chunk_length += 3;

    for (unsigned int i = 0; i < 30000; i++)
    {
        e.delta_time.push_back(0);


        e.event_type = 0x90;
        char note = 32 + rand() % 64;
        char velocity = 32 + rand() % 63;
        e.event_data.push_back(note);
        e.event_data.push_back(velocity);
        events.push_back(e);
        e.reset();

        track_chunk_length += 4;
        e.delta_time.push_back(0);
        e.event_type = 0x90;
        char note2 = 32 + rand() % 64;
        char velocity2 = 32 + rand() % 63;
        e.event_data.push_back(note2);
        e.event_data.push_back(velocity2);
        events.push_back(e);
        e.reset();

        track_chunk_length += 4;

        // note off
        e.delta_time.push_back((char)(48));
        e.event_type = 0x80;
        e.event_data.push_back(note);
        e.event_data.push_back(64);
        events.push_back(e);
        e.reset();

        track_chunk_length += 4;

        e.delta_time.push_back(0);
        e.event_type = 0x80;
        e.event_data.push_back(note2);
        e.event_data.push_back(64);
        events.push_back(e);
        e.reset();

        track_chunk_length += 4;
    }


    e.delta_time.push_back(0);
    e.event_type = 0xFF;
    e.event_data.push_back(0x2F);
    e.event_data.push_back(0x00);
    events.push_back(e);
    e.reset();

    track_chunk_length += 4;

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