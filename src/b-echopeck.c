/**
    Bollie Echopeck - (c) 2017 Thomas Ebeling https://ca9.eu

    This file is part of bollie-echopeck.lv2

    bollie-echopeck.lv2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    bollie-echopeck.lv2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
* \file b-echopeck.c
* \author Bollie
* \date 19 Jun 2017
* \brief An LV2 echo plugin
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define URI "https://ca9.eu/lv2/bollie-echopeck"

#define MAX_TAPE_LEN 192001
#define DELAY_MS 75
#define WOW 1.0


/**
* Make a bool type available. ;)
*/
typedef enum { false, true } bool;


/**
* Enumeration of LV2 ports
*/
typedef enum {
    BEP_INPUT_CONTROL   = 0,
    BEP_LENGTH_OF_SWELL = 1,
    BEP_VOLUME          = 2,
    BEP_BASS_TREBLE     = 3,
    BEP_SELECTOR        = 4,
    BEP_SWITCH          = 5,
    BEP_INPUT           = 6,
    BEP_OUTPUT          = 7,
    BEP_LEVEL_INDICATOR = 8,
    BEP_TRIM_DRY        = 9
} PortIdx;

typedef struct {
    float   pos;
    float   cur_gain;
    float   tgt_gain;
    float   delay;
} PlayHead;

/**
* Struct for THE BollieEchopeck instance, the host is going to use.
*/
typedef struct {
    const float* ctl_input_control;     ///< Input Volume
    const float* ctl_length_of_swell;   ///< Feedback
    const float* ctl_volume;            ///< Volume of echo
    const float* ctl_bass_treble;       ///< Bass/Treble
    const float* ctl_selector;          ///< Selector
    const float* ctl_switch;            ///< Switch
    float* ctl_level_indicator;         ///< Output for level indicator
    const float* ctl_trim_dry;          ///< Dry signal trimming
    const float* input;                 ///< Input port
    float* output;                      ///< Output port

    double rate;                        ///< Current sample rate

    PlayHead playheads[4];              ///< The playheads
    int pos_w;                          ///< Position of the write head

    float buffer[MAX_TAPE_LEN];         ///< delay buffer

    double lfo_x;                       ///< LFO increment

    float cur_length_of_swell;          ///< Current feedback
    float tgt_length_of_swell;          ///< Current feedback

    float cur_volume;                   ///< Volume
    float tgt_volume;                   ///< Volume

    float cur_switch;                   ///< Current switch value

    float cur_trim_dry;                 ///< Dry volume trimmer
    float tgt_trim_dry;                 ///< Dry volume trimmer

    float fil_a0;                       ///< a0 for feedback filter
    float fil_b1;                       ///< a0 for feedback filter
    float fil_z1;                       ///< a0 for feedback filter
} BollieEchopeck;


/**
* Instantiates the plugin
* Allocates memory for the BollieEchopeck object and returns a pointer as
* LV2Handle.
*/
static LV2_Handle instantiate(const LV2_Descriptor * descriptor, double rate,
    const char* bundle_path, const LV2_Feature* const* features) {
    
    BollieEchopeck *self = (BollieEchopeck*)calloc(1, sizeof(BollieEchopeck));

    // Memorize sample rate for calculation
    self->rate = rate;

    return (LV2_Handle)self;
}


/**
* Used by the host to connect the ports of this plugin.
* \param instance current LV2_Handle (will be cast to BollieEchopeck*)
* \param port LV2 port index, maches the enum above.
* \param data Pointer to the actual port data.
*/
static void connect_port(LV2_Handle instance, uint32_t port, void *data) {
    BollieEchopeck *self = (BollieEchopeck*)instance;

    switch ((PortIdx)port) {
        case BEP_INPUT_CONTROL:
            self->ctl_input_control = data;
            break;
        case BEP_LENGTH_OF_SWELL:
            self->ctl_length_of_swell = data;
            break;
        case BEP_VOLUME:
            self->ctl_volume = data;
            break;
        case BEP_BASS_TREBLE:
            self->ctl_bass_treble = data;
            break;
        case BEP_SELECTOR:
            self->ctl_selector = data;
            break;
        case BEP_SWITCH:
            self->ctl_switch = data;
            break;
        case BEP_INPUT:
            self->input = data;
            break;
        case BEP_OUTPUT:
            self->output = data;
            break;
        case BEP_LEVEL_INDICATOR:
            self->ctl_level_indicator = data;
            break;
        case BEP_TRIM_DRY:
            self->ctl_trim_dry = data;
            break;
    }
}
    

