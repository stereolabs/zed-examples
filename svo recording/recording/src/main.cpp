///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

/****************************************************************************************
** This sample shows how to record video in Stereolabs SVO format.					   **
** SVO video files can be played with the ZED API and used with its different modules  **
*****************************************************************************************/

// ZED includes
#include <sl/Camera.hpp>

// Sample includes
#include "utils.hpp"

// Using namespace
using namespace sl;
using namespace std;


void print(std::string msg_prefix, ERROR_CODE err_code = ERROR_CODE::SUCCESS, std::string msg_suffix = "");
void parseArgs(int argc, char **argv,sl::InitParameters& param);

int main(int argc, char **argv) {

    if (argc < 2) {
        std::cout << "Usage : Only the path of the output SVO file should be passed as argument.\n";
        return 1;
    }

    // Create a ZED camera
    Camera zed;

    // Set configuration parameters for the ZED
    InitParameters initParameters;
    initParameters.camera_resolution = RESOLUTION::HD2K;
    initParameters.depth_mode = DEPTH_MODE::NONE;
    parseArgs(argc,argv,initParameters);


    // Open the camera
    ERROR_CODE err = zed.open(initParameters);
    if (err != ERROR_CODE::SUCCESS) {
        print("Opening ZED : ",err);
        zed.close();
        return 1; // Quit if an error occurred
    }

    // Enable recording with the filename specified in argument
    String path_output(argv[1]);
    err = zed.enableRecording(RecordingParameters(path_output, SVO_COMPRESSION_MODE::H264));

    if (err != ERROR_CODE::SUCCESS) {
        print("Recording ZED : ",err);
        zed.close();
        return 1;
    }

    // Start recording SVO, stop with Ctrl-C command
    print("SVO is Recording, use Ctrl-C to stop." );
    SetCtrlHandler();
    int frames_recorded = 0;

    while (!exit_app) {
        if (zed.grab() == ERROR_CODE::SUCCESS) {
            // Each new frame is added to the SVO file
            sl::RecordingStatus state = zed.getRecordingStatus();
            if (state.status)
                frames_recorded++;
            print("Frame count: " +std::to_string(frames_recorded));
        }
    }

    // Stop recording
    zed.disableRecording();
    zed.close();
    return 0;
}

void print(std::string msg_prefix, ERROR_CODE err_code, std::string msg_suffix) {
    cout <<"[Sample]";
    if (err_code != ERROR_CODE::SUCCESS)
        cout << "[Error] ";
    else
        cout<<" ";
    cout << msg_prefix << " ";
    if (err_code != ERROR_CODE::SUCCESS) {
        cout << " | " << toString(err_code) << " : ";
        cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        cout << " " << msg_suffix;
    cout << endl;
}

void parseArgs(int argc, char **argv,sl::InitParameters& param)
{
    if (argc > 2 && string(argv[2]).find(".svo")!=string::npos) {
        // SVO input mode
        cout<<"[sample][Warning] SVO input is not supported... switching to live mode"<<endl;
    } else if (argc > 2 && string(argv[2]).find(".svo")==string::npos) {
        string arg = string(argv[2]);
        unsigned int a,b,c,d,port;
        if (sscanf(arg.c_str(),"%u.%u.%u.%u:%d", &a, &b, &c, &d,&port) == 5) {
            // Stream input mode - IP + port
            string ip_adress = to_string(a)+"."+to_string(b)+"."+to_string(c)+"."+to_string(d);
            param.input.setFromStream(sl::String(ip_adress.c_str()),port);
            cout<<"[Sample] Using Stream input, IP : "<<ip_adress<<", port : "<<port<<endl;
        }
        else  if (sscanf(arg.c_str(),"%u.%u.%u.%u", &a, &b, &c, &d) == 4) {
            // Stream input mode - IP only
            param.input.setFromStream(sl::String(argv[2]));
            cout<<"[Sample] Using Stream input, IP : "<<argv[2]<<endl;
        }
        else if (arg.find("HD2K")!=string::npos) {
            param.camera_resolution = sl::RESOLUTION::HD2K;
            cout<<"[Sample] Using Camera in resolution HD2K"<<endl;
        } else if (arg.find("HD1080")!=string::npos) {
            param.camera_resolution = sl::RESOLUTION::HD1080;
            cout<<"[Sample] Using Camera in resolution HD1080"<<endl;
        } else if (arg.find("HD720")!=string::npos) {
            param.camera_resolution = sl::RESOLUTION::HD720;
            cout<<"[Sample] Using Camera in resolution HD720"<<endl;
        } else if (arg.find("VGA")!=string::npos) {
            param.camera_resolution = sl::RESOLUTION::VGA;
            cout<<"[Sample] Using Camera in resolution VGA"<<endl;
        }
    } else {
        // Default
    }
}
