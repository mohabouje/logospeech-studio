#include "logo_sta.h"

void logo_fft_clear(logo_fft& fft) {
    std::for_each(fft.input.begin(),fft.input.end(),[](logo_real& value){ value = 0.0;});
    std::for_each(fft.output.begin(),fft.output.end(),[](logo_real& value){ value = 0.0;});
    fftw_free(fft.complexFFT);
}

void logo_fft_norm(logo_fft& fft, const logo_vector& frame, logo_vector& result) {
    if (frame.size() > fft.frame_samples || result.size() != fft.fft_samples)
        return;
    std::copy(frame.begin(), frame.end(), fft.input.begin());
    fftw_execute(fft.directFFT);
    result[0] = fft.complexFFT[0][0];
    result[fft.fft_samples-1] =  fft.complexFFT[fft.fft_samples-1][0];
    for (int i=1;i<fft.fft_samples-1;i++) {
        result[i]  = sqrt( squared(fft.complexFFT[i][0]) + squared(fft.complexFFT[i][1]) );
    }
}

void logo_fft_inverse(logo_fft& , logo_complex* , logo_vector& ) {
    /*std::copy(frame, frame + fft.fft_samples, fft.complexFFT);
    for(int i=0; i<fft.fft_samples;i++)
        std::cout << fft.complexFFT[i][0] << "\t" << fft.complexFFT[i][1] << std::endl;
    fftw_execute(fft.inverseFFT);
    std::copy(fft.output.begin(), fft.output.end(), result.begin());*/
}


void logo_fft_real(logo_fft& fft, const logo_vector& frame, logo_vector& result) {
    if (frame.size() > fft.frame_samples || result.size() != fft.fft_samples)
        return;
    std::copy(frame.begin(), frame.end(), fft.input.begin());
    fftw_execute(fft.directFFT);
    result[0] = fft.complexFFT[0][0];
    result[fft.fft_samples-1] = fft.complexFFT[fft.fft_samples-1][0];
    for (int i=1;i<fft.fft_samples-1;i++) {
        result[i]  = fft.complexFFT[i][0];
    }
}


void logo_fft_psd(logo_freq_analyzer& freq, const logo_vector& frame, logo_vector& result, const bool& log) {
    logo_fft_norm(freq.fft,frame,result);
    if (log) {
        std::for_each(result.begin(),
                      result.end(),
                      [](logo_real& value){value = db_20(value);});
    } else {
        std::for_each(result.begin(),
                      result.end(),
                      [](logo_real& value){value = squared(value);});
    }
}

void logo_fft_ceps(logo_freq_analyzer& freq, const logo_vector& frame, logo_vector& result, const bool& log){
    logo_vector C = new_vec(freq.ceps_order);
    logo_cepsLPC(frame,freq.lpc_order,freq.ceps_order,C);
    logo_vector data = new_vec(freq.fft_samples);
    std::copy(C.begin(), C.end(),data.begin());
    logo_fft_real(freq.fft,data,result);
    for (int i=0;i<freq.fft_samples;i++)
        result[i] =  (log) ? db_20(exp(result[i])) : exp(result[i]);
}
void logo_fft_lpc(logo_freq_analyzer& freq, const logo_vector& frame, logo_vector& result, const bool& log){
    logo_vector A = new_vec(freq.lpc_order+1);
    logo_real G = 0.0;
    logo_lpc(frame,freq.lpc_order,A, G);
    logo_vector data = new_vec(freq.fft_samples);
    std::copy(A.begin(), A.end(),data.begin());
    logo_fft_norm(freq.fft,data,result);
    for (int i=0;i<freq.fft_samples;i++)
        result[i] =  (log) ? db_20(G/result[i]) : squared(G)/squared(result[i]);

}


void logo_fft_filterbank(logo_freq_analyzer& freq,const logo_vector& frame, logo_vector& result, const bool& log) {
    if (result.size() != freq.fbank_order)
        return;
    logo_vector temp = new_vec(freq.fft_samples);
    logo_fft_norm(freq.fft,frame,temp);
    int pos;
    for (int i = 0; i < freq.fbank_order; i++) {
        pos =freq.fbank_order-i-1;
        result[pos] = 0.0;
        for (int j = 0; j < freq.fft_samples; j++) {
           result[pos] +=  freq.filter_bank[i][j]*temp[i];
        }
        if (log)
            result[pos] = db_20(result[pos]);
        else
            result[pos] = squared(result[pos]);
    }
}


void logo_zero_crossing_rate(const logo_vector& frame, logo_real& result) {
    logo_real  zcr = 0.0;
    for (int i=1;i<frame.size();i++) {
        if (sign(frame[i]) != sign(frame[i-1]))
            zcr++;
    }
    result = zcr / (logo_real) frame.size();
}

