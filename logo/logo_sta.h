#ifndef LOGO_STA_H
#define LOGO_STA_H

#include <iostream>
#include <algorithm>
#include <cmath>
#include <fftw3.h>
#include <logo/logo_structs.h>
#include <logo/logo_util.h>

#define sign(_a) (_a > 0 ? 1 : 0 )
#define db_spl(_a) 10*log10(_a)
#define db_20(_a)  20*log10(_a)
#define squared(_a) _a*_a



void logo_set_vad_tolerance(logo_vad& vad,  const logo_real& tolerance);
void logo_set_vad_threshold(logo_vad& vad,  const logo_real& threshold);
void logo_fft_clear(logo_fft& fft);
void logo_fft_norm(logo_fft& fft, const logo_vector& frame, logo_vector& result);
void logo_fft_inverse(logo_fft& fft,  logo_complex* frame, logo_vector& result);
void logo_fft_real(logo_fft& fft, const logo_vector& frame, logo_vector& result);
void logo_fft_psd(logo_freq_analyzer& freq, const logo_vector& frame, logo_vector& result, const bool& log);
void logo_fft_lpc(logo_freq_analyzer& freq, const logo_vector& frame, logo_vector& result, const bool& log);
void logo_fft_ceps(logo_freq_analyzer& freq, const logo_vector& frame, logo_vector& result, const bool& log);
void logo_fft_filterbank(logo_freq_analyzer& freq, const logo_vector& frame, logo_vector& result, const bool& log);
void logo_zero_crossing_rate(const logo_vector& frame, logo_real& result);
void logo_energy_lin(const logo_vector& frame, logo_real& result);
void logo_energy_db_spl(const logo_vector& frame, logo_real& result);
void logo_pitch_tracker_amdf(logo_pitch& tracker, const logo_vector& frame, logo_real& result);
void logo_pitch_tracker_acf(logo_pitch& tracker, const logo_vector& frame, logo_real& result);
void logo_pitch_tracker_ceps(logo_fft& fft, logo_pitch& tracker, const logo_vector& frame, logo_real& result);

void logo_frames_fft_psd(logo_freq_analyzer& freq, const logo_matrix& frames, logo_matrix& result, const bool& log = true);
void logo_frames_fft_lpc(logo_freq_analyzer& freq, const logo_matrix& frames, logo_matrix& result, const bool& log = true);
void logo_frames_fft_ceps(logo_freq_analyzer& freq, const logo_matrix& frames, logo_matrix& result, const bool& log = true);
void logo_frames_fft_filterbank(logo_freq_analyzer& freq, const logo_matrix& frames, logo_matrix& result, const bool& log = true);
void logo_frames_energy(const logo_matrix& frames, logo_vector& result);
void logo_frames_energy_db_spl(const logo_matrix& frames, logo_vector& result);
void logo_frames_zero_crossing_rate(const logo_matrix& frames, logo_vector& result);
void logo_frames_pitch_tracker_amdf( logo_pitch& tracker, const logo_matrix& frames, logo_vector& result);
void logo_frames_pitch_tracker_acf( logo_pitch& tracker, const logo_matrix& frames, logo_vector& result);
void logo_frames_pitch_tracker_cepstrum(logo_fft& fft, logo_pitch& tracker, const logo_matrix& frames, logo_vector& result);



#endif // STA_UTILITY_H
