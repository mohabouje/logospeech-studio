#include "logo_preprocessing.h"
#include <cmath>





void logo_apply_window(const logo_window& w, logo_vector& vector) {
    for (int i=0;i<w.size();i++)
        vector[i] *= w[i];
}






void logo_filter_set_a(logo_filter& fil, const logo_vector& a) {
    fil.a = a;
}

void logo_filter_set_b(logo_filter& fil, const logo_vector& b) {
    fil.b = b;
}

void logo_print_filter(const logo_filter& fil) {
    for (int i=0;i<fil.order;i++)
        std::cout << "a[" << i << "] = " << fil.a[i] << "\t"  << "b[" << i << "] = " << fil.b[i] << endl;
}

void logo_filter_filt(const logo_filter& fil, logo_vector& data) {
    const int size = data.size();
    const int order = fil.order;
    logo_vector x = new_vec(size);
    logo_vector y = new_vec(size);

    for (int j = 0; j < size; j++) {
       /* new input */
       x[0] = data[j];
       y[0] = fil.b[0] * x[0];
       for (int l = 1; l < order; l++) {
         y[0] += fil.b[l] * x[l];
         y[0] -= fil.a[l] * y[l];
       }
       /* new output */
       data[j] = y[0];
       /* store for next sample */
       for (int l = order - 1; l > 0; l--) {
         x[l] = x[l - 1];
         y[l] = y[l - 1];
       }
    }
    x.clear();
    y.clear();
}


void logo_print_buffer_slicer_info(const logo_slicer& slicer) {
    std::cout << "Samplerate = " << slicer.samplerate << " Hz." << std::endl;
    std::cout << "Buffer time = " << slicer.buffer_time << " s -> " <<  slicer.buffer_samples << " samples." << std::endl;
    std::cout << "Hope time = " << slicer.hope_time << " s -> " <<  slicer.hope_samples << " samples." << std::endl;
    std::cout << "Window time = " << slicer.window_time << " s -> " <<  slicer.window_samples << " samples." << std::endl;
    std::cout << "Frame count = " << slicer.frame_count << " frames." << std::endl;
    std::cout << "Ring buffer size = " << slicer.ring_buffer_samples << std::endl;
}


void logo_refresh_ring_buffer(logo_slicer& slicer, const logo_vector& data) {
    std::copy(slicer.temp_ring_buffer.begin() + data.size(), slicer.temp_ring_buffer.end(), slicer.temp_ring_buffer.begin());
    std::copy(data.begin(),data.end(), slicer.temp_ring_buffer.begin()  + slicer.ring_buffer_samples - data.size());
    std::copy(slicer.temp_ring_buffer.begin(), slicer.temp_ring_buffer.end(), slicer.ring_buffer.begin());
}

void logo_apply_buffer_slicer(logo_slicer& slicer, const  logo_vector& data, logo_matrix& frames) {
    if (data.isEmpty() ||
            frames.isEmpty() ||
            frames.size() != slicer.frame_count ||
            frames[0].size() != slicer.window_samples ||
            data.size() !=  slicer.buffer_samples)
        return;
    logo_refresh_ring_buffer(slicer, data);
    logo_filter_filt(slicer.offset,slicer.ring_buffer);
    logo_filter_filt(slicer.pre_emphasis,slicer.ring_buffer);
    for (int i=0; i<frames.size();i++){
        std::copy(slicer.ring_buffer.begin()+i*slicer.hope_samples, slicer.ring_buffer.begin()+i*slicer.hope_samples+slicer.window_samples, frames[i].begin());
        logo_apply_window(slicer.w,frames[i]);
    }
}


void logo_set_offset_filter_parameter(logo_slicer& slicer, const logo_real& alpha) {
    slicer.offset.a[0] = 1.0;
    slicer.offset.a[1] = alpha;
    slicer.offset.b[0] = 1.0;
    slicer.offset.b[1] = -1.0;

}


void logo_set_preemphasis_filter_parameter(logo_slicer& slicer, const logo_real& alpha) {
    slicer.pre_emphasis.a[0] = 1.0;
    slicer.pre_emphasis.a[1] = 0;
    slicer.pre_emphasis.b[0] = 1.0;
    slicer.pre_emphasis.b[1] = alpha;

}
