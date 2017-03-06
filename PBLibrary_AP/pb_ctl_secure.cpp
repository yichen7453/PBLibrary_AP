#include "stdafx.h"

#include "enroll_verify.h"

#include "pb_template.h"
#include "pb_image.h"
#include "pb_user.h"
#include "pb_finger.h"
#include "pb_quality.h"

#include "pb_returncodes.h"
#include "pb_algorithm_spectral.h"
#include "pb_algorithm_hybrid.h"
#include "pb_algorithm_minutiae.h"
#include "pb_multitemplate_enrollment_algorithm.h"
#include "pb_multitemplate.h"
#include "pb_slice_renderer.h"
#include "pb_segmented_slicemem.h"
#include "pb_synthetic_image.h"
#include "pb_preprocessor.h"

#include "pb_quality_pb.h"
#include "pb_qualityI.h"
#include "pb_extractor_embedded.h"
#include "pb_verifier_ehm.h"
#include "pb_verifier_isocc.h"
#include "pb_verifier_spectral.h"
#include "pbpng.h"
#include "pbbmp.h"

#include "pb_ctl_secure.h"

#include <iostream>

static int          g_finger_idx = -1;
static const char*  g_finger_name = NULL;
static const char*	g_finger_database_path = NULL;
static const char*  g_finger_template_update = NULL;
static uint8_t*		gp_img_buf = 0;
static int          g_last_match_index = -1;
int					g_match_idx = -1;
char				outputfilename[100];

static uint16_t		g_sensor_width = 0;
static uint16_t		g_sensor_height = 0;
static uint32_t     g_sensor_img_buffer = 0;

static uint32_t     garea_threshold = 20;
pb_session_t*       g_psession = 0;
static int          g_decision = PB_DECISION_NON_MATCH;
pb_template_t*      gp_dyn_updated = 0;
pb_template_t*      gp_verification_template = 0;
pb_multitemplate_enroll_t*  gp_mte = 0;
pb_template_t**     database_templates_array = 0;;

#   define STORE_FINGERS 100
#   if !defined(STORE_SIZE)
#     if !defined(STM32EVAL)
#       define STORE_SIZE 32768 /* big enough for most */
#     else 
#       define STORE_SIZE 8192  /* default to smaller on target */
#     endif
#   endif

struct {
	int   failed_try;
	int   do_load;
	int   do_save;
	int   do_verify;
	int   dpi;
	int   enr_samples;
	int   ver_samples;
	int   num_ver_loop;
	int   num_fingers;
	pb_far_t ver_far;
	int   mpu;
	int   quick_ver_limit;
	int   enr_ver_limit;
	int   dyn_update;
	int   ver_score;
	int   mt_size;
	int   mt_size_bytes;
	int   lock_update;
	int   straight;
	int   crop_cols;
	int   crop_rows;
	int   enr_num_imgs;
	int   ver_num_imgs;
	const pb_preprocessorI* preproc_filter;
	pb_image_t* ui_feedback_bkg; /* Depricated option */
} Opt = {
	10, 0, 0, 1, 508, 0, 0, 1, 1, PB_FAR_50000, 1, 0, 0, 1, 1, 0, STORE_SIZE-8, 0, 0, 0, 0, 0, 0, 0, 0
};

static struct {
	pb_algorithm_t* algorithm;
	int ui_feedback_cnt_img;
	int ui_feedback_cnt_island;
} Global;

/* Simulate a template storage.
*
* NOTE!     Hard coded data size here to fit most tests, an
*           implementation would tune this depending on selected
*           template format and max template sizes.
*
* NOTE MCU: When using the mcu enrollment controller it uses an
*           incremental update only and hence updates are possible
*           without erasing Flash sector untill template
*           reaches max size and update ends.
*
* NOTE MPU: Storage may be as simple as writing data to a
*           regular file. The full controller creates templates
*           suited for continious update and may change on every
*           update hence a more sophisticated storage is required
*           compared to a simple Flash sector.
*/
struct flash_store {
	uint8_t  rfu1;
	uint8_t  rfu2;
	uint8_t  rfu3;
	uint8_t  type;
	uint32_t size;
	uint8_t  data[STORE_SIZE - 8];
} FLASH_STORE[STORE_FINGERS];

