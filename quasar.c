

#include <lv2/core/lv2.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DL_URI "https://github.com/kassoulet/quasar-lv2"

typedef enum {
    INPUT_L = 0,
    INPUT_R = 1,
    OUTPUT_L = 2,
    OUTPUT_R = 3,
    GAIN_IN = 4,
    BITS = 5,
    OVERFLOW = 6,
    DRY_WET = 7
} PortIndex;

typedef struct {
    // Port buffers
    const float *gain_in;
    const float *bits;
    const float *overflow;
    const float *dry_wet;
    const float *input_l;
    const float *input_r;
    float *output_l;
    float *output_r;
} QuasarDistortion;

static LV2_Handle instantiate(const LV2_Descriptor *descriptor, double rate, const char *bundle_path,
                              const LV2_Feature *const *features) {
    QuasarDistortion *dl = (QuasarDistortion *)malloc(sizeof(QuasarDistortion));
    return (LV2_Handle)dl;
}

static void connect_port(LV2_Handle instance, uint32_t port, void *data) {
    QuasarDistortion *dl = (QuasarDistortion *)instance;

    switch ((PortIndex)port) {
        case INPUT_L:
            dl->input_l = (const float *)data;
            break;
        case INPUT_R:
            dl->input_r = (const float *)data;
            break;
        case OUTPUT_L:
            dl->output_l = (float *)data;
            break;
        case OUTPUT_R:
            dl->output_r = (float *)data;
            break;
        case GAIN_IN:
            dl->gain_in = (const float *)data;
            break;
        case BITS:
            dl->bits = (const float *)data;
            break;
        case OVERFLOW:
            dl->overflow = (const float *)data;
            break;
        case DRY_WET:
            dl->dry_wet = (const float *)data;
    }
}

static void activate(LV2_Handle instance) {}

static void run(LV2_Handle instance, uint32_t n_samples) {
    QuasarDistortion *dl = (QuasarDistortion *)instance;

    const float *const input_l = dl->input_l;
    const float *const input_r = dl->input_r;
    float *const output_l = dl->output_l;
    float *const output_r = dl->output_r;
    const float gain_in = *(dl->gain_in);
    const uint32_t bits = *(dl->bits);
    const float overflow = *(dl->overflow);
    const float dry_wet = *(dl->dry_wet);

    float l, r;

    const int scale_factor = (1 << bits) / 2 - 1;

    for (uint32_t pos = 0; pos < n_samples; pos++) {
        // Apply input gain
        l = input_l[pos] * gain_in;
        r = input_r[pos] * gain_in;

        // Convert to integer
        int32_t l_int = l * scale_factor;
        int32_t r_int = r * scale_factor;

        // Slam!
        l_int *= overflow;
        r_int *= overflow;

        // Handle overflow
        if (abs(l_int) > scale_factor) l_int = -l_int;
        if (abs(r_int) > scale_factor) r_int = -r_int;

        // And back
        l = l_int / overflow / scale_factor;
        r = r_int / overflow / scale_factor;

        // Apply dry/wet
        output_l[pos] = dry_wet * l + (1.0 - dry_wet) * input_l[pos];
        output_r[pos] = dry_wet * r + (1.0 - dry_wet) * input_r[pos];
    }
}

static void deactivate(LV2_Handle instance) {}

static void cleanup(LV2_Handle instance) { free(instance); }

static const void *extension_data(const char *uri) { return NULL; }

static const LV2_Descriptor descriptor = {DL_URI, instantiate, connect_port, activate,
                                          run,    deactivate,  cleanup,      extension_data};

LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t index) { return index == 0 ? &descriptor : NULL; }
