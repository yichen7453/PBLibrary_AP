#pragma once
#include "stdafx.h"
#include "pb_image.h"

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

typedef enum enroll_state {
	_enroll_timeout_,
	_enroll_duplicate_fp_,
	_enroll_low_quality_,
	_enroll_few_area_,
	_enroll_template_full_,
	_enroll_run_error_,
	_enroll_successful_,
	_enroll_unknown_error_,
} enroll_state;

typedef struct enroll_info {
	/* input: set timeout seconds as getting finger image*/
	int         timeout_sec;
	/* output*/
	enroll_state es;
	int         num_accepted;
	int         enrolling_finger_idx;
	uint8_t     quality;
	uint16_t    area;
	uint8_t     coverage;
} enroll_info;

void setSensorType(int width, int height);

int lib_init(void);
int lib_deinit(void);

int file_check_available_index(int* file_index);

int enroll_setup(void);
int enroll_finger(pb_image_t* image, uint8_t* coverage_area);
void enroll_finish(int* template_size);

int verify_setup(void);
int verify_finger(pb_image_t* image);
int verify_finish(void);

int set_enroll_count(int count);
int get_enroll_count(void);

int image_quality_chk(const char* filename, uint8_t* p_resQuality, uint16_t* p_resCoverage);

int quality_chk_init(void);
int quality_chk(pb_image_t* image, uint8_t* image_quality, uint16_t* fingerprint_area);
int quality_chk_deinit();

char* get_output_file_name();
int set_archive_path(const char* database_path, const char* filename);
