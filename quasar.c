
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
    if (dl == NULL) {
        return NULL;
    }
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

static inline float sanitize_denormal(float value) {
    if (!isnormal(value)) {
        value = 0.f;
    }
    return value;
}

static inline float handle_sample(float raw_value, const float gain_in, const int32_t scale_factor, const float overflow,
                                  const float dry_wet) {
    // Apply input gain
    double value = (double)sanitize_denormal(raw_value) * (double)gain_in;

    // Convert to integer with clamping to avoid UB on cast
    double scaled_value = value * (double)scale_factor;
    if (scaled_value > 1e18) scaled_value = 1e18;
    if (scaled_value < -1e18) scaled_value = -1e18;
    int64_t value_int = (int64_t)scaled_value;

    // Slam!
    double slammed_value = (double)value_int * (double)overflow;
    if (slammed_value > 1e18) slammed_value = 1e18;
    if (slammed_value < -1e18) slammed_value = -1e18;
    value_int = (int64_t)slammed_value;

    // Handle overflow (distortion folding)
    if (value_int > (int64_t)scale_factor || value_int < -(int64_t)scale_factor) {
        value_int = -value_int;
    }

    // And back
    float out;
    if (overflow != 0.0f && scale_factor != 0) {
        out = (float)((double)value_int / (double)overflow / (double)scale_factor);
    } else {
        out = 0.0f;
    }

    // Apply dry/wet
    float output_value = dry_wet * out + (1.0f - dry_wet) * raw_value;
    return sanitize_denormal(output_value);
}

static void run(LV2_Handle instance, uint32_t n_samples) {
    QuasarDistortion *dl = (QuasarDistortion *)instance;

    const float *const input_l = dl->input_l;
    const float *const input_r = dl->input_r;
    float *const output_l = dl->output_l;
    float *const output_r = dl->output_r;
    float gain_in = *(dl->gain_in);
    if (gain_in < 0.0f) gain_in = 0.0f;
    if (gain_in > 100.0f) gain_in = 100.0f;

    uint32_t bits = *(dl->bits);
    if (bits < 2) bits = 2;
    if (bits > 30) bits = 30;

    float overflow = *(dl->overflow);
    if (overflow < 0.1f) overflow = 0.1f;
    if (overflow > 100.0f) overflow = 100.0f;

    float dry_wet = *(dl->dry_wet);
    if (dry_wet < 0.0f) dry_wet = 0.0f;
    if (dry_wet > 1.0f) dry_wet = 1.0f;

    const int32_t scale_factor = (int32_t)((1U << bits) / 2 - 1);

    for (uint32_t pos = 0; pos < n_samples; pos++) {
        output_l[pos] = handle_sample(input_l[pos], gain_in, scale_factor, overflow, dry_wet);
        output_r[pos] = handle_sample(input_r[pos], gain_in, scale_factor, overflow, dry_wet);
    }
}

static void deactivate(LV2_Handle instance) {}

static void cleanup(LV2_Handle instance) { free(instance); }

static const void *extension_data(const char *uri) { return NULL; }

static const LV2_Descriptor descriptor = {DL_URI, instantiate, connect_port, activate,
                                          run,    deactivate,  cleanup,      extension_data};

LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t index) { return index == 0 ? &descriptor : NULL; }