static const struct {
	const char* base;
	pb_sensor_size_t size;
	const char* name;
	const pb_algorithmI* algo;
} ALGO[] = {
	{ "ehm", PB_SENSOR_SIZE_SQUARE_XL, "ehmsq-xl", &hybrid_square_xl_algorithm },
	{ "ehm", PB_SENSOR_SIZE_SQUARE_L, "ehmsq-l", &hybrid_square_l_algorithm },
	{ "ehm", PB_SENSOR_SIZE_SQUARE_M, "ehmsq-m", &hybrid_square_m_algorithm },
	{ "ehm", PB_SENSOR_SIZE_SQUARE_S, "ehmsq-s", &hybrid_square_s_algorithm },
	{ "ehm", PB_SENSOR_SIZE_SQUARE_XS, "ehmsq-xs", &hybrid_square_xs_algorithm },
	{ "ehm", PB_SENSOR_SIZE_RECTANGULAR_M, "ehmre-m", &hybrid_rectangular_m_algorithm },
	{ "ehm", PB_SENSOR_SIZE_RECTANGULAR_S, "ehmre-s", &hybrid_rectangular_s_algorithm },
	{ "spect", PB_SENSOR_SIZE_SQUARE_XL, "spectsq-xl", &spectral_square_xl_algorithm },
	{ "spect", PB_SENSOR_SIZE_SQUARE_L, "spectsq-l", &spectral_square_l_algorithm },
	{ "spect", PB_SENSOR_SIZE_SQUARE_S, "spectsq-s", &spectral_square_s_algorithm },
	{ "ehm", PB_SENSOR_SIZE_SQUARE_XL, "ehmsq-xl-spmem", &hybrid_square_xl_speed_mem_algorithm },
	{ "ehm", PB_SENSOR_SIZE_SQUARE_L, "ehmsq-l-spmem", &hybrid_square_l_speed_mem_algorithm },
	{ "ehm", PB_SENSOR_SIZE_SQUARE_M, "ehmsq-m-spmem", &hybrid_square_m_speed_mem_algorithm },
	{ "ehm", PB_SENSOR_SIZE_SQUARE_M, "ehmsq-m-nonhr-spmem", &hybrid_square_m_non_hr_speed_mem_algorithm },
	{ "ehm", PB_SENSOR_SIZE_SQUARE_S, "ehmsq-s-spmem", &hybrid_square_s_speed_mem_algorithm },
	{ "ehm", PB_SENSOR_SIZE_RECTANGULAR_M, "ehmre-m-spmem", &hybrid_rectangular_m_speed_mem_algorithm },
	/* some other */
	{ "", PB_SENSOR_SIZE_RECTANGULAR_M, "ehmre-m-nonhr-spmem", &hybrid_rectangular_m_non_hr_speed_mem_algorithm },
	{ "", PB_SENSOR_SIZE_SQUARE_XL, "ehmsq-xl-spmem-slice", &hybrid_square_xl_speed_mem_slice_algorithm },
	{ "", PB_SENSOR_SIZE_SQUARE_L, "ehmsq-l-spmem-slice", &hybrid_square_l_speed_mem_slice_algorithm },
	{ "", PB_SENSOR_SIZE_SQUARE_XL, "ehmsw-slice", &hybrid_swipe_slice_algorithm },
	{ "", PB_SENSOR_SIZE_SQUARE_XL, "ehmsw-spmem-slice", &hybrid_swipe_speed_mem_slice_algorithm },
	{ "", PB_SENSOR_SIZE_SQUARE_XL, "spectsq-xl-slice", &spectral_square_xl_slice_algorithm },
	{ "", PB_SENSOR_SIZE_SQUARE_L, "spectsq-l-slice", &spectral_square_l_slice_algorithm },
	{ "", PB_SENSOR_SIZE_SQUARE_XL, "spectsw-slice", &spectral_swipe_slice_algorithm },
	{ 0 }
};

static pb_rc_t ui_display_event(pb_session_t* session, int event_, pb_finger_t* finger);
static pb_rc_t ui_display_islands(pb_session_t* session, pb_mte_island_t* islands, uint8_t nbr_of_islands);
static pb_rc_t ui_display_synthetic_image(pb_session_t* session, const pb_image_t* image);
static pb_rc_t ui_display_quality(pb_session_t* session, const pb_quality_t* quality, uint8_t image_quality_threshold, uint32_t area_threshold);
static pb_rc_t ui_display_progress(pb_session_t* session, uint16_t completed, uint16_t total);

static const pb_qualityI* gp_qality_i = &pb_quality;
static const char* far2str(pb_far_t far_);
static const char* algo2str(const pb_algorithmI* algo);
static pb_algorithm_t* init_algorithm(const pb_algorithmI** algoip);
static const pb_algorithmI* select_algo(const char* name, pb_sensor_size_t size, int mcu);

int file_check_available_index(int* file_index);
static int flash_write_template(pb_template_t* enroll_template, int finger);
static int flash_erase_template(int finger);
static pb_template_t* flash_read_template(int finger);
static int file_save_template(const char* finger_name, int finger_idx);
static int file_load_template(void);
static pb_template_t* file_read_template(int finger);
static pb_template_t** file_read_template_array(int template_size);
static int file_write_template(pb_template_t* templated, const char* finger_name, int finger);
static int file_write_template_update(pb_template_t* templated, int match_idx);
static int file_load_template_to_flash(void);
static int get_templates_size();

int lib_init(void);
int set_enroll_count(int count);
int get_enroll_count(void);

int verify_finish(pb_template_t* enroll_template);

int quality_chk_init(void);
int quality_chk(pb_image_t* image, uint8_t* image_quality, uint16_t* fingerprint_area);
int quality_chk_deinit(void);
static pb_image_t* read_image(const char* filename);

static const pb_mte_guiI UiFeedbackHandler = {
	&ui_display_event,
	&ui_display_islands,
	&ui_display_synthetic_image,
	&ui_display_quality,
	&ui_display_progress
};

