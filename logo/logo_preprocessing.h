#ifndef LOGO_PREPROCESSING_H
#define LOGO_PREPROCESSING_H
#include <logo/logo_structs.h>

void logo_apply_window(const logo_window& w, logo_vector& vector);
void logo_filter_set_a(logo_filter& fil, const logo_vector& a);
void logo_filter_set_b(logo_filter& fil, const logo_vector& b);
void logo_print_filter(const logo_filter& fil);
void logo_filter_filt(const logo_filter& fil, logo_vector& data);
void logo_print_buffer_slicer_info(const logo_slicer& slicer);
void logo_set_offset_filter_parameter(logo_slicer& slicer, const logo_real& alpha);
void logo_set_preemphasis_filter_parameter(logo_slicer& slicer, const logo_real& alpha);
void logo_apply_buffer_slicer(logo_slicer& slicer, const  logo_vector& data, logo_matrix& frames);

#endif // PREPROCESSING_UTILITY_H
