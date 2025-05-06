#include <iostream>
#include <fstream>
#include <csignal>
#include <iomanip>
#include <chrono>
#include <thread>
#include <libhackrf/hackrf.h>

hackrf_device* device = nullptr;
std::ofstream outfile;

// Called when HackRF receives a buffer of data
int rx_callback(hackrf_transfer* transfer) {
    outfile.write(reinterpret_cast<char*>(transfer->buffer), transfer->valid_length);
    return 0;
}

int main(int argc, char** argv) {

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <freq to record from>\n";
        return 1;
    }

    double freq = atoi(argv[1]); // default Frequency in MHz

    if (hackrf_init() != HACKRF_SUCCESS) {
        std::cerr << "HackRF init failed.\n";
        return 1;
    }

    if (hackrf_open(&device) != HACKRF_SUCCESS) {
        std::cerr << "Failed to open HackRF device.\n";
        hackrf_exit();
        return 1;
    }

    // Set frequency to capture from (you can change this)
    if(freq == 0) {
        hackrf_stop_rx(device);
        hackrf_close(device);
        hackrf_exit();
        std::cout << "Exiting...\n";
        return 0;
    }

    hackrf_set_freq(device, freq);

    // Set sample rate and gain
    hackrf_set_sample_rate(device, 10000000); // 10 MSPS
    hackrf_set_lna_gain(device, 40);          // Optional: adjust gain
    hackrf_set_vga_gain(device, 62);
    hackrf_set_amp_enable(device, 1);         // Enable amplifier

    // Open output file
    outfile.open("signal.bin", std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to open output file.\n";
        return 1;
    }

    std::cout << "[+]Recording signal[+]\n";
    hackrf_start_rx(device, rx_callback, nullptr);

    // Wait 5 seconds
    while (true)
    {
        std::cout<<"type 'q' then enter to quit"<<std::endl;
        char c;
        std::cin>>c;
        if(c == 'q')
        {
            std::cout<<"quitting..."<<std::endl;
            break;
        }
    }
    //stop = true;

    hackrf_stop_rx(device);
    outfile.close();
    hackrf_close(device);
    hackrf_exit();

    std::cout << "[*] Done recording.\n";
    return 0;
}   