int lib_init(void)
{
	int ret = -1;
	const char* algo = "ehm";
	const pb_algorithmI* algoi = 0;
	pb_sensor_size_t ssize;
	int ui_feedback_bkg = 0;
	pb_image_t* image = 0;
	pb_algorithm_config_t* algo_config = 0;
	uint32_t fwidth = 120, fheight = 120, fsize = 14400;
	int image_dpi = 508;
	uint8_t* image_buf = 0;

	unsigned char img_buffer_301c[14400];
	unsigned char img_buffer_501[30976];

	image_buf = (uint8_t*)malloc(g_sensor_img_buffer);
	memset(image_buf, 0, g_sensor_img_buffer);

	hw_init();
	Opt.do_load = 1;
	Opt.do_save = 1;
	ui_feedback_bkg = 1;
	Opt.dyn_update = 1;

	printf("Pb library init()..\n");

	image = pb_image_create(g_sensor_height, g_sensor_width, Opt.dpi, Opt.dpi, image_buf, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);

	/*
	if (sensor_width == 120) {
		image = pb_image_create(sensor_width, sensor_height, Opt.dpi, Opt.dpi, img_buffer_301c, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);
	}
	else if (sensor_width == 176) {
		image = pb_image_create(sensor_width, sensor_height, Opt.dpi, Opt.dpi, img_buffer_501, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);
	}
	*/
	if (!image) {
		printf("Failed capture first image to base algorithm choise on \n");
	}
	
	ssize = pb_image_get_sensor_size(image);
	printf("Image size is %dx%d @ %d dpi \n", pb_image_get_cols(image), pb_image_get_rows(image),
		pb_image_get_horizontal_resolution(image));
	pb_image_delete(image);
	image = 0;

	malloc_trim(0);

	printf("select algo\n\n");
	algoi = select_algo(algo, ssize, !Opt.mpu);

	if (!algoi) {
		printf("Cannot auto-select algorithm based on algo-name '%s' and the given image size \n", algo);
		return ret;
	}
	
	printf("init_algorithm\n\n");
	Global.algorithm = init_algorithm(&algoi);

	if (!Global.algorithm) {
		printf("Failed to create algorithm for '%s' \n", algo);
		return ret;
	}

	printf("pb_algorithm_get_config\n");

	algo_config = pb_algorithm_get_config(Global.algorithm);
	printf("Algorithm: %s \n", algo2str(algoi));
	printf("Algorithm: using %s enrollment controller version \n", Opt.mpu ? "MPU" : "MCU");
	if (Opt.mpu) {
		printf("          MPU - advanced enrollment and continuious update \n");
	}
	else {
		printf("          MCU - simpler enrollment and incremental update to capacity \n");
	}
	printf("Algorithm: using max %d subtemplates\n", algo_config->max_nbr_of_subtemplates);
	printf("Algorithm: max template size %d bytes %s\n", (int)algo_config->max_template_size, algo_config->max_template_size ? "" : "(unlimited)");
	printf("Algorithm: allow template update = %s\n", algo_config->lock_template_from_further_updates ? "no" : "yes");
	printf("Algorithm: prevent enroll several fingers = %s\n", algo_config->prevent_enrollment_of_multiple_fingers ? "yes" : "no");
	printf("Algorithm: max verifies in quick match = %d\n", algo_config->max_nbr_of_subtemplates_to_verify_against);
	printf("Algorithm: using max %d enrollment samples\n", algo_config->max_nbr_of_enrollment_templates);
	printf("Algorithm: Rotation support: %s\n", pb_algorithm_get_feature_state(Global.algorithm, PB_ALGORITHM_FEATURE_360) ? "360" : "straight");
	printf("Verify   : Requested FAR %s, %s match\n", far2str(Opt.ver_far), Opt.ver_score ? "full" : "quick");
	printf("\n");

	return 0;
}

int lib_deinit(void)
{
	pb_algorithm_delete(Global.algorithm);
	pb_image_delete(Opt.ui_feedback_bkg);
	return 0;
}

int set_enroll_count(int count)
{
	pb_algorithm_get_config(Global.algorithm)->max_nbr_of_enrollment_templates = count;
	printf("set_enroll_count => %d\n", count);
	return 0;
}

int get_enroll_count(void)
{
	int enroll_cnt = pb_algorithm_get_config(Global.algorithm)->max_nbr_of_enrollment_templates;
	//printf("get_enroll_count => %d\n", enroll_cnt);
	return enroll_cnt;
}

int image_quality_chk(const char* filename, uint8_t* p_resQuality, uint16_t* p_resCoverage)
{
	pb_image_t* image;

	int res = 1;

	uint8_t coverage = 0;

	quality_chk_init();

	image = read_image(filename);
	printf("filename => %s\n", filename);

	if (!image) {
		printf("Read image is fail.\n");
		res = -1;
	}
	else
	{
		quality_chk(image, p_resQuality, p_resCoverage);
		printf("Quality => %d, Coverage => %d\n", *p_resQuality, *p_resCoverage);
	}
	
	quality_chk_deinit();

	return res;
}

int quality_chk_init(void)
{
	printf("quality_chk_init()...\n");
	pb_session_set_360_state(0, 1);
	g_psession = pb_session_create();
	
	return 0;
}

int quality_chk(pb_image_t* image, uint8_t* image_quality, uint16_t* fingerprint_area)
{
	pb_sensor_size_t sensor_size;
	pb_quality_t*	 quality;
	int				 res = -1;

	sensor_size = pb_image_get_sensor_size(image);
	//printf("sensor_size => %d\n", sensor_size);
	pb_session_set_sensor_size(0, sensor_size);
	pb_session_set_sensor_type(0, PB_SENSOR_TYPE_TOUCH);

	//printf("compute quality start ..>>\n");
	res = gp_qality_i->compute_quality(g_psession, image, &quality);
	//printf("compute quality end   ..<<\n");

	*image_quality = pb_quality_get_image_quality(quality);
	//printf("Quality => %d\n", pb_quality_get_image_quality(quality));
	*fingerprint_area = pb_quality_get_area(quality);
	//printf("Coverage => %d\n", pb_quality_get_area(quality));

	pb_quality_delete(quality);

	return res;
}

int quality_chk_deinit(void)
{
	if (g_psession) {
		pb_session_delete(g_psession);
		g_psession = 0;
	}
	return 0;
}

int enroll_setup(void)
{
	printf("\n enroll_setup...\n");

	if (Opt.mpu) {
		gp_mte = pb_multitemplate_enroll_create_algorithm(
															Global.algorithm,
															PB_FINGER_ANONYMOUS,
															&UiFeedbackHandler,
															Opt.ui_feedback_bkg);
	}
	else {
		gp_mte = pb_multitemplate_enroll_mcu_create_algorithm(
																Global.algorithm,
																PB_FINGER_ANONYMOUS,
																&UiFeedbackHandler,
																Opt.enr_ver_limit,
																0);
	}

	if (!gp_mte) return -1;

	file_check_available_index(&g_finger_idx);

	quality_chk_init();

	return g_finger_idx;
}

int enroll_finger(pb_image_t* image, uint8_t* coverage_area)
{
	int res = -1;
	int num_accepted;

	uint8_t coverage = 0;

	pb_image_mr_soft(image);

	res = pb_multitemplate_enroll_run(gp_mte, image, &coverage);

	*coverage_area = coverage;

	pb_image_delete(image);

	num_accepted = pb_multitemplate_enroll_get_nbr_of_captures(gp_mte);

	//printf("gp_mte = %p, image = %p, cover = %d, num = %d\n", gp_mte, image, coverage, num_accepted);

	if (res == PB_RC_CAPACITY) {
		return 0;
	}
	else if (res != PB_RC_OK) {
		printf("Could not enroll images: %d\n", res);
		pb_multitemplate_enroll_delete(gp_mte);
	}

	return num_accepted;
}