void logo_energy_lin(const logo_vector& frame, logo_real& result) {
    result = std::accumulate(frame.begin(), frame.end(),0.0,
                            [](const logo_real& prev, const logo_real& value) {
                                return prev + value*value;
                            });
    result /= frame.size();
}

void logo_energy_db_spl(const logo_vector& frame, logo_real& result) {
    logo_energy_lin(frame,result);
    result =  db_spl(result);
}



void logo_check_armonic_frequency(const logo_vector& frame, int& pos) {
    if (pos > frame.length() || pos < 0)
        return;

    int doubleFrequencyPos = ceil(pos/2);
    int mediumFrequencyPos = ceil(pos*2);

    logo_real currentValue = frame[pos];
    logo_real doubleFrequencyValue = (doubleFrequencyPos >0 && doubleFrequencyPos < frame.length()) ? frame[doubleFrequencyPos] : 0;
    logo_real mediumFrequencyValue = (mediumFrequencyPos >0 && mediumFrequencyPos < frame.length()) ? frame[mediumFrequencyPos] : 0;

    pos = ((currentValue - doubleFrequencyValue) / currentValue < 0.3) ? doubleFrequencyPos : pos;
    pos = ((currentValue - mediumFrequencyValue) / currentValue < 0.3) ? mediumFrequencyPos : pos;

}




bool logo_vad_normalized_autocor(const logo_vector& data) {
    logo_real sum_1 = 0.0, sum_2 = 0.0, sum_3 = 0.0;
    for (int i=0; i<data.size();i++) {
        if (i>0) {
            sum_1 += data[i]*data[i-1];
            sum_2 += squared(data[i]);
        }
        sum_3 = squared(data[i]);
    }
    return sum_1/sqrt(sum_2*sum_3)>0.5;
}



void logo_pitch_tracker_amdf(logo_pitch& tracker, const logo_vector& frame, logo_real& result) {
    if  (tracker.type != pitch_algo_amdf ||
        frame.size() != tracker.frame_samples) {
        result = 0;
        return;
    }
    if (!logo_vad_normalized_autocor(frame)) {
        result = 0.0;
        return;
    }
    logo_real max;
    logo_energy_db_spl(frame, max);
    if (max < tracker.vad.levelthreshold) {
        result = 0.0;
        return;
    }
    logo_zero_crossing_rate(frame, max);
    if (max > tracker.vad.tolerance){
        result = 0.0;
        return;
    }
    logo_real sum;
    for (int i=0;i<frame.size();i++) {
        sum = 0.0;
        for (int j=0;j<frame.size()-i;j++)
           sum += qAbs(frame[j] - frame[j+i]);
        tracker.data[i] = sum;
    }
    int posPitch = std::distance(tracker.data.begin(), std::min_element(tracker.data.begin() + tracker.pitch_min, tracker.data.begin() + tracker.pitch_max));
    //logo_check_armonic_frequency(frame,posPitch);
    result =  (posPitch > tracker.pitch_max || posPitch < tracker.pitch_min) ? 0 : ((logo_real)tracker.sample_rate)/posPitch;
}

void logo_pitch_tracker_acf(logo_pitch& tracker, const logo_vector& frame, logo_real& result){
    if  (tracker.type != pitch_algo_autocor ||
        frame.size() != tracker.frame_samples) {
        result = 0.0;
        return;
    }
    if (!logo_vad_normalized_autocor(frame)) {
        result = 0.0;
        return;
    }
    logo_real max;
    logo_energy_db_spl(frame, max);
    if (max < tracker.vad.levelthreshold) {
        result = 0.0;
        return;
    }
    logo_zero_crossing_rate(frame, max);
    if (max > tracker.vad.tolerance){
        result = 0.0;
        return;
    }
    logo_real sum;
    max = pow(10,max/10);
    for (int i=1;i<frame.size();i++) {
        sum = 0.0;
        for (int j=0;j<frame.size()-i;j++)
           sum += logo_real(frame[j]*frame[j+i]);
        tracker.data[i] = sum/max;
    }
    tracker.data[0] = 1;

    int posPitch = std::distance(tracker.data.begin() , std::max_element(tracker.data.begin() + tracker.pitch_min, tracker.data.begin() + tracker.pitch_max));
    //logo_check_armonic_frequency(frame,posPitch);
    result =  (posPitch > tracker.pitch_max || posPitch < tracker.pitch_min) ? 0 : ((logo_real)tracker.sample_rate)/posPitch;
}

