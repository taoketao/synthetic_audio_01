//-----------------------------------------------------------------------------
// name: HelloSine.cpp
// desc: hello sine wave, real-time
//
// author: Ge Wang (ge@ccrma.stanford.edu)
//   date: fall 2011
//   uses: RtAudio by Gary Scavone
//-----------------------------------------------------------------------------
#include "RtAudio.h"
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
using namespace std;


// our datetype
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
// globla sample number variable
SAMPLE g_t = 0;



/* int getInteger(string message, int min, int max);
 *
 * Gets a single integer from input line.
 * Values must lie in the (closed) set defined by min and max.
 * This function probably is borrowed/adapted from lectures or the course reader.
 * If { max <= min }, then the max is ignored.
 */
    int getInteger(string message, int min, int max) {
        cout << message;
        int x;
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
    // name: callmeBasic()
    // desc: audio callback.  Makes a plain, basic sine wave.
    //-----------------------------------------------------------------------------
    int callmeBasic( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
                double streamTime, RtAudioStreamStatus status, void * data )
    {
        // debug print something out per callback
        cerr << ".";

        // cast!
        SAMPLE * buffy = (SAMPLE *)outputBuffer;

        // fill
        for( int i = 0; i < numFrames; i++ )
        {
            // generate signal
            buffy[i*MY_CHANNELS] = ::sin( 2 * MY_PIE * g_freq * g_t / MY_SRATE );

            // copy into other channels
            for( int j = 1; j < MY_CHANNELS; j++ )
                buffy[i*MY_CHANNELS+j] = buffy[i*MY_CHANNELS];

            // increment sample number
            g_t += 1.0;
        }

        return 0;
    }




//-----------------------------------------------------------------------------
// name: main()
// desc: entry point
//-----------------------------------------------------------------------------
int main( int argc, char ** argv )
{
    // instantiate RtAudio object
    RtAudio adac;
    // variables
    unsigned int bufferBytes = 0;
    // frame size
    unsigned int bufferFrames = 512;
    
    // check for audio devices
    if( adac.getDeviceCount() < 1 )
    {
        // nopes
        cout << "no audio devices found!" << endl;
        exit( 1 );
    }

    // let RtAudio print messages to stderr.
    adac.showWarnings( true );

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


    /*
     *   here is where the different callbacks will be triggered via the different args...
     */

    if (argc == 1) {
        // user sets the frequency
        g_freq = getInteger("Enter frequency (20-20K):  ", 20, 20000);

         
        // go for it
        try {
            // open a stream
            adac.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, 
 		    &bufferFrames, &callmeBasic, (void *)&bufferBytes, &options );
        }
        catch( RtError& e )
        {
            // error!
            cout << e.getMessage() << endl;
            exit( 1 );
        }

        // compute
        bufferBytes = bufferFrames * MY_CHANNELS * sizeof(SAMPLE);

        // test RtAudio functionality for reporting latency.
        cout << "stream latency: " << adac.getStreamLatency() << " frames" << endl;

        // go for it
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
        }
        catch( RtError& e )
        {
            // print error message
            cout << e.getMessage() << endl;
        }
        // close if open
        if( adac.isStreamOpen() )
            adac.closeStream();
    
        // done
        return 0;
    } 
















    // close if open
    if( adac.isStreamOpen() )
        adac.closeStream();
    
    // done
    return 0;
}