void enroll_finish(int* template_size)
{
	printf("\n enroll_finish...\n");
	pb_template_t* enrolled_template = 0;
	int res = -1;

	*template_size = 0;

	printf("Finialzing enrollment.\n");

	res = pb_multitemplate_enroll_finalize_template_algorithm(Global.algorithm, gp_mte, &enrolled_template);
	if (res != PB_RC_OK) {
		printf("Could not finalize template: %d\n", res);
		pb_multitemplate_enroll_delete(gp_mte);
		pb_template_delete(enrolled_template);
	}

	*template_size = (int)pb_template_get_data_size(enrolled_template);
	printf("Template size  : %d\n", (int)pb_template_get_data_size(enrolled_template));

	res = flash_write_template(enrolled_template, g_finger_idx);
	if (res) {
		printf("flash_write_template() failed.\n");
		pb_multitemplate_enroll_delete(gp_mte);
		pb_template_delete(enrolled_template);
	}

	res = file_save_template(g_finger_name, g_finger_idx);
	if (res) {
		printf("file_save_template index failed.\n");
	}

	if (gp_mte) {
		pb_multitemplate_enroll_delete(gp_mte);
		gp_mte = 0;
	}

	if (enrolled_template) pb_template_delete(enrolled_template);

	quality_chk_deinit();
}

int verify_setup(void)
{
	printf("\n verify_setup...\n");

	g_decision = PB_DECISION_NON_MATCH;

	quality_chk_init();

	return 0;
}

int verify_finger(pb_image_t* image)
{
	printf("\n verify_finger...\n");
	int res = -1;
	pb_template_t* enrolled_template = 0;

	printf("pb_algorithm_extract_template start\n");
	res = pb_algorithm_extract_template(
										Global.algorithm,
										image,
										PB_FINGER_ANONYMOUS,
										&gp_verification_template);
	printf("pb_algorithm_extract_template end\n");

	pb_image_delete(image);
	if (res != PB_RC_OK) {
		printf("extraction failed: %d\n", res);
		res = -1;
	}

	if (gp_verification_template) {
		uint16_t	score	= 0;
		uint16_t*	scorep	= 0;
		int			i		= 0;

		pb_template_t* dyn_updated = 0;

		if (Opt.ver_score) {
			scorep = &score;
		}

		while (true) {				
			
			int databaseSize = get_templates_size();
			printf("databaseSize => %d\n", databaseSize);
			//pb_template_t** templates = file_read_template_array(databaseSize);
			database_templates_array = file_read_template_array(databaseSize);
			printf("  verify quickly: %d fingers\n", databaseSize % 256);

			/*
			res = pb_algorithm_verify_template(
												Global.algorithm,
												enrolled_template,
												gp_verification_template,
												Opt.ver_far,
												scorep,
												&g_decision,
												0);
			*/
			res = pb_algorithm_verify_template_quickly(
														Global.algorithm,
														database_templates_array,
														databaseSize,
														gp_verification_template,
														Opt.ver_far,
														&g_decision,
														0,
														&g_match_idx);
				
			printf("verify_template_quickly end, res = %d, decision = %d, match idex = %d\n", res, g_decision, g_match_idx);
			if (res != PB_RC_OK) {
				printf(" verification failed : %d \n", res);
			}

			if (g_decision == PB_DECISION_MATCH) {
				printf("match_idx = %d, g_decision = %d\n", g_match_idx, g_decision);
				res = g_match_idx;
				break;
			}
			else {
				printf("matching index failure\n");
				free(database_templates_array);
				res = -1;					
				break;
			}				
			i++;
		}
	}

	if (image) {
		pb_image_delete(image);
		image = NULL;
	}

	return res;
}

int verify_finish(void)
{
	printf("\n verify_finish...\n");
	int res = -1;
	pb_template_t* enroll_template1 = database_templates_array[g_match_idx];
	pb_template_t* templ = 0;

	free(database_templates_array);

	if (Opt.dyn_update && g_decision == PB_DECISION_MATCH) {
		printf("Dynamic update...\n");

		if (gp_dyn_updated) {
			pb_template_delete(gp_dyn_updated);
			gp_dyn_updated = NULL;
		}
	
		pb_algorithm_update_multitemplate(
											Global.algorithm,
											enroll_template1,
											&gp_verification_template,
											1,
											&gp_dyn_updated);

		if (enroll_template1) {
			pb_template_delete(enroll_template1);
		}
	}

	if (gp_verification_template) {
		pb_template_delete(gp_verification_template);
	}

	printf("gp_dyn_updated = %p\n", gp_dyn_updated);
	
	if (gp_dyn_updated) {
		uint8_t type = (uint8_t)pb_template_get_type(gp_dyn_updated);
		const uint8_t* data = pb_template_get_data(gp_dyn_updated);
		uint32_t data_size = pb_template_get_data_size(gp_dyn_updated);

		printf("type = %d\n", type);
		printf("data = %d\n", data);
		printf("data_size = %d\n", data_size);

		//if (Opt.mt_size_bytes - data_size > 1200) {

			printf("file_write_template dyn_updated\n");

			res = flash_write_template(gp_dyn_updated, g_match_idx);
			if (res) {
				printf("flash_write_template() failed.\n");
				pb_multitemplate_enroll_delete(gp_mte);
				pb_template_delete(gp_dyn_updated);
			}

			templ = flash_read_template(g_match_idx);

			if (templ) {
				res = file_write_template_update(templ, g_match_idx);
				printf("FILE updated - stored template is %d bytes.\n", (int)pb_template_get_data_size(templ));
			}

			pb_template_delete(templ);
			gp_dyn_updated = NULL;
			templ = NULL;

			printf("template updated done.\n");
		//}
	}
	else {
		printf("gp_dyn_updated is null\n");
	}
	
	//quality_chk_deinit();
	return 0;
}

