#ifndef LOGO_DATA_H
#define LOGO_DATA_H

#define _USE_MATH_DEFINES

#include <QVector>
#include <fftw3.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <QString>
using namespace std;

typedef double                          logo_real;
typedef fftw_complex                    logo_complex;
typedef QVector<logo_real>                   logo_vector;
typedef QVector< QVector<logo_real> >        logo_matrix;

typedef struct {
      logo_real first;
      logo_real second;
} logo_pair;

typedef enum
{
    time_domain,
    frequency_domain,
    quefrequency_domain,
    default_domain = time_domain,

} axis_domain;

typedef struct {
    int length;
    int sample_rate;
    int duration;
    logo_real min;
    logo_real max;
    axis_domain domain;
    logo_vector x;
    logo_vector y;
} logo_plot;


typedef struct {
    int tmax;
    int fmax;
    int frames;
    int fft_samples;
    int filterbank_order;
    logo_plot time_plot;
    logo_plot pitch_plot;
    logo_matrix    spectrogram_plot;
    logo_matrix    filterbank_plot;
    logo_vector temp_result;
    logo_matrix temp_frames;
    logo_matrix temp_spectrum;
    logo_matrix temp_fb_spectrum;
} logo_data;


typedef struct {
  int frame_samples;
  int fft_samples;
  logo_vector input;
  logo_vector output;
  logo_complex *complexFFT;
  fftw_plan directFFT;
  fftw_plan inverseFFT;
} logo_fft;

typedef struct {
    int sample_rate;
    int frame_samples;
    int fft_samples;
    int lpc_order;
    int ceps_order;
    int fbank_order;
    logo_matrix filter_bank;
    logo_fft fft;

} logo_freq_analyzer;


typedef enum {
    pitch_algo_autocor,
    pitch_algo_amdf,
    pitch_algo_cepstrum,
    pitch_algo_default = pitch_algo_amdf,
} pitch_algorithm;


typedef struct {
    logo_real tolerance;
    logo_real levelthreshold;
} logo_vad;

typedef struct {
    pitch_algorithm type;
    logo_vad vad;
    int sample_rate;
    int frame_samples;
    int pitch_min;
    int pitch_max;
    logo_vector data;
} logo_pitch;

typedef logo_vector logo_window;

typedef enum
{
  window_rectangle,
  window_hamming,
  window_hanning,
  window_hanningz,
  window_blackman,
  window_blackman_harris,
  window_gaussian,
  window_welch,
  window_parzen,
  window_default = window_hamming,
} window_type;

typedef struct {
    int order;
    logo_vector a;
    logo_vector b;
} logo_filter;

typedef struct {
    int samplerate;
    int frame_count;
    logo_real hope_time;
    int hope_samples;
    logo_real window_time;
    int window_samples;
    logo_real buffer_time;
    int buffer_samples;
    int ring_buffer_samples;
    logo_vector ring_buffer;
    logo_vector temp_ring_buffer;
    logo_window w;
    window_type w_type;
    logo_filter pre_emphasis;
    logo_filter offset;
    int filter_order;
} logo_slicer;





logo_vad new_logo_vad(const logo_real& tolerance, const logo_real& levelthreshold);
logo_data new_logo_data(const int& tmax, const logo_slicer& slicer,  const logo_freq_analyzer& freq);
logo_plot new_vecplot(const int& length, const int& value = 0, const int& sampleRate = 80000, const int& duration = 10, const axis_domain& domain = default_domain);
logo_vector new_vec(const int& length, const int& value = 0);
logo_matrix new_mat(const int& rowCount, const int& columnCount, const int& value = 0);
logo_filter new_filter(const int& order);
logo_window new_window(const int& length, const window_type& type = window_default);
logo_slicer new_buffer_slicer(const logo_real& buffer_time, const logo_real& hope_time, const logo_real& window_time, const int& samplerate, const window_type& w_type = window_default, const int& filter_order = 2);
logo_fft new_fft(const int& frame_samples, const int& fft_samples);
logo_freq_analyzer new_freq_analyzer(const int& sample_rate, const int& frame_samples, const int& lpc_order, const int& ceps_order, const int& fbank_order);
logo_pitch new_pitch_tracker(const pitch_algorithm& algo, const int& frame_samples, const int& sample_rate, const int& pitch_min = 100, const int& pitch_max = 500);


void logo_plot_concatenate(logo_plot& data, const logo_vector& add);


void logo_init_x_axis(logo_plot& vector);
void logo_print_vecplot(const logo_plot& vector);


void logo_normalize_vec(logo_vector& vector, const logo_real& factor = 0);
void logo_minmax_vec(const logo_vector& vector, logo_pair& minmax);
void logo_max_vec(const logo_vector& vector, logo_real& max);
void logo_min_vec(const logo_vector& vector, logo_real& min);
void logo_print_vec(const logo_vector& vector);
void logo_export_vec(const logo_vector& vector, const QString& fileName);
void logo_free_vec(logo_vector* vector);
void logo_vec_concatenate(logo_vector& first, const logo_vector& second);


void logo_normalize_mat(logo_matrix& matrix, const logo_real& factor = 0);
void logo_minmax_mat(const logo_matrix& matrix, logo_pair& minmax);
void logo_max_mat(const logo_matrix& matrix, logo_real& max);
void logo_min_mat(const logo_matrix& matrix, logo_real& min);
void logo_print_mat(const logo_matrix& matrix);
void logo_export_mat(const logo_matrix& matrix, const QString& fileName);
void logo_free_mat(logo_matrix* matrix);
void logo_concatenate_matrix(logo_matrix& first, const logo_matrix& second);

#endif // DATAUTILITY_H