/**
* This has to reset all the internal states of the plugin
* \param instance pointer to current plugin instance
*/
static void activate(LV2_Handle instance) {
    BollieEchopeck* self = (BollieEchopeck*)instance;
    // Let's remove all that noise
    for (int i = 0 ; i < MAX_TAPE_LEN ; ++i) {
        self->buffer[i] = 0;
    }
    self->pos_w = 0;

    // Set up delay times
    for (int i = 0 ; i < 4 ; ++i) {
        self->playheads[i].delay = (i+1.0f) * DELAY_MS/1000 * self->rate;
        self->playheads[i].pos = 0;
    }
    self->lfo_x = 0;
    self->cur_length_of_swell = 0;
    self->cur_switch = 0;
    self->cur_trim_dry = 0;

    self->fil_b1 = exp(-2.0 * M_PI * (950 / self->rate));
    self->fil_a0 = 1.0 - self->fil_b1;
    self->fil_z1 = 0.0f;
}

static void switch_heads(BollieEchopeck* self, int sw) {
    for (int i = 0 ; i < 4 ; ++i) {
        self->playheads[i].tgt_gain = 0;
    }

    switch(sw) {
        case 1:
            self->playheads[0].tgt_gain = 1;
            break;
        case 2:
            self->playheads[1].tgt_gain = 1;
            break;
        case 3:
            self->playheads[2].tgt_gain = 1;
            break;
        case 4:
            self->playheads[3].tgt_gain = 1;
            break;
        case 5:
            self->playheads[0].tgt_gain = 1;
            self->playheads[1].tgt_gain = 1;
            break;
        case 6:
            self->playheads[1].tgt_gain = 1;
            self->playheads[2].tgt_gain = 1;
            break;
        case 7:
            self->playheads[2].tgt_gain = 1;
            self->playheads[3].tgt_gain = 1;
            break;
        case 8:
            self->playheads[0].tgt_gain = 1;
            self->playheads[2].tgt_gain = 1;
            break;
        case 9:
            self->playheads[1].tgt_gain = 1;
            self->playheads[3].tgt_gain = 1;
            break;
        case 10:
            self->playheads[0].tgt_gain = 1;
            self->playheads[1].tgt_gain = 1;
            self->playheads[2].tgt_gain = 1;
            break;
        case 11:
            self->playheads[1].tgt_gain = 1;
            self->playheads[2].tgt_gain = 1;
            self->playheads[3].tgt_gain = 1;
            break;
        case 12:
            self->playheads[0].tgt_gain = 1;
            self->playheads[1].tgt_gain = 1;
            self->playheads[2].tgt_gain = 1;
            self->playheads[3].tgt_gain = 1;
            break;
            
    }
}


