//----------------------------------------------------------------------------
// name: Waveforms.cpp
// desc: creates various kinds of (audio) waves, real-time
//
// author: Morgan Bryant (mrbryant@stanford.edu)
//   date: fall 2014
//   uses: RtAudio by Gary Scavone
// partially adapted/inspired by HelloSine by Ge Wang
//----------------------------------------------------------------------------





//  ~*~*~  Necessary includes, defines, constants, and variables  ~*~*~



#include "RtAudio.h"
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
using namespace std;

// datatype:
#define SAMPLE double
// corresponding format for RtAudio
#define MY_FORMAT RTAUDIO_FLOAT64
// sample rate
#define MY_SRATE 44100
// number of channels
#define MY_CHANNELS 2
// for convenience
#define MY_PIE 3.14159265358979


// global for frequency
SAMPLE g_freq;
// global sample number variable
SAMPLE g_t = 0;
// global for width
SAMPLE g_width;




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



//   ~*~*~    utility functions.  include i/o, valid UI checks, ...   ~*~*~


/* int getDouble(string message, double min, double max);
 *
 * Gets a single double from input line.
 * Accepted values lie fully in the (closed, ie inclusive) set defined by min and max.
 * If { max <= min }, then the max is ignored.
 */
double getDouble(string message, double min, double  max) {
    cout << message;
    double  x;
    string line;
    while (true) {
        getline(cin, line);
        istringstream stream(line);
        stream >> x >> ws;
        if (max <= min) {
            if (x >= min) {
                if (stream && stream.eof()) break;
            }
        } else {
            if (x >= min && x <= max) {
                if (stream && stream.eof()) break;
            }
        }
        //throw "Error [001]";
        cout << endl << "Bad input. " << message;
    }
    return x;
}




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//  ~*~*~  Callback Functions  ~*~**~

// Here are the callback functions.  When called they define the different 
//   fundamental waves.


int callmeEditFile( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
                	double streamTime, RtAudioStreamStatus status, void * data ) {

    SAMPLE * buffy = (SAMPLE *)outputBuffer;
	

	return 0;
}
 




/* name: callmeSine()
 * desc: audio callback.  Makes a plain, basic sine wave.
 */
int callmeSine( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
                double streamTime, RtAudioStreamStatus status, void * data ) {
    // debug print something out per callback
    cerr << ".";

    // cast!
    SAMPLE * buffy = (SAMPLE *)outputBuffer;
    
    // fill
    for( int i = 0; i < numFrames; i++ )
    {
        // generate signal
        buffy[i*MY_CHANNELS] = sin( 2 * MY_PIE * g_freq * g_t / MY_SRATE );
        
        // copy into other channels
        for( int j = 1; j < MY_CHANNELS; j++ )
            buffy[i*MY_CHANNELS+j] = buffy[i*MY_CHANNELS];
            
        // increment sample number
        g_t += 1.0;
    }
    
    return 0;
}




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//   ~*~*~   Main   ~*~*~


/* int main(int argc, char* argv[]);
 * entry point for program.  Indeed uses some well-formed arguments.
 */
int main(int argc, char* argv[]) {    
    
    // instantiate RtAudio object
    RtAudio adac;
    // variables
    unsigned int bufferBytes = 0;
    // frame size
    unsigned int bufferFrames = 512;

    if (adac.getDeviceCount() < 1) {
	std::cout << "seeing no speakers, man" << std::endl;
	exit(1);
    }

    // Let RtAudio print messages to stderr.
    adac.showWarnings(true);

    // set input and output parameters
    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = adac.getDefaultInputDevice();
    iParams.nChannels = MY_CHANNELS;
    iParams.firstChannel = 0;
    oParams.deviceId = adac.getDefaultOutputDevice();
    oParams.nChannels = MY_CHANNELS;
    oParams.firstChannel = 0;

    // create stream options
    RtAudio::StreamOptions options;


	if (argc != 1) {
		cout << "File to edit must be titled 'samp_to_be_logged.wav'!" << endl;
		exit (1);
	} else {
		try {
            // open a stream
            adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                             &callmeEditFile, (void *)&bufferBytes, &options );
        } catch( RtError& e ) {
            // error!
            cout << e.getMessage() << endl;
            exit( 1 );
        }

	}

    // compute
    bufferBytes = bufferFrames * MY_CHANNELS * sizeof(SAMPLE);
    // test RtAudio functionality for reporting latency.
    cout << "stream latency: " << adac.getStreamLatency() << " frames" << endl;
    try {
        // start stream
        adac.startStream();
        // get input
        char input;
        std::cout << "running... press <enter> to quit (buffer frames: "
                  << bufferFrames << ")" << endl;
        std::cin.get(input);
        // stop the stream.
        adac.stopStream();
    } catch( RtError& e ) {
        // print error message
        cerr << e.getMessage() << endl;
        if (adac.isStreamOpen()) adac.closeStream();
        return 1;
    }

    if (adac.isStreamOpen()) adac.closeStream();


    return 0;
}