void logo_pitch_tracker_ceps(logo_fft& fft, logo_pitch& tracker, const logo_vector& frame, logo_real& result) {
    if  (tracker.type != pitch_algo_cepstrum ||
        (frame.size() != tracker.frame_samples) != fft.frame_samples) {
        result = 0.0;
        return;
    }

    logo_fft_norm(fft,frame, tracker.data);
    std::for_each(tracker.data.begin(), tracker.data.end(), [](logo_real& value) {value = log(value);});
    // inverseFFT=fftw_plan_dft_c2r_1d(samples,fftData,cepstrum,FFTW_ESTIMATE);
    // fftw_execute(inverseFFT);
    int posPitch = std::distance(tracker.data.begin() , std::max_element(tracker.data.begin() + tracker.pitch_min, tracker.data.begin() + tracker.pitch_max));
    result =  (posPitch > tracker.pitch_max || posPitch < tracker.pitch_min) ? 0 : tracker.sample_rate/posPitch;
}



void logo_frames_fft_psd(logo_freq_analyzer& freq, const logo_matrix& frames, logo_matrix& result, const bool& log) {
    if (frames.isEmpty() || result.isEmpty() ||
             frames[0].size() > freq.frame_samples ||
             result.size() != frames.size() ||
             result[0].size() != freq.fft_samples
             )
         return;
     for (int i=0;i<result.size();i++)
         logo_fft_psd(freq, frames[i],result[i],log);

}

void logo_frames_fft_lpc(logo_freq_analyzer& freq, const logo_matrix& frames, logo_matrix& result, const bool& log) {
    if (frames.isEmpty() || result.isEmpty() ||
            frames[0].size() > freq.frame_samples ||
            result.size() != frames.size() ||
            result[0].size() != freq.fft_samples
            )
        return;
     for (int i=0;i<result.size();i++)
         logo_fft_lpc(freq,frames[i],result[i],log);

}
void logo_frames_fft_ceps(logo_freq_analyzer& freq, const logo_matrix& frames, logo_matrix& result, const bool& log) {
    if (frames.isEmpty() || result.isEmpty() ||
            frames[0].size() > freq.frame_samples ||
            result.size() != frames.size() ||
            result[0].size() != freq.fft_samples
            )
        return;
     for (int i=0;i<result.size();i++)
         logo_fft_ceps(freq,frames[i],result[i],log);

}

void logo_frames_fft_filterbank(logo_freq_analyzer& freq, const logo_matrix& frames, logo_matrix& result, const bool& log) {
    if (frames.isEmpty() || result.isEmpty() ||
            frames[0].size() > freq.frame_samples ||
            result.size() != frames.size() ||
            result[0].size() != freq.fbank_order
            )
        return;
     for (int i=0;i<result.size();i++)
         logo_fft_filterbank(freq,frames[i],result[i],log);
}



void logo_frames_energy(const logo_matrix& frames, logo_vector& result) {
    if (result.size() != frames.size() ||
            frames.isEmpty() ||
            result.isEmpty())
        return;
    for (int i=0; i<result.size(); i++)
        logo_energy_lin(frames[i],result[i]);
}

void logo_frames_energy_db_spl(const logo_matrix& frames, logo_vector& result) {
    if (result.size() != frames.size() ||
            frames.isEmpty() ||
            result.isEmpty())
        return;
    for (int i=0; i<result.size(); i++)
        logo_energy_db_spl(frames[i],result[i]);
}

void logo_frames_zero_crossing_rate(const logo_matrix& frames, logo_vector& result) {
    if (result.size() != frames.size() ||
            frames.isEmpty() ||
            result.isEmpty())
        return;
    for (int i=0; i<result.size(); i++)
        logo_zero_crossing_rate(frames[i],result[i]);
}


void logo_frames_pitch_tracker_amdf( logo_pitch& tracker, const logo_matrix& frames, logo_vector& result) {
    if (result.size() != frames.size() ||
            frames.isEmpty() ||
            result.isEmpty())
        return;
    for (int i=0; i<result.size(); i++)
        logo_pitch_tracker_amdf(tracker, frames[i],result[i]);
}

void logo_frames_pitch_tracker_acf( logo_pitch& tracker, const logo_matrix& frames, logo_vector& result) {
    if (result.size() != frames.size() ||
            frames.isEmpty() ||
            result.isEmpty())
        return;
    for (int i=0; i<result.size(); i++)
        logo_pitch_tracker_acf(tracker, frames[i],result[i]);
}

void logo_frames_pitch_tracker_cepstrum(logo_fft& fft, logo_pitch& tracker, const logo_matrix& frames, logo_vector& result) {
    if (result.size() != frames.size() ||
            frames.isEmpty() ||
            result.isEmpty())
        return;
    for (int i=0; i<result.size(); i++)
        logo_pitch_tracker_ceps(fft, tracker, frames[i],result[i]);
}


void logo_set_vad_tolerance(logo_vad& vad,  const logo_real& tolerance) {
    vad.tolerance = tolerance;
}

void logo_set_vad_threshold(logo_vad& vad,  const logo_real& threshold) {
    vad.levelthreshold = threshold;
}