static pb_image_t* read_image(const char* filename)
{
	printf("start read image\n");
	pb_image_t* image = 0;
	//unsigned char* image_buffer;
	int image_dpi = 508;
	//unsigned int cols;
	//unsigned int rows;

	if (strstr(filename, ".raw")) {
		FILE * fd;
		unsigned char img_buf[14400];

		if ((fd = fopen(filename, "r")) != NULL) {
		
			printf("File open OK!\n");
			fread(img_buf, 14400, 1, fd);
			fclose(fd);

			image = pb_image_create(120, 120, 508, 508, img_buf, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);
		}
	}
	
	if (image && Opt.crop_cols && Opt.crop_rows) {
		pb_image_t* tmp = pb_image_crop_centered(image, Opt.crop_rows, Opt.crop_cols);
		pb_image_delete(image);
		image = tmp;
	}

	if (!image) {
		printf("ERROR: Failed to read PNG/BMP image %s\n", filename);
	}

	return image;
}

static pb_algorithm_t* init_algorithm(const pb_algorithmI** algoip)
{
	const pb_algorithmI* algoi = *algoip;
	pb_algorithm_t* algorithm;
	pb_algorithm_config_t* algo_config;

	/* Create instance of algoritm of the selected type. */

	if (!algoi) {
		/* Option "-algo other" given with the intention to specify
		* it here instead - the example code only includes a few
		* sample algorithms from all that are available.
		*/
		//algoi = &hybrid_swipe_speed_mem_slice_algorithm;
		//algoi = &hybrid_square_xl_speed_mem_slice_algorithm;
		//algoi = &minutiae_square_xl_speed_mem_algorithm;
		//Opt.preproc_filter = &pb_preprocessor;        
	}
	if (!algoi) return 0;
	algorithm = algoi->create(0);

	/* Optional; set / modify algorithm parameters and configuration.
	*
	* In demo applocation this is done via command line options but
	* may typically be hardcoded.
	*/
	algo_config = pb_algorithm_get_config(algorithm);
	if (Opt.mt_size)         algo_config->max_nbr_of_subtemplates = Opt.mt_size;
	if (Opt.mt_size_bytes)   algo_config->max_template_size = Opt.mt_size_bytes;
	if (Opt.lock_update)     algo_config->lock_template_from_further_updates = Opt.lock_update;
	if (Opt.enr_samples)     algo_config->max_nbr_of_enrollment_templates = Opt.enr_samples;
	if (Opt.quick_ver_limit) algo_config->max_nbr_of_subtemplates_to_verify_against = Opt.quick_ver_limit;
	if (Opt.straight)        pb_algorithm_set_feature_state(algorithm, PB_ALGORITHM_FEATURE_360, 0);

	*algoip = algoi;
	return algorithm;
}

static const char* algo2str(const pb_algorithmI* algo)
{
	int i;
	if (algo == 0) return "unspecified";
	for (i = 0; ALGO[i].name; i++) {
		if (algo == ALGO[i].algo) return ALGO[i].name;
	}
	return "unknown/other";
}

static const pb_algorithmI* select_algo(const char* name, pb_sensor_size_t size, int mcu)
{
	int i;
	char* spmem = mcu ? "-spmem" : ""; /* If MCU prefer speedmem if available */

	/* Default to ehm of given size */
	if (name == 0) name = "ehm";

	/* Select on full name */
	for (i = 0; ALGO[i].name; i++) {
		if (strcmp(name, ALGO[i].name) == 0) {
			return ALGO[i].algo;
		}
	}

	/* try base name with sensor size and speedmem version if preferred */
	for (i = 0; ALGO[i].name; i++) {
		if (strcmp(name, ALGO[i].base) == 0 && size == ALGO[i].size && strstr(ALGO[i].name, spmem)) {
			return ALGO[i].algo;
		}
	}
	for (i = 0; ALGO[i].name; i++) {
		if (strcmp(name, ALGO[i].base) == 0 && size == ALGO[i].size) {
			return ALGO[i].algo;
		}
	}

	return 0;
}

static const struct {
	pb_far_t far_;
	const char* name;
} Far_map[] = {
	{ PB_FAR_5, "1:5" },
	{ PB_FAR_10, "1:10" },
	{ PB_FAR_50, "1:50" },
	{ PB_FAR_100, "1:100" },
	{ PB_FAR_500, "1:500" },
	{ PB_FAR_1000, "1:1K" }, { PB_FAR_1000, "1:1000" },
	{ PB_FAR_5000, "1:5K" }, { PB_FAR_5000, "1:5000" },
	{ PB_FAR_10000, "1:10K" }, { PB_FAR_10000, "1:10000" },
	{ PB_FAR_50000, "1:50K" }, { PB_FAR_50000, "1:50000" },
	{ PB_FAR_100000, "1:100K" }, { PB_FAR_100000, "1:100000" },
	{ PB_FAR_500000, "1:500K" }, { PB_FAR_500000, "1:500000" },
	{ PB_FAR_1000000, "1:1M" }, { PB_FAR_1000000, "1:1000000" },
};

static const char* far2str(pb_far_t far_)
{
	int i = 0;
	while (Far_map[i].name) {
		if (far_ == Far_map[i].far_) return Far_map[i].name;
		i++;
	}
	return "1:?";
}

static pb_rc_t ui_display_event(pb_session_t* session, int event_, pb_finger_t* finger)
{
	switch (event_) {
	case PB_MTE_EVENT_PROMPT_MOVE_FINGER_MORE:
		printf("        UI feedback: Move finger more!\n");
		break;
	case PB_MTE_EVENT_PROMPT_MOVE_FINGER_LESS:
		printf("        UI feedback: Do not move finger too much\n");
		break;
	case PB_MTE_EVENT_ALERT_DUPLICATE_TEMPLATE:
		printf("        UI feedback: Duplicate template\n");
		/* As user feedback the move finger more will trigger when
		* duplicate is with the previous sample. */
		break;
	case PB_MTE_EVENT_ALERT_TEMPLATE_ENROLLED:
		printf("        UI feedback: Alert template enrolled\n");
		break;
	case PB_MTE_EVENT_ALERT_TEMPLATE_EXTRACTED:
		printf("        UI feedback: Alert template extracted\n");
		break;
	case PB_MTE_EVENT_PROMPT_PLACE_FINGER:
		printf("        UI feedback: Place finger\n");
		break;
	case PB_MTE_EVENT_PROMPT_LIFT_FINGER:
		printf("        UI feedback: Lift finger\n");
		break;
	}
	return PB_RC_OK;
}

