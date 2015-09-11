#include "logo_structs.h"

#define SQR(_a)   _a*_a
#define PI         M_PI
const static logo_real TWO_PI = 2*PI;


void logo_vec_concatenate(logo_vector& first, const logo_vector& second) {
    if (second.size() > first.size())
        return;
    std::copy(first.begin()+second.size(), first.end(), first.begin());
    std::copy(second.begin(), second.end(), first.end()-second.size());
}

void logo_plot_concatenate(logo_plot& data, const logo_vector& add) {
    logo_vec_concatenate(data.y, add);
    logo_pair minmax;
    logo_minmax_vec(data.y, minmax);
    data.min = minmax.first;
    data.max = minmax.second;
}


void logo_init_x_axis(logo_plot& vector) {
    const int size = vector.length;
    const int sampleRate = vector.sample_rate;
    const int duration =  vector.duration;
    switch (vector.domain) {
    case time_domain:
            for (int i=0;i<size;i++)
                vector.x[i]=1.0/size*i*duration;
            break;
    case frequency_domain:
            for (int i=0;i<size;i++)
                vector.x[i]=0.5*i/size*sampleRate;
            break;
    case quefrequency_domain:
            for (int i=0;i<size;i++)
                vector.x[i]=0.5/size*i*duration;
            break;
    default:
            break;
    }

}


void logo_print_vecplot(const logo_plot& vector) {
    for (int i=0;i<vector.length;i++)
        std::cout << vector.x[i] << "\t" << vector.y[i] << std::endl;
}





void logo_normalize_vec(logo_vector& vector, const logo_real& factor) {
    logo_real max = factor;
    if (!factor)
        logo_max_vec(vector, max);
    std::for_each(vector.begin(),vector.end(),[max](logo_real& value){ value /= max;});
}

void logo_minmax_vec(const logo_vector& vector, logo_pair& minmax){
    auto temp = std::minmax_element(vector.begin(), vector.end());
    minmax.first = *temp.first;
    minmax.second = *temp.second;
}

void logo_max_vec(const logo_vector& vector, logo_real& max) {
    max = *std::max_element(vector.begin(),vector.end());
}
void logo_min_vec(const logo_vector& vector, logo_real& min) {
    min = *std::min_element(vector.begin(),vector.end());
}
void logo_print_vec(const logo_vector& vector) {
    int i = 0;
    foreach (const logo_real& value, vector)
        std::cout << i++ << "\t" << value << std::endl;
}
void logo_export_vec(const logo_vector& vector, const QString& fileName) {
    std::ofstream out(fileName.toStdString(), std::ofstream::out);
    foreach (const logo_real& value, vector)
        out << value << std::endl;
    out.close();
}



void logo_free_vec(logo_vector* vector) {
    delete vector;
}


void logo_concatenate_matrix(logo_matrix& first, const logo_matrix& second) {
    if (first.isEmpty() || second.isEmpty() || first.size() < second.size() || first[0].size() != second[0].size())
        return;
    std::copy(first.begin()+second.size(), first.end(), first.begin());
    std::copy(second.begin(), second.end(), first.end()-second.size());
}

void logo_normalize_mat(logo_matrix& matrix, const logo_real& factor) {
    logo_real max = factor;
    if (!factor)
        logo_max_mat(matrix, max);
    std::for_each(matrix.begin(),matrix.end(),[max](logo_vector& vector){
        std::for_each(vector.begin(),vector.end(),[max](logo_real& value){ value /= max;});
    });
}

void logo_minmax_mat(const logo_matrix& matrix, logo_pair& minmax){
    double min = 0.0, max = 0.0;
    foreach (const logo_vector& vector, matrix) {
        auto temp = std::minmax_element(vector.begin(), vector.end());
        max = ( *temp.first > max ) ? *temp.first : max;
        min = ( *temp.second < min) ? *temp.second : min;
    }
    minmax.first = min;
    minmax.second = max;
}

void logo_max_mat(const logo_matrix& matrix, logo_real& max) {
    logo_real temp;
    foreach (const logo_vector& vector, matrix) {
        temp = *std::max_element(vector.begin(),vector.end());
        max  = ( temp > max ) ? temp : max;
    }
}
void logo_min_mat(const logo_matrix& matrix, logo_real& min) {
    logo_real temp;
    foreach (const logo_vector& vector, matrix) {
        temp = *std::min_element(vector.begin(),vector.end());
        min  = ( temp < min ) ? temp : min;
    }
}




void logo_print_mat(const logo_matrix& matrix) {
    foreach (const logo_vector& vector, matrix) {
        foreach (const logo_real& value, vector)
           std::cout << value << " ";
        std::cout << std::endl;
    }
}
void logo_export_mat(const logo_matrix& matrix, const QString& fileName) {
    std::ofstream out(fileName.toStdString(), std::ofstream::out);
    foreach (const logo_vector& vector, matrix) {
        foreach (const logo_real& value, vector)
            out << value << " ";
        out << std::endl;
    }
    out.close();
}

