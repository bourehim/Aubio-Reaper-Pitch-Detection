#define REAPERAPI_IMPLEMENT

#include "utils.h"
#include <reaper_plugin.h>
#include <reaper_plugin_functions.h>
#include <stdio.h>
#include <stdlib.h>
#include <aubio/aubio.h>
#include <sndfile.h>
#include <math.h>
#include <aubio/aubio.h>

// Function to perform the specified operation
int PerformOperation() {
    int noteNumber = 5 + 5;

    //int noteNumber = static_cast<int>(69 + 12 * std::log2(60.0 / 440.0));
    return noteNumber;
}

// Function declaration
int process_audio();


// Function to convert pitch to note name
const char *pitchToNote(float pitch)
{
    // Calculate note number based on MIDI standard
    int noteNumber = 69 + 12 * log2(pitch / 440.0);

    noteNumber -= 12;

    // Array of note names
    const char *noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    // Calculate octave and index within the note names array
    int octave = noteNumber / 12;
    int noteIndex = noteNumber % 12;

    // Create a string with the note name and octave
    static char noteString[10];
    snprintf(noteString, sizeof(noteString), "%s%d", noteNames[noteIndex], octave);

    return noteString;
}


extern "C" {
    REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t* rec) {
        if (!rec) {
            return 0;
        }
        if (rec->caller_version != REAPER_PLUGIN_VERSION || !rec->GetFunc) {
            return 0;
        }
        REAPERAPI_LoadAPI(rec->GetFunc);
        ShowConsoleMsg("Loaded reaper-minimal-plugin\n");

        // Call the function and display the result
        int noteNumber = process_audio();

        const char *mostCommonNote = pitchToNote(noteNumber);

        ShowConsoleMsg("MostCommonNote:\n");
        ShowConsoleMsg(mostCommonNote);
        ShowConsoleMsg("\n");

        char msg[256];
        ShowConsoleMsg("Note number\n");
        sprintf(msg, "%d", noteNumber);
        ShowConsoleMsg(msg);

        return 1;
    }
};



int process_audio()
{
    // 1. allocate some memory
    uint_t n = 0; // frame counter
    uint_t win_s = 1024; // window size
    uint_t hop_s = win_s / 4; // hop size
    uint_t samplerate = 44100; // samplerate

    // Open the WAV file
    const char *filename = "/home/android1/Desktop/MAIN_DEV/YB_Dev/reaper-minimal-plugin/src/sample2_A2.wav";

    SF_INFO sfinfo;
    SNDFILE *sndfile = sf_open(filename, SFM_READ, &sfinfo);
    if (!sndfile)
    {
        fprintf(stderr, "Error: Unable to open input file %s.\n", filename);
        return 1;
    }

    // create some vectors
    fvec_t *input = new_fvec(hop_s); // input buffer
    fvec_t *out = new_fvec(1);       // output candidates

    // create pitch object
    aubio_pitch_t *o = new_aubio_pitch("default", win_s, hop_s, samplerate);

    // Dynamic array to store the frequency counts for each pitch
    int *pitchCounts = (int *)calloc(1200, sizeof(int)); // Assuming a larger range of pitch values

    // 2. do something with it
    while (n < 100)
    {
        // Read `hop_s` new samples into `input` from the WAV file
        sf_count_t read_count = sf_read_float(sndfile, input->data, hop_s);
        if (read_count <= 0)
            break; // End of file or error

        // execute pitch
        aubio_pitch_do(o, input, out);

        // get the pitch value
        float pitch = out->data[0];

        // convert pitch to note name
        const char *note = pitchToNote(pitch);

        // Increment the count for the corresponding pitch
        int pitchIndex = round(pitch);
        if (pitchIndex >= 0 && pitchIndex < 1200) // Ensure the index is within the array bounds
        {
            pitchCounts[pitchIndex]++;
        }

        n++;
    }

        // Find the pitch with the maximum count
        int maxPitchIndex = 0;
        for (int i = 1; i < 1200; i++)
        {
            if (pitchCounts[i] > pitchCounts[maxPitchIndex])
            {
                maxPitchIndex = i;
            }
        }

    // 3. clean up memory
    sf_close(sndfile);
    del_aubio_pitch(o);
    del_fvec(out);
    del_fvec(input);
    free(pitchCounts);
    aubio_cleanup();

    return maxPitchIndex;
}
