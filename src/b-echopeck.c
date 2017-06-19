/**
    Bollie Echopeck - (c) 2016 Thomas Ebeling https://ca9.eu

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

#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define URI "https://ca9.eu/lv2/bollie-echopeck"

#define MAX_TAPE_LEN 1920001


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
    BEP_LEVEL_INDICATOR = 8
} PortIdx;

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
    const float* input;                 ///< Input port
    float* output;                      ///< Output port

    double rate;                        ///< Current sample rate

    float buffer[MAX_TAPE_LEN];     ///< delay buffer
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
}


/**
* Main process function of the plugin.
* \param instance  handle of the current plugin
* \param n_samples number of samples in this current input block.
*/
static void run(LV2_Handle instance, uint32_t n_samples) {
    BollieEchopeck* self = (BollieEchopeck*)instance;

    // Loop over the block of audio we got
    for (unsigned int i = 0 ; i < n_samples ; ++i) {
        self->output[i] = self->input[i];
    }

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