void logo_free_mat(logo_matrix* matrix) {
    delete matrix;
}


logo_data new_logo_data(const int& tmax, const logo_slicer& slicer,  const logo_freq_analyzer& freq) {
    logo_data data;
    data.fmax = freq.sample_rate/2;
    data.tmax = tmax;
    data.fft_samples = freq.fft_samples;
    data.filterbank_order = freq.fbank_order;
    data.frames = tmax / slicer.hope_time;
    data.time_plot = new_vecplot(data.frames,0,freq.sample_rate,tmax);
    data.pitch_plot = new_vecplot(data.frames,0,freq.sample_rate,tmax);
    data.spectrogram_plot = new_mat(data.frames, freq.fft_samples,-100);
    data.filterbank_plot = new_mat(data.frames, freq.fbank_order);
    data.temp_frames = new_mat(slicer.frame_count, slicer.window_samples);
    data.temp_result = new_vec(slicer.frame_count);
    data.temp_spectrum = new_mat(slicer.frame_count, data.fft_samples);
    data.temp_fb_spectrum = new_mat(slicer.frame_count, freq.fbank_order);
    return data;
}


logo_plot new_vecplot(const int& length,
                     const int& value,
                     const int& sampleRate,
                     const int& duration,
                     const axis_domain& domain) {
        logo_plot vector;
        vector.length = length;
        vector.sample_rate = sampleRate;
        vector.duration = duration;
        vector.domain = domain;
        vector.x = new_vec(length, value);
        vector.y = new_vec(length, value);
        vector.min = 0;
        vector.max = 0;
        logo_init_x_axis(vector);
        return vector;
}

logo_vector new_vec(const int& length, const int& value) {
    return logo_vector(length,value);
}


logo_matrix new_mat(const int& rowCount, const int& columnCount, const int& value) {
    return logo_matrix(rowCount,logo_vector(columnCount,value));
}

logo_window new_window(const int& length, const window_type& type) {
    logo_window w = new_vec(length);
    const int size =  w.length();
    switch(type) {
        case window_rectangle:
          for (int i=0;i<size;i++)
            w[i] = 0.5;
          break;
        case window_hamming || window_default:
          for (int i=0;i<size;i++)
            w[i] = 0.54 - 0.46 * cos(TWO_PI * i / (size));
          break;
        case window_hanning:
          for (int i=0;i<size;i++)
            w[i] = 0.5 - (0.5 * cos(TWO_PI * i / (size)));
          break;
        case window_hanningz:
          for (int i=0;i<size;i++)
            w[i] = 0.5 * (1.0 - cos(TWO_PI * i / (size)));
          break;
        case window_blackman:
          for (int i=0;i<size;i++)
            w[i] = 0.42
              - 0.50 * cos(    TWO_PI*i/(size-1.0))
              + 0.08 * cos(2.0*TWO_PI*i/(size-1.0));
          break;
        case window_blackman_harris:
          for (int i=0;i<size;i++)
            w[i] = 0.35875
              - 0.48829 * cos(    TWO_PI*i/(size-1.0))
              + 0.14128 * cos(2.0*TWO_PI*i/(size-1.0))
              - 0.01168 * cos(3.0*TWO_PI*i/(size-1.0));
          break;
        case window_gaussian: {
            logo_real a, b, c = 0.5;
            for (int n = 0; n < size; n++) {
              a = (n-c*(size-1))/(SQR(c)*(size-1));
              b = -c*SQR(a);
              w[n] = exp(b);
            }
          }
          break;
        case window_welch:
          for (int i=0;i<size;i++)
            w[i] = 1.0 - SQR((2.*i-size)/(size+1.0));
          break;
        case window_parzen:
          for (int i=0;i<size;i++)
            w[i] = 1.0 - abs((2.f*i-size)/(size+1.0f));
          break;
        default:
          break;
    }
    return w;
}

logo_filter new_filter(const int& order) {
    logo_filter fil;
    fil.order = order;
    fil.a = new_vec(order);
    fil.b = new_vec(order);
    return fil;
}

logo_slicer new_buffer_slicer(const logo_real& buffer_time, const logo_real& hope_time, const logo_real& window_time, const int& samplerate, const window_type& w_type, const int& filter_order) {
    logo_slicer slicer;
    slicer.samplerate = samplerate;
    slicer.buffer_time = buffer_time;
    slicer.buffer_samples = buffer_time * samplerate;
    slicer.hope_time = hope_time;
    slicer.hope_samples = hope_time * samplerate;
    slicer.window_time = window_time;
    slicer.window_samples = window_time * samplerate;
    slicer.frame_count = buffer_time/hope_time;
    slicer.ring_buffer_samples = (slicer.frame_count-1)*slicer.hope_samples + slicer.window_samples;
    slicer.ring_buffer = new_vec(slicer.ring_buffer_samples);
    slicer.temp_ring_buffer = new_vec(slicer.ring_buffer_samples);
    slicer.w_type = w_type;
    slicer.w = new_window(slicer.window_samples,w_type);
    slicer.pre_emphasis = new_filter(filter_order);
    slicer.offset = new_filter(filter_order);
    slicer.filter_order = filter_order;
    return slicer;
}