/**
* This example will save feedback images to files during enrollment.
* View these to get a basic example of how such a feedback image
* may guide enhance the enrollment procedure.
*/
static pb_rc_t ui_display_synthetic_image(pb_session_t* session, const pb_image_t* image)
{
#   ifndef NOFILES
	/* Disable first..*/
#if 0
	char filename[100];
	int cols = pb_image_get_cols(image);
	int rows = pb_image_get_rows(image);
	const uint8_t* pixels = pb_image_get_pixels(image);
	sprintf(filename, "ui-enr-feedback-%02d.png", ++Global.ui_feedback_cnt_img);
	pbpng_buffer_to_pngfile(pixels, cols, rows, PB_BIR_ENCODE_BM8, filename);
#endif
#   endif
	//UNUSED(session);
	//UNUSED(image);
	return PB_RC_OK;
}

/**
* This example will not render any nice graphical feedback images,
* the available feedback events will be stored in a text file.
*
* When not used setting this method to the NULL pointer will
* prevent any overhead of generating the feedback.
*
*/
static pb_rc_t ui_display_islands(pb_session_t* session, pb_mte_island_t* islands, uint8_t nbr_of_islands)
{
	/* See API for more info, some basics,
	*
	* The feedback is given as a list of "islands". An island
	* is a group of image samples that is close enough to
	* match each other. The first island is always the largest
	* one, just focusing on one is simpler but especially in the
	* beginning of enrollment several islands may be created
	* and user still needs some feedback to stay happy.
	*
	* An island is further described as a list or rectangles
	* which each represents an image sample, the very first of
	* these is the "root" of the island and will remain the same
	* and its top corner is at (0,0). The remaining rectangle
	* coordinates will reflect the relative position to the root.
	*
	* The intention is to make a GUI render this information
	* over a stylished fingerprint image in some fancy way and
	* also display other feedback events part along with this.
	*
	* As enrollment progresses islands may be joined together
	* and thus be fewer than previously.
	*/
	//UNUSED(session); UNUSED(islands); UNUSED(nbr_of_islands);
#   ifdef NOFILES
	/* NOTE: islands must be deleted unless return unsupported */
	return PB_RC_NOT_SUPPORTED;
#   else
	/* Non-used so far, comment*/
#if 0
	int i, r;
	//FILE* fp;
	JMT_FILE fp = 0;

	UNUSED(session);
	//fp = fopen("ui-enr-feedback.txt", "a");
	JMT_FOPEN(&fp, "ui-enr-feedback.txt", APPEND)
	if (!fp) return PB_RC_NOT_SUPPORTED;

	/* This demo will just loop throgh the data structure and
	* log textual representation of the feedback data. */

	fprintf(fp, "Feedback #%2d\n", ++Global.ui_feedback_cnt_island);
	fprintf(fp, "-------------\n");
	for (i = 0; i < nbr_of_islands; i++) {
		fprintf(fp, "Island #%d:\n", i);
		for (r = 0; r < islands[i].nbr_of_rectangles; r++) {
			const char* duplicate = "";
			const char* latest = "";
			pb_mte_rectangle_t* rect = &islands[i].rectangles[r];
			int flag = rect->flag;
			if (flag & PB_MTE_RECTANGLE_FLAG_LATEST)    latest = "LATEST";
			if (flag & PB_MTE_RECTANGLE_FLAG_DUPLICATE) duplicate = "DUPLICATE";
			fprintf(fp, "  rect id=%p (%4d,%4d)(%4d,%4d)(%4d,%4d)(%4d,%4d) %7s %s\n",
				rect->subtemplate,
				rect->corners[0].x, rect->corners[0].y,
				rect->corners[1].x, rect->corners[1].y,
				rect->corners[2].x, rect->corners[2].y,
				rect->corners[3].x, rect->corners[3].y,
				latest, duplicate);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
	JMT_FCLOSE(fp);

	pb_mte_delete_islands(islands, nbr_of_islands);

	return PB_RC_OK;
#else
	return PB_RC_OK;
#endif
#   endif
}

static pb_rc_t ui_display_quality(pb_session_t* session, const pb_quality_t* quality, uint8_t image_quality_threshold, uint32_t area_threshold)
{
	return PB_RC_NOT_SUPPORTED;
}

static pb_rc_t ui_display_progress(pb_session_t* session, uint16_t completed, uint16_t total)
{
	//printf("ui_display_progress => %d, %d\n", completed, total);
	return PB_RC_OK;
}

int file_check_available_index(int* file_index)
{
	FILE* fp;
	char buf[200];
	char string[200];
	time_t		now = time(0);
	struct tm	tstruct;
	char		date_buf[80];
	tstruct = *localtime(&now);

	int i = 1;
	*file_index = -1;

	strftime(date_buf, sizeof(date_buf), "%Y%m%d", &tstruct);
	
	strcpy(string, g_finger_database_path);
	strcat(string, "\\");
	strcat(string, date_buf);
	strcat(string, "_");
	strcat(string, g_finger_name);
	strcat(string, "_%02d.bir");

	while (true) {
		sprintf(buf, string, i);
		printf("file_check_available_index => %s\n", buf);
		if (access(buf, 0) == -1) {
			printf("Found index %d is available\n", i);
			*file_index = i;
			break;
		}
		i++;
	}
	return 0;
}

static int flash_erase_template(int finger)
{
	if (finger != -1) {
		struct flash_store* store = &FLASH_STORE[finger];
		memset(store, 0xFF, sizeof(struct flash_store));
		printf("    FLASH erased\n");
		return 0;
	}
	else {
		return -1;
	}
}

static pb_template_t* flash_read_template(int finger)
{
	//if (finger < STORE_FINGERS) {
	if (finger != -1) {
		struct flash_store* store = &FLASH_STORE[finger];
		printf("read finger template => %d\n", finger);

		if (store->size == 0 || store->size > sizeof(store->data)) {
			/* Flash uninitialized */
			printf("store->size == 0\n");
			return 0;
		}
		else {
			/* Using the _mr function to reference data in flash instead
			* of making a copy. For MCU this is more RAM efficient
			* and especially for incremental updates where the data size
			* parameter is typically max alloted flash space.
			*/
			printf("get template %d, type = %d, size = %d\n", finger, store->type, store->size);

			return pb_template_create_mr(
										 (pb_template_type_t)FLASH_STORE[finger].type,
										 FLASH_STORE[finger].data,
										 FLASH_STORE[finger].size);
		}
	}
	else {
		return 0;
	}
}

static int flash_write_template(pb_template_t* templated, int finger)
{
	printf("\n flash_write_template...\n");
	if (finger != -1) {
		uint32_t i, erase = 0;
		struct flash_store* store = &FLASH_STORE[finger];
		uint8_t type = (uint8_t)pb_template_get_type(templated);
		const uint8_t* data = pb_template_get_data(templated);
		uint32_t data_size = pb_template_get_data_size(templated);

		printf("template : %d\n", finger);
		printf("template type = %d\n", type);
		printf("tenplate data size = %d\n", data_size);

		if (data_size > sizeof(store->data)) {
			printf("    template is too large %d for flash storage configuration of %d bytes\n",
				(int)data_size, (int)sizeof(store->data));
			printf("     either increase define STORE_SIZE or set -mt_size_bytes\n");
			return -1;
		}

		/* Check if flash needs to be erased and reinitialized */

		if ((store->size != 0xFFFFFFFF || store->type != 0xFF) &&
			(store->size == 0 || store->size > sizeof(store->data) ||
			store->size < data_size || type != store->type)) {
			/* Apparently not erased, or needs change */
			erase = 1;
		}
		else {
			/* Check if incremental update is possible, expected with
			* MCU incremental templates until max size reached.*
			*
			* MPU uses more complex update and will most likely not
			* pass this test and such platforms likely needs a more
			* advanced flash file system to handle continuous
			* updates.
			*
			* NOTE: The max number of subtemplates and especially the
			*       max template size parameters need to be set
			*       according to the size of data storage, else
			*       updates will be attempted but fail when written
			*       to flash.
			*
			* NOTE: The basis for this is that most Flash memories
			*       are filled with 0xFF when erased and bits can
			*       then be reset to 0.
			*/
			for (i = 0; i < data_size; i++) {
				if ((data[i] & store->data[i]) != data[i]) erase = 1;
				//if (store->data[i] != 0xFF && data[i] != store->data[i]) erase = 1;
			}
		}

		if (erase) {
			flash_erase_template(finger);
		}

		/* Store template data first, and update meta info after, makes it a bit
		* more robust if aborted.
		*/
		memcpy(store->data, data, data_size);
		/*
		for (i = 0; i < data_size; i++) {
		store->data[i] = data[i];
		}
		*/

		if (store->size == 0xFFFFFFFF) {
			/* Setting metadata last makes it more robust if aborted.
			*
			* On every template update the template size is the actual
			* size but here the size is forced to the maximum available
			* size. This is valid as long as any extra data is 0xFF.
			*
			* If not done then even for MCU using incremental update
			* the size test would fail and require erase every time.
			*
			* NOTE: When using flash with direct memory access the
			*       extra data size used here does not need to be
			*       copied to RAM when using the pb_template_create_mr()
			*       function.
			*/
			store->type = type;
			store->size = sizeof(store->data);

			// Charles add
			//store->size = data_size;
		}

		printf("    FLASH updated - template size %d, maximum is %d bytes\n", (int)data_size, (int)store->size);
		return 0;
	}
	else {
		return -1;
	}
}

static int file_save_template(const char* finger_name, int finger_idx)
{
	int res = -1;
	pb_template_t* templ = 0;
	char file;
	string a;
	
	if (finger_idx != -1) {
		templ = flash_read_template(finger_idx);

		if (templ) {
			res = file_write_template(templ, finger_name, finger_idx);
			printf("FILE updated - stored template is %d bytes.\n", (int)pb_template_get_data_size(templ));			
		}
		pb_template_delete(templ);
	}
	return res;
}

static int file_load_template(void)
{
	int i = 0;
	int res = -1;
	bool flag = true;

	//for (i = 0; i < STORE_FINGERS; i++) {
	while (flag) {
		pb_template_t* templ = file_read_template(i);

		if (templ) {
			printf("File load - size is %d bytes\n", (int)pb_template_get_data_size(templ));

			res = flash_write_template(templ, i);
			pb_template_delete(templ);
		}
		else
		{
			flag = false;
		}
		i++;
	}
	return res;
}

static pb_template_t* file_read_template(int finger)
{
	FILE* fp;
	size_t size;
	char filename[100];
	uint8_t type;
	uint8_t* data;
	pb_template_t* templ = 0;

	sprintf(filename, "database/multi-template-%d.bir", finger);
	//sprintf(filename, "database/%4d%2d%02d_finger_%d.bir", year, month, mday, finger);
	
	if (access(filename, 0) == -1) {
		printf("access %s = -1\n", filename);
		return 0;
	}

	fp = fopen(filename, "rb");
	if (fp == 0) {
		printf("FILE open failed: %s\n", filename);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	rewind(fp);
	data = (uint8_t*)malloc(size);
	if (fread(&type,	   1, 1, fp) != 1 ||
		fread(data, size - 1, 1, fp) != 1) {
		printf("FILE read failed\n");
	}
	else {
		templ = pb_template_create((pb_template_type_t)type, data, (uint32_t)size - 1);
	}
	fclose(fp);
	free(data);
	
	return templ;
}

static pb_template_t** file_read_template_array(int template_size)
{
	printf("\n file_read_template_array...\n");
	FILE* fp;
	size_t size;
	char filename[200];
	char string[200];
	uint8_t type;
	uint8_t* data;
	pb_template_t* templ = 0;
	pb_template_t** templates = (pb_template_t**)malloc(sizeof(pb_template_t*)* template_size);

	char path[200];
	struct _finddata_t c_file;
	intptr_t hFile = 0;
	int i = 0;

	strcpy(filename, g_finger_database_path);
	strcat(filename, "\\*.bir");
	//printf("filename => %s\n", filename);

	strcpy(string, g_finger_database_path);
	strcat(string, "\\%s");
	//printf("string => %s\n", string);

	hFile = _findfirst(filename, &c_file);
	if (hFile != -1) {
		do {
			sprintf(path, string, c_file.name);
			//printf("template path => %s\n", path);
			//printf("template name => %s\n", c_file.name);
			if (access(path, 0) == -1) {
				printf("access %s = -1\n", path);
				return 0;
			}

			fp = fopen(path, "rb");
			if (fp == 0) {
				printf("FILE open failed: %s\n", path);
				return 0;
			}
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			rewind(fp);
			data = (uint8_t*)malloc(size);
			if (fread(&type, 1, 1, fp) != 1 ||
				fread(data, size - 1, 1, fp) != 1) {
				printf("FILE read failed\n");
			}
			else {
				templ = pb_template_create((pb_template_type_t)type, data, (uint32_t)size - 1);
				templates[i] = templ;				
			}
			fclose(fp);
			free(data);
			i++;
		} while (_findnext(hFile, &c_file) == 0);
	}
	_findclose(hFile);

	return templates;
}

static int get_templates_size()
{
	char path[100];
	char filename[100];
	struct _finddata_t c_file;
	intptr_t hFile;
	int size = 0;

	strcpy(filename, g_finger_database_path);
	strcat(filename, "\\*.bir");

	hFile = _findfirst(filename, &c_file);
	if (hFile != -1) {
		do {
			size++;
		} while (_findnext(hFile, &c_file) == 0);
	}
	return size;
}

static int file_write_template(pb_template_t* templated, const char* finger_name, int finger)
{
	FILE* fp;
	char filename[200];
	uint8_t type		= (uint8_t) pb_template_get_type(templated);
	const uint8_t* data = pb_template_get_data(templated);
	uint32_t data_size	= pb_template_get_data_size(templated);

	char string[200];
	char outputstring[80];

	time_t		now = time(0);
	struct tm	tstruct;
	char		buf[80];
	tstruct = *localtime(&now);

	strftime(buf, sizeof(buf), "%Y%m%d", &tstruct);

	strcpy(string, g_finger_database_path);
	strcat(string, "\\");
	strcat(string, buf);
	strcat(string, "_");
	strcat(string, finger_name);
	strcat(string, "_%02d.bir");

	strcpy(outputstring, buf);
	strcat(outputstring, "_");
	strcat(outputstring, finger_name);
	strcat(outputstring, "_%02d.bir");

	sprintf(filename, string, finger);
	sprintf(outputfilename, outputstring, finger);
	printf("outputfilename => %s\n", outputfilename);

	fp = fopen(filename, "wb");
	printf("%s\n", filename);
	if (fp == 0) {
		printf("FILE open failed: %s\n", filename);
		return -1;
	}

	if (fwrite(&type, 1, 1, fp) != 1 ||
		fwrite(data, data_size, 1, fp) != 1) {
		printf("FILE write failed\n");
		fclose(fp);
		return -1;
	}
	printf("  FLASH UPDATED - template size %d\n", (int)data_size);

	fclose(fp);
	return 0;
}

static int file_write_template_update(pb_template_t* templated, int match_idx)
{
	printf("\n file_write_template_update...\n");
	FILE* fp;
	char filename[100];
	uint8_t type = (uint8_t)pb_template_get_type(templated);
	const uint8_t* data = pb_template_get_data(templated);
	uint32_t data_size = pb_template_get_data_size(templated);
	char string[100];

	char path[200];
	struct _finddata_t c_file;
	intptr_t hFile = 0;
	int i = 0;

	strcpy(filename, g_finger_database_path);
	strcat(filename, "\\*.bir");

	strcpy(string, g_finger_database_path);
	strcat(string, "\\%s");

	hFile = _findfirst(filename, &c_file);
	if (hFile != -1) {
		do {
			sprintf(path, string, c_file.name);
			
			if (i == match_idx) {
				fp = fopen(path, "wb");
				printf("%s\n", path);
				if (fp == 0) {
					printf("FILE open failed: %s\n", filename);
					return -1;
				}


				if (fwrite(&type, 1, 1, fp) != 1 ||
					fwrite(data, data_size, 1, fp) != 1) {
					printf("FILE write failed\n");
					fclose(fp);
					return -1;
				}

				printf("  FLASH UPDATED - template size %d\n", (int)data_size);

				fclose(fp);
				break;
			}
			else {
				i++;
			}
		} while (_findnext(hFile, &c_file) == 0);
	}
	_findclose(hFile);
	return 0;
}

char* get_output_file_name()
{
	return outputfilename;
}

int set_archive_path(const char* database_path, const char* filename)
{
	g_finger_database_path = database_path;
	g_finger_name = filename;

	printf("path => %s\n", g_finger_database_path);
	printf("name => %s\n\n", g_finger_name);
	return 0;
}

void setSensorType(int width, int height)
{
	g_sensor_width = width;
	g_sensor_height = height;

	if (width == 120 && height == 120) {
		g_sensor_img_buffer = 14400;
	}
	else if (width == 176 && height == 176) {
		g_sensor_img_buffer = 30976;
	}
	else if (width == 128 && height == 128) {
		g_sensor_img_buffer = 16384;
	}
}