/**
* Main process function of the plugin.
* \param instance  handle of the current plugin
* \param n_samples number of samples in this current input block.
*/
static void run(LV2_Handle instance, uint32_t n_samples) {
    BollieEchopeck* self = (BollieEchopeck*)instance;

    // Pull stuff from heap
    int pos_w = self->pos_w;
    double rate = self->rate;
    double lfo_x = self->lfo_x;

    // Current switch setting
    if (*self->ctl_switch != self->cur_switch) {
        float s = *self->ctl_switch;
        if (s < 1 || s > 12) s = 1;
        switch_heads(self, *self->ctl_switch);
    }

    // Length of Swell
    float cur_length_of_swell = self->cur_length_of_swell;
    float tgt_length_of_swell = 0;
    const float ctl_length_of_swell = *self->ctl_length_of_swell;
    if (ctl_length_of_swell > 0 && ctl_length_of_swell < 10) {
        tgt_length_of_swell = powf(10.0f, (ctl_length_of_swell-10) * 0.2f);
    }
    else if (ctl_length_of_swell == 10) {
        tgt_length_of_swell = 1;
    }

    // Volume
    float cur_volume = self->cur_volume;
    float tgt_volume = 0;
    const float ctl_volume = *self->ctl_volume;
    if (ctl_volume > 0 && ctl_volume < 10) {
        tgt_volume = powf(10.0f, (ctl_volume-10) * 0.2f);
    }
    else if (ctl_volume == 10) {
        tgt_volume = 1;
    }

    // Volume
    float cur_trim_dry = self->cur_trim_dry;
    float tgt_trim_dry = 0;
    const float ctl_trim_dry = *self->ctl_trim_dry;
    if (ctl_trim_dry > 0 && ctl_trim_dry < 10) {
        tgt_trim_dry = powf(10.0f, (ctl_trim_dry-10) * 0.2f);
    }
    else if (ctl_trim_dry == 10) {
        tgt_trim_dry = 1;
    }

    // Loop over the block of audio we got
    for (unsigned int i = 0 ; i < n_samples ; ++i) {
        float sample_in = self->input[i];
        float sample_out = 0;

        // Parameter smoothing
        cur_length_of_swell = tgt_length_of_swell * 0.01f 
            + cur_length_of_swell *0.99f;

        cur_volume = tgt_volume * 0.01f 
            + cur_volume *0.99f;

        cur_trim_dry = tgt_trim_dry * 0.01f 
            + cur_trim_dry *0.99f;

        // LFO
        float w = WOW/1000 * rate;
        float lfo_offset = w * sin(2 * (2* M_PI) * lfo_x / rate);
        lfo_x = lfo_x + 1 >= rate ? 0 : lfo_x+1;

        // Playback Heads
        for (unsigned int h = 0 ; h < 4 ; ++h) {
            // Parameter smoothing for head gain coeff
            self->playheads[h].cur_gain = self->playheads[h].tgt_gain * 0.01f
                + self->playheads[h].cur_gain * 0.99f;

            float out = 0;
            float d = self->playheads[h].delay;
            float x = (float)pos_w - d;
            x += lfo_offset;
            if (x < 0) {
                x = MAX_TAPE_LEN + x;
            }
            self->playheads[h].pos = x;

            // Calculate fraction
            double x1, frac;
            frac = modf(x, &x1);
            if (frac == 0) {
               out = self->buffer[(int)x1]; 
            }
            else {
                // Rule out edge cases
                float y1 = self->buffer[(int)x1];
                int x2 = (int)x1+1;
                float y2 = self->buffer[x2 >= MAX_TAPE_LEN ? 0 : x2];
                out = y1 + (y2-y1)/(x2-x1) * (x-x1);
            }
            sample_out += out * self->playheads[h].cur_gain;
        }
        
        // constantly write to the buffer
        self->fil_z1 = (sample_out * self->fil_a0 
            + self->fil_z1 * self->fil_b1);
        self->buffer[pos_w] = sample_in + self->fil_z1 * cur_length_of_swell;

        self->output[i] = self->input[i] * cur_trim_dry + sample_out * cur_volume;
        pos_w = (pos_w+1 >= MAX_TAPE_LEN ? 0 : pos_w+1);
    }
    self->cur_length_of_swell = cur_length_of_swell;
    self->cur_volume = cur_volume;
    self->cur_trim_dry = cur_trim_dry;
    self->pos_w = pos_w;
    self->lfo_x = lfo_x;

}


/**
* Called, when the host deactivates the plugin.
*/
static void deactivate(LV2_Handle instance) {
}


/**
* Cleanup, freeing memory and stuff
*/
static void cleanup(LV2_Handle instance) {
    free(instance);
}


/**
* extension stuff for additional interfaces
*/
static const void* extension_data(const char* uri) {
    return NULL;
}


/**
* Descriptor linking our methods.
*/
static const LV2_Descriptor descriptor = {
    URI,
    instantiate,
    connect_port,
    activate,
    run,
    deactivate,
    cleanup,
    extension_data
};


/**
* Symbol export using the descriptor above
*/
LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index) {
    switch (index) {
        case 0:  return &descriptor;
        default: return NULL;
    }
}