logo_real logo_linToMelScale(logo_real value) {
    return 1127*log(1+value/700);
}
logo_real logo_melToLinScale(logo_real value) {
    return 700*(exp(value/1127)-1);
}

void logo_melFreqBoundary(logo_vector& f, const int sampleRate) {
    const int& melSampleRate = logo_linToMelScale(sampleRate/2);
    const int& filterCount = f.size(); //Add Limit, 0 and Fs/2
    double mel = 0;
    for (int i=0; i<filterCount;i++) {
        mel = i*melSampleRate/(filterCount-1);
        f[i] = logo_melToLinScale(mel);
    }
}
int logo_freqToSample(float f, int sampleRate,  int windowSamples){
    return floor(2 * f * (windowSamples) / sampleRate);
}





void logo_set_freq_analyzer_triangle_bands(logo_freq_analyzer& freq) {
    const int rowCount = freq.fbank_order;
    const int columnCount = freq.fft_samples;
    const int samplerate = freq.sample_rate;

    logo_vector f = new_vec(rowCount + 2);
    logo_vector lower_freqs = new_vec(rowCount);
    logo_vector upper_freqs = new_vec(rowCount);
    logo_vector center_freqs = new_vec(rowCount);

    logo_melFreqBoundary(f, samplerate);
    for (int fn = 0; fn < rowCount; fn++) {
      lower_freqs[fn] = logo_freqToSample(f[fn], samplerate, columnCount);
      center_freqs[fn] = logo_freqToSample(f[fn + 1], samplerate, columnCount);
      upper_freqs[fn] = logo_freqToSample(f[fn + 2], samplerate, columnCount);
    }

    for (int i=0;i<rowCount;i++) {
        for (int j=0;j<columnCount;j++) {
            if (lower_freqs[i] <= j && j <= center_freqs[i])
                freq.filter_bank[i][j] = (j - lower_freqs[i])/(center_freqs[i]-lower_freqs[i]);
            else if (center_freqs[i] <= j && j <= upper_freqs[i])
                freq.filter_bank[i][j] = (upper_freqs[i]-j)/(upper_freqs[i]-center_freqs[i]);
        }
    }

}


logo_fft new_fft(const int& frame_samples, const int& fft_samples) {
    logo_fft fft;
    fft.fft_samples = fft_samples;
    fft.frame_samples = frame_samples;
    fft.input = new_vec(frame_samples);
    fft.output = new_vec(frame_samples);
    fft.complexFFT =  (logo_complex*)fftw_malloc(sizeof(logo_complex)*fft_samples);
    fft.directFFT = fftw_plan_dft_r2c_1d(frame_samples, fft.input.data(), fft.complexFFT, FFTW_ESTIMATE);
    fft.inverseFFT = fftw_plan_dft_c2r_1d(frame_samples, fft.complexFFT, fft.output.data(), FFTW_ESTIMATE);
    return fft;
}

logo_freq_analyzer new_freq_analyzer(const int& sample_rate, const int& frame_samples, const int& lpc_order, const int& ceps_order, const int& fbank_order) {
    logo_freq_analyzer freq;
    freq.sample_rate = sample_rate;
    freq.frame_samples = frame_samples;
    freq.fft_samples = frame_samples/2 + 1;
    freq.lpc_order = lpc_order;
    freq.ceps_order = ceps_order;
    freq.fbank_order = fbank_order;
    freq.filter_bank = new_mat(fbank_order, freq.fft_samples);
    logo_set_freq_analyzer_triangle_bands(freq);
    freq.fft = new_fft(frame_samples, freq.fft_samples);
    return freq;
}


logo_pitch new_pitch_tracker(const pitch_algorithm& algo, const int& frame_samples, const int& sample_rate, const int& pitch_min, const int& pitch_max){
    logo_pitch tracker;
    tracker.vad = new_logo_vad(0.0,0.0);
    tracker.type = algo;
    tracker.sample_rate = sample_rate;
    tracker.frame_samples = frame_samples;
    tracker.pitch_min = sample_rate / pitch_max;
    tracker.pitch_max = sample_rate / pitch_min;
    tracker.data = new_vec(frame_samples);
    return tracker;
}


logo_vad new_logo_vad(const logo_real& tolerance, const logo_real& levelthreshold) {
    logo_vad vad;
    vad.tolerance = tolerance;
    vad.levelthreshold = levelthreshold;
    return vad;
}



