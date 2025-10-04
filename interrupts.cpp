/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include "interrupts.hpp"

int main(int argc, char** argv) {
    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);
    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/

    int currentTime = 0;
    int contextSaveTime = 10;
    int isr_duration;
    int device_driver_time = 24;

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        
        auto [activity, duration_intr] = parse_trace(trace);
        isr_duration = delays[duration_intr]; //duration of isr (both syscall and end_io)

        /******************ADD YOUR SIMULATION CODE HERE*************************/

        //CPU Burst
        if (activity == "CPU")
        {
            execution += std::to_string(currentTime) + ", " + std::to_string(duration_intr) + ", CPU burst\n";
            currentTime += duration_intr;
        }

        else if(activity == "SYSCALL")
        {
            /*boilerplate calclates all the steps in the interrupt process
            up to finding the ISR address*/
            auto pair = intr_boilerplate(currentTime, duration_intr, contextSaveTime, vectors);
            execution += pair.first; //total excecution time
            currentTime += pair.second; //current time

            //execute ISR
            /*
                Depending on the interrupt number, calculate where in memory the ISR start address is (time: negligible; use a duration of 1 ms) 
                Get ISR address from vector table (time: negligible; use a duration of 1 ms) Execute ISR body. 
                Include all the activities carried by the ISR (each activity: say 40ms 
            */
            
            //call the driver
            execution += std::to_string(currentTime) + ", " + std::to_string(device_driver_time) + ", call device driver\n";
            currentTime += device_driver_time;

            //execute ISR
            execution += std::to_string(currentTime) + ", " + std::to_string(isr_duration) + ", ISR activity\n";
            currentTime += isr_duration;   
        }

        else if(activity == "END_IO")
        {
            //IRET
            execution += std::to_string(currentTime) + ", " + std::to_string(1) + ", IRET\n";
            currentTime++;

            //interrupt
            execution += std::to_string(currentTime) + ", " +  std::to_string(isr_duration)  + ", switch back to user mode\n";
            currentTime += isr_duration;      
        }


        else
        {
            std::cerr << "Activity " + activity + " is unknown.\n";
        }

        /************************************************************************/

    }

    input_file.close();
    write_output(execution);

    return 0;
}
