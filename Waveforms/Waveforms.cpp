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

/* name: callmeSaw()
 * desc: audio callback.  Makes a saw wave.
 */
int callmeSaw( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
                double streamTime, RtAudioStreamStatus status, void * data ) {
    // debug print something out per callback
    cerr << ".";

    // cast!
    SAMPLE * buffy = (SAMPLE *)outputBuffer;
    
    SAMPLE curS = -1.0;

    SAMPLE prd = MY_SRATE / g_freq; // prd = period, which stays constant over the loop
    // fill
    for( int i = 0; i < numFrames; i++ )
    {
        if (g_t > prd) g_t -= (int) prd; // prd is rounded down when cast as an int
        // generate signal
	if (g_t < g_width * prd) {
            curS = -1.0 + 2.0 * g_t / (prd * g_width);
        } else {
            curS = 1.0 - 2.0 * (g_t - g_width * prd) / (prd * (1-g_width));
        }
        if (curS >= -1.0 && curS <= 1.0) { // just in case!
            buffy[i*MY_CHANNELS] = curS;

            // copy into other channels
            for( int j = 1; j < MY_CHANNELS; j++ )
                buffy[i*MY_CHANNELS+j] = buffy[i*MY_CHANNELS];
        } else {
            cerr << "warning: amplitude spike" << endl;
            exit(1);
        }
        // increment sample number
        g_t += 1.0;   
    }
    
    return 0;
}


int callmePulse( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
                double streamTime, RtAudioStreamStatus status, void * data ) {
    // debug print something out per callback
    cerr << ".";

    // cast!
    SAMPLE * buffy = (SAMPLE *)outputBuffer;

    SAMPLE prd = MY_SRATE / g_freq;
    
    // fill
    for( int i = 0; i < numFrames; i++ )
    {
        if (g_t > prd) g_t -= (int) prd;
        // generate signal
	if (g_t <= g_width * MY_SRATE / g_freq) {
            buffy[i*MY_CHANNELS] = 1.0;
        } else {
            buffy[i*MY_CHANNELS] = 0.0;
        }
        // copy into other channels
        for( int j = 1; j < MY_CHANNELS; j++ )
            buffy[i*MY_CHANNELS+j] = buffy[i*MY_CHANNELS];
            
        // increment sample number
        g_t += 1.0;   
    }
  
    return 0;
}

int callmeNoise( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
                double streamTime, RtAudioStreamStatus status, void * data ) {

    // debug print something out per callback
    cerr << ".";

    // cast!
    SAMPLE * buffy = (SAMPLE *)outputBuffer;
    
    // fill
    for( int i = 0; i < numFrames; i++ )
    {
        // generate signal
        buffy[i*MY_CHANNELS] = (double) (rand() % 128 - 64) / 64 ;
        
        // copy into other channels
        for( int j = 1; j < MY_CHANNELS; j++ )
            buffy[i*MY_CHANNELS+j] = buffy[i*MY_CHANNELS];
    }
    
    return 0;
}

int callmeImpulse( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
                double streamTime, RtAudioStreamStatus status, void * data ) {
    // debug print something out per callback
    cerr << ".";

    // cast!
    SAMPLE * buffy = (SAMPLE *)outputBuffer;
    
    // fill
    for( int i = 0; i < numFrames; i++ )
    {
        SAMPLE prd = MY_SRATE / g_freq;
        
        // generate signal
	if (g_t > prd) {
            buffy[i*MY_CHANNELS] = 1.0;
            g_t -= (int) prd;
        } else
            buffy[i*MY_CHANNELS] = 0.0;
        
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




    /*  A series of if-else switches handle and appropriately respond
        to various user input.  Also requests needed user-given information 
        when needed.  */

    if (argc == 1) {
        // Set the (plain, basic sine wave's) freq:
	g_freq = getDouble("Enter frequency (>0.1 Hz, <20k Hz):  ", 0.1, 20000.0);

        try {
            // open a stream
            adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                             &callmeSine, (void *)&bufferBytes, &options );
        } catch( RtError& e ) {
            // error!
            cout << e.getMessage() << endl;
            exit( 1 );
        }
    } else if (argc == 2) {
        if (strcmp(argv[1],"--noise") == 0) {
            adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                             &callmeNoise, (void *)&bufferBytes, &options );
        } else {
            g_freq = getDouble("Enter frequency (>0.1 Hz, <20k Hz):  ", 0.1, 20000.0);        
            if ( strcmp(argv[1],"--sine") == 0) {
                 adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                             &callmeSine, (void *)&bufferBytes, &options );
            } else if ( strcmp(argv[1],"--impulse") == 0) {
                 adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                             &callmeImpulse, (void *)&bufferBytes, &options );
            } else {
                g_width = getDouble("Enter a width (0.0-1.0):  ",0.0,1.0);                
                if ( strcmp(argv[1],"--pulse") == 0) {
                    adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                             &callmePulse, (void *)&bufferBytes, &options );
                } else if ( strcmp(argv[1],"--saw") == 0) {
                    adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                             &callmeSaw, (void *)&bufferBytes, &options );
                } else {
                    cerr << "Bad input format. Exiting." << endl;
                    if (adac.isStreamOpen()) adac.closeStream();
                    return 1;
                }
            }
        }
    } else if (argc == 3) {
        g_freq = (double) atof(argv[2]);
        if (strcmp(argv[1],"--sine") == 0) {
            adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                             &callmeSine, (void *)&bufferBytes, &options );
        } else if (strcmp(argv[1],"--impulse") == 0) {
            adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                             &callmeSaw, (void *)&bufferBytes, &options );
        } else {
            g_width = getDouble("Enter a width (0.0-1.0):  ",0.0,1.0);
            if (strcmp(argv[1],"--pulse") == 0) {
                adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                                 &callmePulse, (void *)&bufferBytes, &options );
            } else if (strcmp(argv[1],"--saw") == 0) {
                adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                                 &callmeImpulse, (void *)&bufferBytes, &options );
            } else {
                cerr << "Bad input format. Exiting." << endl;
                if (adac.isStreamOpen()) adac.closeStream();
                return 1;
            }
        }
    } else if (argc == 4) {
        g_freq = (double) atof(argv[2]);
        g_width = (double) atof(argv[3]);
        if (strcmp(argv[1],"--pulse") == 0) {
            adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                             &callmePulse, (void *)&bufferBytes, &options );
        } else if (strcmp(argv[1],"--saw") == 0) {
            adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
                             &callmeSaw, (void *)&bufferBytes, &options );
        } else {
            cerr << "Bad input format. Exiting." << endl;
            if (adac.isStreamOpen()) adac.closeStream();
            return 1;
        }
    } else {
        cerr << "Bad input format. Exiting." << endl;
        if (adac.isStreamOpen()) adac.closeStream();
        return 1;
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


