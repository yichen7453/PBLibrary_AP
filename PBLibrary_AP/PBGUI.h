#pragma once

#include <pb_image.h>
#include <pb_algorithm.h>
#include <pb_multitemplate_enrollment_algorithm.h>
#include <pb_preprocessorI.h>

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\legacy\compat.hpp>

#include <msclr/marshal_cppstd.h>

#include "pb_ctl_secure.h"
#include "define.h"

#include <iostream>
#include <fstream>
#include <conio.h>


namespace PBLibrary_AP {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;
	using namespace System::IO;

	using namespace System::Diagnostics;
	using namespace System::Runtime::InteropServices;
	using namespace System::Threading;

	using namespace std;

	vector<string> FILENAME = vector<string>();
	vector<string> FILEPATH = vector<string>();

	int deviceNumber = -1;
	double dWidth = 0;
	double dHeight = 0;
	VideoCapture video_capture;
	CvCapture * capture;
	IplImage * frame;

	int	max_samples = 0;
	int num_accepted = 0;
	int num_accepted_prev = 0;

	bool quality_chk_flag;
	bool get_frame;
	int verify_fail = 0;

	bool start_capture;
	bool start_realtime;
	bool start_enroll;
	bool start_verify;

	bool enroll_save_image;
	bool enroll_preview;
	bool verify_preview;
	bool verify_con;

	char file_path_string[200];

	char databaseFile[200];
	char listViewFile[200];

	char imageFile[100];
	char logo[100];
	char fingerprint_enrollScan[200];
	char fingerprint_enrollProgress[200];
	char fingerprint_enrollFinish[200];
	char fingerprint_verifyScan[200];
	char fingerprint_verifySuccess[200];
	char fingerprint_verifyFail[200];

	char* app_path = (char*)(void*)Marshal::StringToHGlobalAnsi(Application::StartupPath);

	int finger_idx = -1;
	char date_buf[80];
	char datetime_buf[80];
	char finger_idx_buf[10];
	char* filename;

	/// <summary>
	/// PBGUI 的摘要
	/// </summary>
	public ref class PBGUI : public System::Windows::Forms::Form
	{
	public:
		PBGUI(void)
		{
			InitializeComponent();
			//
			//TODO:  在此加入建構函式程式碼
			//			

			strcpy(file_path_string, app_path);
			strcat(file_path_string, "\\res");
			strcat(file_path_string, "\\logo.png");
			sprintf(logo, file_path_string);
			logoImage = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)logo));
			pictureBox1->Image = logoImage;

			time_t		now = time(0);
			struct tm	tstruct;
			tstruct = *localtime(&now);

			strftime(date_buf, sizeof(date_buf), "%Y%m%d", &tstruct);			
			//printf("date => %s\n", date_buf);

			enrollProgressImage = get_enroll_progress_image();

			template_path = Application::StartupPath + "\\Template\\";
			image_path	  = Application::StartupPath + "\\Fingerprint Image\\";
		}

	protected:
		/// <summary>
		/// 清除任何使用中的資源。
		/// </summary>
		~PBGUI()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btnOpen_device;

	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::Button^  btnRealTime;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;

	private: System::Windows::Forms::Timer^  capture_timer;
	private: System::Windows::Forms::Timer^  realtime_timer;
	private: System::Windows::Forms::Timer^  enroll_timer;
	private: System::Windows::Forms::Timer^  verify_timer;

	private: System::Windows::Forms::Button^  btnCancel;
	private: System::Windows::Forms::Button^  btnEnroll;
	private: System::Windows::Forms::Button^  btnVerify;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label_area;

	private: System::Windows::Forms::Label^  label_quality;

	private: System::Windows::Forms::GroupBox^  groupBox4;

	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  tb_enroll_fingerprint_area;

	private: System::Windows::Forms::TextBox^  tb_enroll_image_quality;

	private: System::Windows::Forms::TextBox^  textBox2;

	private: System::Windows::Forms::TextBox^  tb_enroll_count;

	private: System::Windows::Forms::Label^  label8;

	private: System::Windows::Forms::Label^  lb_message;
	private: System::Windows::Forms::Label^  lb_enroll_fingerprint_area;

	private: System::Windows::Forms::Label^  lb_enroll_image_quality;

	private: System::Windows::Forms::Label^  lb_enroll_count;
	private: System::Windows::Forms::Label^  lb_enroll_return_coverage;

	private: System::Windows::Forms::TextBox^  tb_enroll_return_coverage;

	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::Button^  btnDeleteAll;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::TextBox^  tb_enroll_name;

	private: System::Windows::Forms::CheckBox^  cb_enroll_preview;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::CheckBox^  cb_verify_preview;
	private: System::Windows::Forms::CheckBox^  cb_verify_con;
	private: System::Windows::Forms::Button^  btnCapture;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  lb_verify_fingerprint_area;
	private: System::Windows::Forms::Label^  lb_verify_image_quality;
	private: System::Windows::Forms::TextBox^  tb_verify_fingerprint_area;
	private: System::Windows::Forms::TextBox^  tb_verify_image_quality;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::PictureBox^  pb_gif;
	private: System::Windows::Forms::PictureBox^  pb_png;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::Label^  lb_open_parameter;
	private: System::Windows::Forms::Label^  lb_enroll_parameter;
	private: System::Windows::Forms::Label^  lb_open_colon;
	private: System::Windows::Forms::Label^  lb_open_information;
	private: System::Windows::Forms::Label^  lb_enroll_colon;
	private: System::Windows::Forms::Label^  lb_enroll_information;
	private: System::Windows::Forms::ListView^  listView;

	private: System::Windows::Forms::ColumnHeader^  columnHeader1;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Button^  btnChooseFile;
	private: System::Windows::Forms::Label^  lb_filename;
	private: System::Windows::Forms::PictureBox^  pb_showFile;
	private: System::Windows::Forms::CheckBox^  cb_501;
	private: System::Windows::Forms::CheckBox^  cb_301c;
	private: System::Windows::Forms::CheckBox^  cb_301b;

	private: System::Windows::Forms::Button^  btnSave;

	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// 設計工具所需的變數。
		/// </summary>
		array<System::String^>^ fileList;
		array<System::Drawing::Bitmap^>^ enrollProgressImage;
		array<System::String^>^ filePath; 

		Bitmap^ logoImage;
		Bitmap^ enrollScan;
		Bitmap^ enrollProgress;
		Bitmap^ enrollFinish;
		Bitmap^ verifyScan;
		Bitmap^ verifySuccess;
		Bitmap^ verifyFail;

		System::String^ sensorType;

		int max_enrollment_samples		= 0;
		int max_fingerprint_area		= 0;
		int set_enrollment_samples		= 8;
		int enrollment_image_quality	= 0;
		int enrollment_fingerprint_area = 0;
		int verify_image_quality		= 0;
		int verify_fingerprint_area		= 0;

		System::String^ template_path;
		System::String^ image_path;
		System::String^ template_type_path;
		System::String^ image_type_path;
private: System::Windows::Forms::CheckBox^  cb_enroll_save_image;

		System::Drawing::Bitmap^ bmp;


#pragma region Windows Form Designer generated code
		/// <summary>
		/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器
		/// 修改這個方法的內容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(PBGUI::typeid));
			this->btnOpen_device = (gcnew System::Windows::Forms::Button());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->btnRealTime = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->btnSave = (gcnew System::Windows::Forms::Button());
			this->label_quality = (gcnew System::Windows::Forms::Label());
			this->label_area = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->btnCapture = (gcnew System::Windows::Forms::Button());
			this->btnDeleteAll = (gcnew System::Windows::Forms::Button());
			this->btnVerify = (gcnew System::Windows::Forms::Button());
			this->btnEnroll = (gcnew System::Windows::Forms::Button());
			this->btnCancel = (gcnew System::Windows::Forms::Button());
			this->realtime_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->enroll_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->verify_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->cb_enroll_save_image = (gcnew System::Windows::Forms::CheckBox());
			this->cb_enroll_preview = (gcnew System::Windows::Forms::CheckBox());
			this->tb_enroll_name = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->lb_enroll_return_coverage = (gcnew System::Windows::Forms::Label());
			this->tb_enroll_image_quality = (gcnew System::Windows::Forms::TextBox());
			this->tb_enroll_return_coverage = (gcnew System::Windows::Forms::TextBox());
			this->tb_enroll_fingerprint_area = (gcnew System::Windows::Forms::TextBox());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->lb_enroll_fingerprint_area = (gcnew System::Windows::Forms::Label());
			this->lb_enroll_count = (gcnew System::Windows::Forms::Label());
			this->lb_enroll_image_quality = (gcnew System::Windows::Forms::Label());
			this->tb_enroll_count = (gcnew System::Windows::Forms::TextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->lb_message = (gcnew System::Windows::Forms::Label());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->lb_verify_fingerprint_area = (gcnew System::Windows::Forms::Label());
			this->cb_verify_con = (gcnew System::Windows::Forms::CheckBox());
			this->lb_verify_image_quality = (gcnew System::Windows::Forms::Label());
			this->cb_verify_preview = (gcnew System::Windows::Forms::CheckBox());
			this->tb_verify_fingerprint_area = (gcnew System::Windows::Forms::TextBox());
			this->tb_verify_image_quality = (gcnew System::Windows::Forms::TextBox());
			this->pb_gif = (gcnew System::Windows::Forms::PictureBox());
			this->pb_png = (gcnew System::Windows::Forms::PictureBox());
			this->capture_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->lb_open_parameter = (gcnew System::Windows::Forms::Label());
			this->lb_enroll_parameter = (gcnew System::Windows::Forms::Label());
			this->lb_open_colon = (gcnew System::Windows::Forms::Label());
			this->lb_open_information = (gcnew System::Windows::Forms::Label());
			this->lb_enroll_colon = (gcnew System::Windows::Forms::Label());
			this->lb_enroll_information = (gcnew System::Windows::Forms::Label());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->cb_501 = (gcnew System::Windows::Forms::CheckBox());
			this->cb_301c = (gcnew System::Windows::Forms::CheckBox());
			this->cb_301b = (gcnew System::Windows::Forms::CheckBox());
			this->lb_filename = (gcnew System::Windows::Forms::Label());
			this->pb_showFile = (gcnew System::Windows::Forms::PictureBox());
			this->listView = (gcnew System::Windows::Forms::ListView());
			this->columnHeader1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->btnChooseFile = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox6->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_gif))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_png))->BeginInit();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_showFile))->BeginInit();
			this->SuspendLayout();
			// 
			// btnOpen_device
			// 
			this->btnOpen_device->BackColor = System::Drawing::SystemColors::Control;
			this->btnOpen_device->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnOpen_device->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnOpen_device->Location = System::Drawing::Point(6, 31);
			this->btnOpen_device->Name = L"btnOpen_device";
			this->btnOpen_device->Size = System::Drawing::Size(120, 40);
			this->btnOpen_device->TabIndex = 0;
			this->btnOpen_device->Text = L"Open Device";
			this->btnOpen_device->UseVisualStyleBackColor = true;
			this->btnOpen_device->Click += gcnew System::EventHandler(this, &PBGUI::btnOpen_device_Click);
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::SystemColors::Control;
			this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox1->Cursor = System::Windows::Forms::Cursors::Arrow;
			this->pictureBox1->Location = System::Drawing::Point(38, 28);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(180, 180);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox1->TabIndex = 2;
			this->pictureBox1->TabStop = false;
			// 
			// btnRealTime
			// 
			this->btnRealTime->Enabled = false;
			this->btnRealTime->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnRealTime->Location = System::Drawing::Point(132, 82);
			this->btnRealTime->Name = L"btnRealTime";
			this->btnRealTime->Size = System::Drawing::Size(120, 40);
			this->btnRealTime->TabIndex = 3;
			this->btnRealTime->Text = L"RealTime";
			this->btnRealTime->UseVisualStyleBackColor = true;
			this->btnRealTime->Click += gcnew System::EventHandler(this, &PBGUI::btnRealTime_Click);
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::White;
			this->label1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label1->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->ImageAlign = System::Drawing::ContentAlignment::TopCenter;
			this->label1->ImageIndex = 16;
			this->label1->Location = System::Drawing::Point(9, 10);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(454, 257);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Welcome to use fingerprint tool";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->btnSave);
			this->groupBox1->Controls->Add(this->label_quality);
			this->groupBox1->Controls->Add(this->label_area);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->pictureBox1);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(469, 1);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(255, 266);
			this->groupBox1->TabIndex = 5;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Show Image";
			// 
			// btnSave
			// 
			this->btnSave->Enabled = false;
			this->btnSave->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->btnSave->Location = System::Drawing::Point(18, 216);
			this->btnSave->Name = L"btnSave";
			this->btnSave->Size = System::Drawing::Size(100, 40);
			this->btnSave->TabIndex = 7;
			this->btnSave->Text = L"Save";
			this->btnSave->UseVisualStyleBackColor = true;
			this->btnSave->Click += gcnew System::EventHandler(this, &PBGUI::btnSave_Click);
			// 
			// label_quality
			// 
			this->label_quality->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_quality->Location = System::Drawing::Point(207, 216);
			this->label_quality->Name = L"label_quality";
			this->label_quality->Size = System::Drawing::Size(42, 18);
			this->label_quality->TabIndex = 5;
			this->label_quality->Text = L"  0";
			this->label_quality->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label_area
			// 
			this->label_area->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_area->Location = System::Drawing::Point(207, 239);
			this->label_area->Name = L"label_area";
			this->label_area->Size = System::Drawing::Size(45, 18);
			this->label_area->TabIndex = 6;
			this->label_area->Text = L"  0";
			this->label_area->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(132, 239);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(80, 18);
			this->label3->TabIndex = 4;
			this->label3->Text = L"Area    :";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(132, 216);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(80, 18);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Quality :";
			// 
			// groupBox2
			// 
			this->groupBox2->BackColor = System::Drawing::SystemColors::Control;
			this->groupBox2->Controls->Add(this->btnCapture);
			this->groupBox2->Controls->Add(this->btnDeleteAll);
			this->groupBox2->Controls->Add(this->btnVerify);
			this->groupBox2->Controls->Add(this->btnEnroll);
			this->groupBox2->Controls->Add(this->btnCancel);
			this->groupBox2->Controls->Add(this->btnOpen_device);
			this->groupBox2->Controls->Add(this->btnRealTime);
			this->groupBox2->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox2->Location = System::Drawing::Point(8, 274);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(257, 238);
			this->groupBox2->TabIndex = 6;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Main Function";
			// 
			// btnCapture
			// 
			this->btnCapture->Enabled = false;
			this->btnCapture->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnCapture->Location = System::Drawing::Point(6, 82);
			this->btnCapture->Name = L"btnCapture";
			this->btnCapture->Size = System::Drawing::Size(120, 40);
			this->btnCapture->TabIndex = 8;
			this->btnCapture->Text = L"Capture";
			this->btnCapture->UseVisualStyleBackColor = true;
			this->btnCapture->Click += gcnew System::EventHandler(this, &PBGUI::btnCapture_Click);
			// 
			// btnDeleteAll
			// 
			this->btnDeleteAll->Enabled = false;
			this->btnDeleteAll->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->btnDeleteAll->Location = System::Drawing::Point(6, 189);
			this->btnDeleteAll->Name = L"btnDeleteAll";
			this->btnDeleteAll->Size = System::Drawing::Size(120, 40);
			this->btnDeleteAll->TabIndex = 7;
			this->btnDeleteAll->Text = L"Delete All";
			this->btnDeleteAll->UseVisualStyleBackColor = true;
			this->btnDeleteAll->Click += gcnew System::EventHandler(this, &PBGUI::btnDeleteAll_Click);
			// 
			// btnVerify
			// 
			this->btnVerify->Enabled = false;
			this->btnVerify->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->btnVerify->Location = System::Drawing::Point(132, 136);
			this->btnVerify->Name = L"btnVerify";
			this->btnVerify->Size = System::Drawing::Size(120, 40);
			this->btnVerify->TabIndex = 6;
			this->btnVerify->Text = L"Verify";
			this->btnVerify->UseVisualStyleBackColor = true;
			this->btnVerify->Click += gcnew System::EventHandler(this, &PBGUI::btnVerify_Click);
			// 
			// btnEnroll
			// 
			this->btnEnroll->Enabled = false;
			this->btnEnroll->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->btnEnroll->Location = System::Drawing::Point(6, 135);
			this->btnEnroll->Name = L"btnEnroll";
			this->btnEnroll->Size = System::Drawing::Size(120, 40);
			this->btnEnroll->TabIndex = 5;
			this->btnEnroll->Text = L"Enroll";
			this->btnEnroll->UseVisualStyleBackColor = true;
			this->btnEnroll->Click += gcnew System::EventHandler(this, &PBGUI::btnEnroll_Click);
			// 
			// btnCancel
			// 
			this->btnCancel->Enabled = false;
			this->btnCancel->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnCancel->Location = System::Drawing::Point(132, 189);
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size(120, 40);
			this->btnCancel->TabIndex = 4;
			this->btnCancel->Text = L"Cancel";
			this->btnCancel->UseVisualStyleBackColor = true;
			this->btnCancel->Click += gcnew System::EventHandler(this, &PBGUI::btnCancel_Click);
			// 
			// realtime_timer
			// 
			this->realtime_timer->Tick += gcnew System::EventHandler(this, &PBGUI::realtime_timer_Tick);
			// 
			// enroll_timer
			// 
			this->enroll_timer->Tick += gcnew System::EventHandler(this, &PBGUI::enroll_timer_Tick);
			// 
			// verify_timer
			// 
			this->verify_timer->Tick += gcnew System::EventHandler(this, &PBGUI::verify_timer_Tick);
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->cb_enroll_save_image);
			this->groupBox4->Controls->Add(this->cb_enroll_preview);
			this->groupBox4->Controls->Add(this->tb_enroll_name);
			this->groupBox4->Controls->Add(this->label6);
			this->groupBox4->Controls->Add(this->groupBox5);
			this->groupBox4->Controls->Add(this->label4);
			this->groupBox4->Controls->Add(this->label5);
			this->groupBox4->Controls->Add(this->lb_enroll_return_coverage);
			this->groupBox4->Controls->Add(this->tb_enroll_image_quality);
			this->groupBox4->Controls->Add(this->tb_enroll_return_coverage);
			this->groupBox4->Controls->Add(this->tb_enroll_fingerprint_area);
			this->groupBox4->Controls->Add(this->label10);
			this->groupBox4->Controls->Add(this->lb_enroll_fingerprint_area);
			this->groupBox4->Controls->Add(this->lb_enroll_count);
			this->groupBox4->Controls->Add(this->lb_enroll_image_quality);
			this->groupBox4->Controls->Add(this->tb_enroll_count);
			this->groupBox4->Controls->Add(this->label8);
			this->groupBox4->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox4->Location = System::Drawing::Point(271, 274);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(225, 238);
			this->groupBox4->TabIndex = 10;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Enroll Parameter";
			// 
			// cb_enroll_save_image
			// 
			this->cb_enroll_save_image->Enabled = false;
			this->cb_enroll_save_image->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->cb_enroll_save_image->Location = System::Drawing::Point(11, 54);
			this->cb_enroll_save_image->Name = L"cb_enroll_save_image";
			this->cb_enroll_save_image->Size = System::Drawing::Size(181, 23);
			this->cb_enroll_save_image->TabIndex = 17;
			this->cb_enroll_save_image->Text = L"Save enroll image";
			this->cb_enroll_save_image->UseVisualStyleBackColor = true;
			// 
			// cb_enroll_preview
			// 
			this->cb_enroll_preview->Enabled = false;
			this->cb_enroll_preview->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->cb_enroll_preview->Location = System::Drawing::Point(11, 25);
			this->cb_enroll_preview->Name = L"cb_enroll_preview";
			this->cb_enroll_preview->Size = System::Drawing::Size(145, 23);
			this->cb_enroll_preview->TabIndex = 16;
			this->cb_enroll_preview->Text = L"Preview image";
			this->cb_enroll_preview->UseVisualStyleBackColor = true;
			// 
			// tb_enroll_name
			// 
			this->tb_enroll_name->BackColor = System::Drawing::Color::White;
			this->tb_enroll_name->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_enroll_name->Enabled = false;
			this->tb_enroll_name->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->tb_enroll_name->Location = System::Drawing::Point(104, 91);
			this->tb_enroll_name->MaxLength = 10;
			this->tb_enroll_name->Name = L"tb_enroll_name";
			this->tb_enroll_name->ShortcutsEnabled = false;
			this->tb_enroll_name->Size = System::Drawing::Size(110, 21);
			this->tb_enroll_name->TabIndex = 14;
			this->tb_enroll_name->Text = L"0";
			this->tb_enroll_name->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(8, 94);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(88, 18);
			this->label6->TabIndex = 13;
			this->label6->Text = L"Name     :";
			// 
			// groupBox5
			// 
			this->groupBox5->BackColor = System::Drawing::Color::White;
			this->groupBox5->Location = System::Drawing::Point(0, 174);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(225, 2);
			this->groupBox5->TabIndex = 12;
			this->groupBox5->TabStop = false;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(8, 183);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(88, 18);
			this->label4->TabIndex = 0;
			this->label4->Text = L"Quality  :";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(8, 208);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(88, 18);
			this->label5->TabIndex = 1;
			this->label5->Text = L"Area     :";
			// 
			// lb_enroll_return_coverage
			// 
			this->lb_enroll_return_coverage->AutoSize = true;
			this->lb_enroll_return_coverage->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_enroll_return_coverage->Location = System::Drawing::Point(158, 144);
			this->lb_enroll_return_coverage->Name = L"lb_enroll_return_coverage";
			this->lb_enroll_return_coverage->Size = System::Drawing::Size(48, 18);
			this->lb_enroll_return_coverage->TabIndex = 11;
			this->lb_enroll_return_coverage->Text = L"(0-0)";
			// 
			// tb_enroll_image_quality
			// 
			this->tb_enroll_image_quality->BackColor = System::Drawing::Color::White;
			this->tb_enroll_image_quality->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_enroll_image_quality->Enabled = false;
			this->tb_enroll_image_quality->Location = System::Drawing::Point(104, 181);
			this->tb_enroll_image_quality->MaxLength = 3;
			this->tb_enroll_image_quality->Name = L"tb_enroll_image_quality";
			this->tb_enroll_image_quality->Size = System::Drawing::Size(48, 21);
			this->tb_enroll_image_quality->TabIndex = 2;
			this->tb_enroll_image_quality->Text = L"0";
			this->tb_enroll_image_quality->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb_enroll_image_quality->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &PBGUI::tb_enroll_image_quality_KeyPress);
			// 
			// tb_enroll_return_coverage
			// 
			this->tb_enroll_return_coverage->BackColor = System::Drawing::Color::White;
			this->tb_enroll_return_coverage->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_enroll_return_coverage->Enabled = false;
			this->tb_enroll_return_coverage->Location = System::Drawing::Point(104, 143);
			this->tb_enroll_return_coverage->MaxLength = 3;
			this->tb_enroll_return_coverage->Name = L"tb_enroll_return_coverage";
			this->tb_enroll_return_coverage->Size = System::Drawing::Size(48, 21);
			this->tb_enroll_return_coverage->TabIndex = 10;
			this->tb_enroll_return_coverage->Text = L"0";
			this->tb_enroll_return_coverage->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb_enroll_return_coverage->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &PBGUI::tb_enroll_return_coverage_KeyPress);
			// 
			// tb_enroll_fingerprint_area
			// 
			this->tb_enroll_fingerprint_area->BackColor = System::Drawing::Color::White;
			this->tb_enroll_fingerprint_area->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_enroll_fingerprint_area->Enabled = false;
			this->tb_enroll_fingerprint_area->Location = System::Drawing::Point(104, 207);
			this->tb_enroll_fingerprint_area->MaxLength = 2;
			this->tb_enroll_fingerprint_area->Name = L"tb_enroll_fingerprint_area";
			this->tb_enroll_fingerprint_area->Size = System::Drawing::Size(48, 21);
			this->tb_enroll_fingerprint_area->TabIndex = 3;
			this->tb_enroll_fingerprint_area->Text = L"0";
			this->tb_enroll_fingerprint_area->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb_enroll_fingerprint_area->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &PBGUI::tb_enroll_fingerprint_area_KeyPress);
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label10->Location = System::Drawing::Point(8, 144);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(88, 18);
			this->label10->TabIndex = 9;
			this->label10->Text = L"Coverage :";
			// 
			// lb_enroll_fingerprint_area
			// 
			this->lb_enroll_fingerprint_area->AutoSize = true;
			this->lb_enroll_fingerprint_area->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_enroll_fingerprint_area->Location = System::Drawing::Point(158, 208);
			this->lb_enroll_fingerprint_area->Name = L"lb_enroll_fingerprint_area";
			this->lb_enroll_fingerprint_area->Size = System::Drawing::Size(48, 18);
			this->lb_enroll_fingerprint_area->TabIndex = 8;
			this->lb_enroll_fingerprint_area->Text = L"(0-0)";
			// 
			// lb_enroll_count
			// 
			this->lb_enroll_count->AutoSize = true;
			this->lb_enroll_count->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lb_enroll_count->Location = System::Drawing::Point(158, 119);
			this->lb_enroll_count->Name = L"lb_enroll_count";
			this->lb_enroll_count->Size = System::Drawing::Size(48, 18);
			this->lb_enroll_count->TabIndex = 6;
			this->lb_enroll_count->Text = L"(0-0)";
			// 
			// lb_enroll_image_quality
			// 
			this->lb_enroll_image_quality->AutoSize = true;
			this->lb_enroll_image_quality->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_enroll_image_quality->Location = System::Drawing::Point(158, 183);
			this->lb_enroll_image_quality->Name = L"lb_enroll_image_quality";
			this->lb_enroll_image_quality->Size = System::Drawing::Size(48, 18);
			this->lb_enroll_image_quality->TabIndex = 7;
			this->lb_enroll_image_quality->Text = L"(0-0)";
			// 
			// tb_enroll_count
			// 
			this->tb_enroll_count->BackColor = System::Drawing::Color::White;
			this->tb_enroll_count->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_enroll_count->Enabled = false;
			this->tb_enroll_count->Location = System::Drawing::Point(104, 117);
			this->tb_enroll_count->MaxLength = 2;
			this->tb_enroll_count->Name = L"tb_enroll_count";
			this->tb_enroll_count->ShortcutsEnabled = false;
			this->tb_enroll_count->Size = System::Drawing::Size(48, 21);
			this->tb_enroll_count->TabIndex = 5;
			this->tb_enroll_count->Text = L"0";
			this->tb_enroll_count->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb_enroll_count->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &PBGUI::tb_enroll_count_KeyPress);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label8->Location = System::Drawing::Point(8, 119);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(88, 18);
			this->label8->TabIndex = 4;
			this->label8->Text = L"Count    :";
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(796, 21);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(100, 22);
			this->textBox2->TabIndex = 12;
			// 
			// lb_message
			// 
			this->lb_message->BackColor = System::Drawing::Color::White;
			this->lb_message->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lb_message->ForeColor = System::Drawing::Color::DodgerBlue;
			this->lb_message->Location = System::Drawing::Point(41, 24);
			this->lb_message->Name = L"lb_message";
			this->lb_message->Size = System::Drawing::Size(390, 26);
			this->lb_message->TabIndex = 14;
			this->lb_message->Text = L"message";
			this->lb_message->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->lb_message->Visible = false;
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->groupBox3);
			this->groupBox6->Controls->Add(this->label9);
			this->groupBox6->Controls->Add(this->label7);
			this->groupBox6->Controls->Add(this->lb_verify_fingerprint_area);
			this->groupBox6->Controls->Add(this->cb_verify_con);
			this->groupBox6->Controls->Add(this->lb_verify_image_quality);
			this->groupBox6->Controls->Add(this->cb_verify_preview);
			this->groupBox6->Controls->Add(this->tb_verify_fingerprint_area);
			this->groupBox6->Controls->Add(this->tb_verify_image_quality);
			this->groupBox6->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox6->Location = System::Drawing::Point(501, 274);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(225, 147);
			this->groupBox6->TabIndex = 21;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"Verify Parameter";
			// 
			// groupBox3
			// 
			this->groupBox3->BackColor = System::Drawing::Color::White;
			this->groupBox3->Location = System::Drawing::Point(1, 84);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(225, 2);
			this->groupBox3->TabIndex = 13;
			this->groupBox3->TabStop = false;
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label9->Location = System::Drawing::Point(9, 121);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(88, 18);
			this->label9->TabIndex = 18;
			this->label9->Text = L"Area     :";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label7->Location = System::Drawing::Point(9, 94);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(88, 18);
			this->label7->TabIndex = 17;
			this->label7->Text = L"Quality  :";
			// 
			// lb_verify_fingerprint_area
			// 
			this->lb_verify_fingerprint_area->AutoSize = true;
			this->lb_verify_fingerprint_area->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_verify_fingerprint_area->Location = System::Drawing::Point(157, 120);
			this->lb_verify_fingerprint_area->Name = L"lb_verify_fingerprint_area";
			this->lb_verify_fingerprint_area->Size = System::Drawing::Size(48, 18);
			this->lb_verify_fingerprint_area->TabIndex = 22;
			this->lb_verify_fingerprint_area->Text = L"(0-0)";
			// 
			// cb_verify_con
			// 
			this->cb_verify_con->AutoSize = true;
			this->cb_verify_con->Enabled = false;
			this->cb_verify_con->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->cb_verify_con->Location = System::Drawing::Point(10, 55);
			this->cb_verify_con->Name = L"cb_verify_con";
			this->cb_verify_con->Size = System::Drawing::Size(181, 23);
			this->cb_verify_con->TabIndex = 18;
			this->cb_verify_con->Text = L"Continuous verify";
			this->cb_verify_con->UseVisualStyleBackColor = true;
			// 
			// lb_verify_image_quality
			// 
			this->lb_verify_image_quality->AutoSize = true;
			this->lb_verify_image_quality->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_verify_image_quality->Location = System::Drawing::Point(157, 94);
			this->lb_verify_image_quality->Name = L"lb_verify_image_quality";
			this->lb_verify_image_quality->Size = System::Drawing::Size(48, 18);
			this->lb_verify_image_quality->TabIndex = 21;
			this->lb_verify_image_quality->Text = L"(0-0)";
			// 
			// cb_verify_preview
			// 
			this->cb_verify_preview->AutoSize = true;
			this->cb_verify_preview->Enabled = false;
			this->cb_verify_preview->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->cb_verify_preview->Location = System::Drawing::Point(10, 25);
			this->cb_verify_preview->Name = L"cb_verify_preview";
			this->cb_verify_preview->Size = System::Drawing::Size(145, 23);
			this->cb_verify_preview->TabIndex = 17;
			this->cb_verify_preview->Text = L"Preview image";
			this->cb_verify_preview->UseVisualStyleBackColor = true;
			// 
			// tb_verify_fingerprint_area
			// 
			this->tb_verify_fingerprint_area->BackColor = System::Drawing::Color::White;
			this->tb_verify_fingerprint_area->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_verify_fingerprint_area->Enabled = false;
			this->tb_verify_fingerprint_area->Location = System::Drawing::Point(103, 120);
			this->tb_verify_fingerprint_area->MaxLength = 2;
			this->tb_verify_fingerprint_area->Name = L"tb_verify_fingerprint_area";
			this->tb_verify_fingerprint_area->Size = System::Drawing::Size(48, 21);
			this->tb_verify_fingerprint_area->TabIndex = 20;
			this->tb_verify_fingerprint_area->Text = L"0";
			this->tb_verify_fingerprint_area->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb_verify_fingerprint_area->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &PBGUI::tb_verify_fingerprint_area_KeyPress);
			// 
			// tb_verify_image_quality
			// 
			this->tb_verify_image_quality->BackColor = System::Drawing::Color::White;
			this->tb_verify_image_quality->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_verify_image_quality->Enabled = false;
			this->tb_verify_image_quality->Location = System::Drawing::Point(103, 94);
			this->tb_verify_image_quality->MaxLength = 3;
			this->tb_verify_image_quality->Name = L"tb_verify_image_quality";
			this->tb_verify_image_quality->Size = System::Drawing::Size(48, 21);
			this->tb_verify_image_quality->TabIndex = 19;
			this->tb_verify_image_quality->Text = L"0";
			this->tb_verify_image_quality->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb_verify_image_quality->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &PBGUI::tb_verify_image_quality_KeyPress);
			// 
			// pb_gif
			// 
			this->pb_gif->BackColor = System::Drawing::Color::White;
			this->pb_gif->Location = System::Drawing::Point(164, 62);
			this->pb_gif->Name = L"pb_gif";
			this->pb_gif->Size = System::Drawing::Size(140, 140);
			this->pb_gif->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pb_gif->TabIndex = 22;
			this->pb_gif->TabStop = false;
			this->pb_gif->Visible = false;
			// 
			// pb_png
			// 
			this->pb_png->BackColor = System::Drawing::Color::Transparent;
			this->pb_png->Location = System::Drawing::Point(310, 34);
			this->pb_png->Name = L"pb_png";
			this->pb_png->Size = System::Drawing::Size(140, 140);
			this->pb_png->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pb_png->TabIndex = 23;
			this->pb_png->TabStop = false;
			this->pb_png->Visible = false;
			// 
			// capture_timer
			// 
			this->capture_timer->Tick += gcnew System::EventHandler(this, &PBGUI::capture_timer_Tick);
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->tabControl1->Location = System::Drawing::Point(9, 9);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(740, 548);
			this->tabControl1->TabIndex = 24;
			this->tabControl1->SelectedIndexChanged += gcnew System::EventHandler(this, &PBGUI::tabControl_SelectedIndexChanged);
			// 
			// tabPage1
			// 
			this->tabPage1->BackColor = System::Drawing::SystemColors::Control;
			this->tabPage1->Controls->Add(this->pb_png);
			this->tabPage1->Controls->Add(this->lb_open_parameter);
			this->tabPage1->Controls->Add(this->lb_enroll_parameter);
			this->tabPage1->Controls->Add(this->lb_open_colon);
			this->tabPage1->Controls->Add(this->pb_gif);
			this->tabPage1->Controls->Add(this->groupBox1);
			this->tabPage1->Controls->Add(this->lb_open_information);
			this->tabPage1->Controls->Add(this->lb_enroll_colon);
			this->tabPage1->Controls->Add(this->groupBox2);
			this->tabPage1->Controls->Add(this->lb_enroll_information);
			this->tabPage1->Controls->Add(this->groupBox6);
			this->tabPage1->Controls->Add(this->lb_message);
			this->tabPage1->Controls->Add(this->label1);
			this->tabPage1->Controls->Add(this->groupBox4);
			this->tabPage1->Location = System::Drawing::Point(4, 29);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(732, 515);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Main";
			// 
			// lb_open_parameter
			// 
			this->lb_open_parameter->AutoSize = true;
			this->lb_open_parameter->BackColor = System::Drawing::Color::White;
			this->lb_open_parameter->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_open_parameter->Location = System::Drawing::Point(324, 154);
			this->lb_open_parameter->Name = L"lb_open_parameter";
			this->lb_open_parameter->Size = System::Drawing::Size(79, 20);
			this->lb_open_parameter->TabIndex = 27;
			this->lb_open_parameter->Text = L"label11";
			this->lb_open_parameter->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->lb_open_parameter->Visible = false;
			// 
			// lb_enroll_parameter
			// 
			this->lb_enroll_parameter->AutoSize = true;
			this->lb_enroll_parameter->BackColor = System::Drawing::Color::White;
			this->lb_enroll_parameter->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_enroll_parameter->Location = System::Drawing::Point(187, 128);
			this->lb_enroll_parameter->Name = L"lb_enroll_parameter";
			this->lb_enroll_parameter->Size = System::Drawing::Size(99, 20);
			this->lb_enroll_parameter->TabIndex = 27;
			this->lb_enroll_parameter->Text = L"parameter";
			this->lb_enroll_parameter->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->lb_enroll_parameter->Visible = false;
			// 
			// lb_open_colon
			// 
			this->lb_open_colon->AutoSize = true;
			this->lb_open_colon->BackColor = System::Drawing::Color::White;
			this->lb_open_colon->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lb_open_colon->Location = System::Drawing::Point(299, 154);
			this->lb_open_colon->Name = L"lb_open_colon";
			this->lb_open_colon->Size = System::Drawing::Size(19, 20);
			this->lb_open_colon->TabIndex = 26;
			this->lb_open_colon->Text = L":";
			this->lb_open_colon->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			this->lb_open_colon->Visible = false;
			// 
			// lb_open_information
			// 
			this->lb_open_information->AutoSize = true;
			this->lb_open_information->BackColor = System::Drawing::Color::White;
			this->lb_open_information->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_open_information->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->lb_open_information->Location = System::Drawing::Point(77, 154);
			this->lb_open_information->Name = L"lb_open_information";
			this->lb_open_information->Size = System::Drawing::Size(209, 20);
			this->lb_open_information->TabIndex = 25;
			this->lb_open_information->Text = L"Max fingerprint area";
			this->lb_open_information->Visible = false;
			// 
			// lb_enroll_colon
			// 
			this->lb_enroll_colon->AutoSize = true;
			this->lb_enroll_colon->BackColor = System::Drawing::Color::White;
			this->lb_enroll_colon->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lb_enroll_colon->Location = System::Drawing::Point(170, 128);
			this->lb_enroll_colon->Name = L"lb_enroll_colon";
			this->lb_enroll_colon->Size = System::Drawing::Size(19, 20);
			this->lb_enroll_colon->TabIndex = 26;
			this->lb_enroll_colon->Text = L":";
			this->lb_enroll_colon->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			this->lb_enroll_colon->Visible = false;
			// 
			// lb_enroll_information
			// 
			this->lb_enroll_information->AutoSize = true;
			this->lb_enroll_information->BackColor = System::Drawing::Color::White;
			this->lb_enroll_information->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_enroll_information->Location = System::Drawing::Point(42, 128);
			this->lb_enroll_information->Name = L"lb_enroll_information";
			this->lb_enroll_information->Size = System::Drawing::Size(129, 20);
			this->lb_enroll_information->TabIndex = 25;
			this->lb_enroll_information->Text = L"1) File Name";
			this->lb_enroll_information->Visible = false;
			// 
			// tabPage2
			// 
			this->tabPage2->BackColor = System::Drawing::SystemColors::Control;
			this->tabPage2->Controls->Add(this->cb_501);
			this->tabPage2->Controls->Add(this->cb_301c);
			this->tabPage2->Controls->Add(this->cb_301b);
			this->tabPage2->Controls->Add(this->lb_filename);
			this->tabPage2->Controls->Add(this->pb_showFile);
			this->tabPage2->Controls->Add(this->listView);
			this->tabPage2->Controls->Add(this->label11);
			this->tabPage2->Controls->Add(this->btnChooseFile);
			this->tabPage2->Location = System::Drawing::Point(4, 29);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(732, 515);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"Gallery";
			// 
			// cb_501
			// 
			this->cb_501->AutoSize = true;
			this->cb_501->Location = System::Drawing::Point(184, 70);
			this->cb_501->Name = L"cb_501";
			this->cb_501->Size = System::Drawing::Size(58, 24);
			this->cb_501->TabIndex = 7;
			this->cb_501->Text = L"501";
			this->cb_501->UseVisualStyleBackColor = true;
			this->cb_501->CheckStateChanged += gcnew System::EventHandler(this, &PBGUI::cb_501_CheckStateChanged);
			// 
			// cb_301c
			// 
			this->cb_301c->AutoSize = true;
			this->cb_301c->Location = System::Drawing::Point(99, 70);
			this->cb_301c->Name = L"cb_301c";
			this->cb_301c->Size = System::Drawing::Size(68, 24);
			this->cb_301c->TabIndex = 6;
			this->cb_301c->Text = L"301C";
			this->cb_301c->UseVisualStyleBackColor = true;
			this->cb_301c->CheckStateChanged += gcnew System::EventHandler(this, &PBGUI::cb_301c_CheckStateChanged);
			// 
			// cb_301b
			// 
			this->cb_301b->AutoSize = true;
			this->cb_301b->Location = System::Drawing::Point(13, 70);
			this->cb_301b->Name = L"cb_301b";
			this->cb_301b->Size = System::Drawing::Size(68, 24);
			this->cb_301b->TabIndex = 5;
			this->cb_301b->Text = L"301B";
			this->cb_301b->UseVisualStyleBackColor = true;
			this->cb_301b->CheckStateChanged += gcnew System::EventHandler(this, &PBGUI::cb_301b_CheckStateChanged);
			// 
			// lb_filename
			// 
			this->lb_filename->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lb_filename->Location = System::Drawing::Point(302, 441);
			this->lb_filename->Name = L"lb_filename";
			this->lb_filename->Size = System::Drawing::Size(395, 41);
			this->lb_filename->TabIndex = 4;
			this->lb_filename->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// pb_showFile
			// 
			this->pb_showFile->Location = System::Drawing::Point(360, 141);
			this->pb_showFile->Name = L"pb_showFile";
			this->pb_showFile->Size = System::Drawing::Size(285, 285);
			this->pb_showFile->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pb_showFile->TabIndex = 3;
			this->pb_showFile->TabStop = false;
			// 
			// listView
			// 
			this->listView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(1) { this->columnHeader1 });
			this->listView->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->listView->FullRowSelect = true;
			this->listView->GridLines = true;
			this->listView->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::Nonclickable;
			this->listView->Location = System::Drawing::Point(4, 100);
			this->listView->Name = L"listView";
			this->listView->Size = System::Drawing::Size(250, 428);
			this->listView->TabIndex = 2;
			this->listView->UseCompatibleStateImageBehavior = false;
			this->listView->View = System::Windows::Forms::View::Details;
			this->listView->SelectedIndexChanged += gcnew System::EventHandler(this, &PBGUI::lv_SelectedIndexChanged);
			// 
			// columnHeader1
			// 
			this->columnHeader1->Text = L"File Name";
			this->columnHeader1->Width = 250;
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label11->Location = System::Drawing::Point(143, 28);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(239, 20);
			this->label11->TabIndex = 1;
			this->label11->Text = L"Choice image to show it";
			// 
			// btnChooseFile
			// 
			this->btnChooseFile->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->btnChooseFile->Location = System::Drawing::Point(20, 18);
			this->btnChooseFile->Name = L"btnChooseFile";
			this->btnChooseFile->Size = System::Drawing::Size(106, 40);
			this->btnChooseFile->TabIndex = 0;
			this->btnChooseFile->Text = L"Choose";
			this->btnChooseFile->UseVisualStyleBackColor = true;
			this->btnChooseFile->Click += gcnew System::EventHandler(this, &PBGUI::btnChooseFile_Click);
			// 
			// PBGUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(754, 554);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->textBox2);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"PBGUI";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"PBLibrary_AP";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &PBGUI::PBGUI_FormClosing);
			this->Load += gcnew System::EventHandler(this, &PBGUI::PBGUI_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->groupBox6->ResumeLayout(false);
			this->groupBox6->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_gif))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_png))->EndInit();
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_showFile))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}

#pragma endregion

	private: System::Void btnOpen_device_Click(System::Object^  sender, System::EventArgs^  e) {
				 printf("open device click...\n\n");	
				 label1->TextAlign = ContentAlignment::MiddleCenter;
				 label1->Text = "Searching device ....";
				 label1->Refresh();	

				 deviceNumber = -1;
		 
				 for (int i = 0; i < 10; i++) {
					 video_capture.open(i);					

					 dWidth = video_capture.get(CV_CAP_PROP_FRAME_WIDTH);
					 dHeight = video_capture.get(CV_CAP_PROP_FRAME_HEIGHT);	

					 printf("Width = %d, Height = %d.\n", (int)dWidth, (int)dHeight);
					 
					 if ((int)dWidth == 120 && (int)dHeight == 120) {
						 printf("VideoCapture(%d) is open.\n", i);	

						 set_adjust_parameter(dWidth, dHeight);

						 setSensorType(dWidth, dHeight);

						 control_item_enable(true);
						 btnOpen_device->Enabled = false;
						 btnSave->Enabled = false;

						 deviceNumber = i;

						 break;
					 }
					 else if ((int)dWidth == 176 && (int)dHeight == 176) {
						 printf("VideoCapture(%d) is open.\n", i);					
						 set_adjust_parameter(dWidth, dHeight);

						 control_item_enable(true);
						 btnOpen_device->Enabled = false;	
						 btnSave->Enabled = false;

						 deviceNumber = i;

						 setSensorType(dWidth, dHeight);

						 break;
					 }
					 else if ((int)dWidth == 128 && (int)dHeight == 128) {
						 printf("VideoCapture(%d) is open.\n", i);
						 set_adjust_parameter(dWidth, dHeight);

						 setSensorType(dWidth, dHeight);

						 control_item_enable(true);
						 btnOpen_device->Enabled = false;
						 btnSave->Enabled = false;

						 deviceNumber = i;

						 setSensorType(dWidth, dHeight);

						 break;
					 }
				 }

				 if (deviceNumber == -1) {
					 label1->Refresh();
					 label1->Text = "Not detect fingerprint camera device,\n";
					 label1->Text += "please connect the device.";
				 }
				 else {

					 label1->Text = "Fingerprint camera device is connected.\n\n";
					 label1->Text += "Sensor type is " + sensorType + " (" + dWidth + " x " + dHeight + ")";
					 label1->Text += "\n\n\n";

					 lb_open_message_visible(true);
					 lb_open_parameter->Text = "" + max_enrollment_samples + "\n";
					 lb_open_parameter->Text += "100\n";
					 lb_open_parameter->Text += "" + max_fingerprint_area;

					 textBox2->Focus();

					 tb_enroll_name->Text = "Your Name";
					 tb_enroll_count->Text = Convert::ToString(set_enrollment_samples);
					 tb_enroll_image_quality->Text = Convert::ToString(enrollment_image_quality);
					 tb_enroll_fingerprint_area->Text = Convert::ToString(enrollment_fingerprint_area);
					 tb_enroll_return_coverage->Text = "0";

					 lb_enroll_count->Text = "(0-" + max_enrollment_samples + ")";
					 lb_enroll_image_quality->Text = "(0-100)";
					 lb_enroll_fingerprint_area->Text = "(0-" + max_fingerprint_area + ")";
					 lb_enroll_return_coverage->Text = "(0-100)";

					 tb_verify_image_quality->Text = Convert::ToString(verify_image_quality);
					 tb_verify_fingerprint_area->Text = Convert::ToString(verify_fingerprint_area);
					 lb_verify_image_quality->Text = "(0-100)";
					 lb_verify_fingerprint_area->Text = "(0-" + max_fingerprint_area + ")";

					 /*
					 strcpy(file_path_string, app_path);
					 strcat(file_path_string, "\\");
					 strcat(file_path_string, (char*)(void*)Marshal::StringToHGlobalAnsi(sensorType));
					 strcat(file_path_string, "_template");
					 sprintf(databaseFile, file_path_string);
					 _mkdir(databaseFile);
					 */

					 template_type_path = template_path  + sensorType;

					 if (!System::IO::Directory::Exists(template_type_path))
					 {
						 System::IO::Directory::CreateDirectory(template_type_path);
					 }
				 }
	}
	private: System::Void btnCapture_Click(System::Object^  sender, System::EventArgs^  e) {
				 printf("btnCapture_Click...\n");

				 label1->ForeColor = Color::Black;
				 pb_gif->Visible = false;

				 if (btnCapture->Text == "Capture") {
					 if (capture = cvCreateCameraCapture(deviceNumber)) {
						 printf("Press finger to capture fingerprint on device\n");
						 label1->TextAlign = ContentAlignment::MiddleCenter;
						 label1->Text = "Place finger to capture fingerprint \n\non device.";
						 label1->Refresh();

						 lb_open_message_visible(false);
						 lb_enroll_message_visible(false);
						 lb_message->Visible = false;

						 control_item_enable(false);
						 btnCapture->Text = "Capturing";
						 btnCancel->Enabled = true;

						 quality_chk_init();

						 start_capture = true;
						 get_frame = true;
						 pictureBox1->Image = nullptr;

						 capture_timer->Interval = 33;
						 capture_timer->Start();
					 }
					 else {
						 not_detect_device();
					 }
				 }
	}
		
	private: System::Void capture_timer_Tick(System::Object^  sender, System::EventArgs^  e) {
				 pb_image_t* image = 0;
				 uint8_t image_quality = 0;
				 uint16_t fingerprint_area = 0;

				 if (frame = cvQueryFrame(capture)) {
					 if (get_frame) {
						 frame = NULL;
						 get_frame = false;
					 }
					 else {
						 int size = (frame->imageSize) / 3;

						 uchar* old_data = (uchar*)frame->imageData;
						 uchar* new_data = new uchar[size];

						 for (int i = 0; i < size; i++) {
							 new_data[i] = old_data[i * 3];
						 }

						 image = pb_image_create_mr(frame->width, frame->height, 508, 508, new_data, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);
						 if (!image) {
							 printf("ERROR: pb_image_create_mr() failed.\n");
						 }

						 quality_chk(image, &image_quality, &fingerprint_area);

						 if (image_quality > 50 && fingerprint_area > 24) {
							 label_quality->Text = "  " + image_quality;
							 label_area->Text = "  " + fingerprint_area;

							 //System::Drawing::Graphics^ graphics = pictureBox1->CreateGraphics();
							 pictureBox1->Image = gcnew Bitmap(pictureBox1->Width, pictureBox1->Height);
							 Graphics^ graphics = Graphics::FromImage(pictureBox1->Image);
							 System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(frame->width, frame->height, frame->widthStep,
								 System::Drawing::Imaging::PixelFormat::Format24bppRgb, (System::IntPtr)frame->imageData);
							 System::Drawing::RectangleF rect(0, 0, pictureBox1->Width, pictureBox1->Height);
							 graphics->DrawImage(bmp, rect);

							 pb_gif->Visible = true;
							 pb_png->Visible = true;
							 pb_png->Parent = pb_gif;
							 pb_png->Location = System::Drawing::Point(0, 0);
							 pb_png->Image = enrollProgressImage[enrollProgressImage->Length - 1];

							 capture_timer->Stop();

							 lb_message->Visible = true;
							 lb_message->Text = "Capture Successfully";

							 label1->TextAlign = ContentAlignment::BottomCenter;
							 label1->Text  = "You can click the save button to\n";
							 label1->Text += "save the fingerprint image.\n";
							 control_item_enable(true);
							 btnOpen_device->Enabled = false;
							 btnCapture->Text = "Capture";
							 start_capture = false;
							 quality_chk_deinit();
						 }
					 }
				 }
	}
	
	private: System::Void btnRealTime_Click(System::Object^  sender, System::EventArgs^  e) {
				 printf("btnRealTime_Click...\n");	

				 label1->ForeColor = Color::Black;
				 pb_gif->Visible = false;
				 lb_message->Visible = false;

				 if (btnRealTime->Text == "RealTime") {				
					 if (capture = cvCreateCameraCapture(deviceNumber)) {
						 printf("Press finger to show real-time image on device\n");	
						 label1->TextAlign = ContentAlignment::MiddleCenter;
						 label1->Text = "Press finger to show real-time\n\n image on device.";
						 label1->Refresh();

						 lb_open_message_visible(false);
						 lb_enroll_message_visible(false);

						 control_item_enable(false);
						 btnRealTime->Enabled = true;
						 btnRealTime->Text = "Stop";

						 quality_chk_init();

						 get_frame = true;
						 pictureBox1->Image = nullptr;

						 realtime_timer->Interval = 33;
						 realtime_timer->Start();

						 start_realtime = true;
					 }
					 else {						
						 not_detect_device();
					 }					 													 
				 }
				 else {		 
					 realtime_timer->Stop();

					 label1->TextAlign = ContentAlignment::MiddleCenter;
					 //label1->Text = "Welcome to use fingerprint tool";
					 label1->Text = "You can click the save button to\n\n";
					 label1->Text += "save the fingerprint image.";

					 control_item_enable(true);
					 btnOpen_device->Enabled = false;
					 btnRealTime->Text = "RealTime";

					 start_realtime = false;
					 //pictureBox1->Image = logoImage;
				 }
	}
	private: System::Void realtime_timer_Tick(System::Object^  sender, System::EventArgs^  e) {
			    pb_image_t* image = 0;
				uint8_t image_quality;
				uint16_t fingerprint_area;

				if (frame = cvQueryFrame(capture)) {
					if (get_frame) {
						frame = NULL;
						get_frame = false;
					}
					else {

						int size = (frame->imageSize) / 3;

						uchar* old_data = (uchar*)frame->imageData;
						uchar* new_data = new uchar[size];

						for (int i = 0; i < size; i++) {
							new_data[i] = old_data[i * 3];
						}

						image = pb_image_create_mr(frame->width, frame->height, 508, 508, new_data, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);
						if (!image) {
							printf("ERROR: pb_image_create() failed.\n");
						}

						quality_chk(image, &image_quality, &fingerprint_area);

						label_quality->Text = "  " + image_quality;
						label_area->Text = "  " + fingerprint_area;
						printf("Quality => %d, Coverage => %d\n\n", image_quality, fingerprint_area);

						pictureBox1->Image = gcnew Bitmap(pictureBox1->Width, pictureBox1->Height);
						Graphics^ graphics = Graphics::FromImage(pictureBox1->Image);
						System::Drawing::Bitmap^ b = gcnew System::Drawing::Bitmap(frame->width, frame->height, frame->widthStep,
							System::Drawing::Imaging::PixelFormat::Format24bppRgb, (System::IntPtr)frame->imageData);
						System::Drawing::RectangleF rect(0, 0, pictureBox1->Width, pictureBox1->Height);
						graphics->DrawImage(b, rect);
					}
				}
				else {
					not_detect_device();
				}
				
	}
	
	private: System::Void btnCancel_Click(System::Object^  sender, System::EventArgs^  e) {				

				 pictureBox1->Image = logoImage;

				 lb_message->Visible = false;

				 lb_open_message_visible(false);
				 lb_enroll_message_visible(false);

				 label1->TextAlign = ContentAlignment::MiddleCenter;
				 label1->Text = "Welcome to use fingerprint tool";

				 textBox2->Focus();

				 label_quality->Text = "  " + 0;
				 label_area->Text = "  " + 0;

				 if (start_capture) {
					 capture_timer->Stop();
					 start_capture = false;

					 control_item_enable(true);
					 btnOpen_device->Enabled = false;
					 btnSave->Enabled = false;
					 btnCapture->Text = "Capture";
				 }
				 else if (start_enroll) {
					 enroll_timer->Stop();
					 start_enroll = false;

					 control_item_enable(true);
					 btnOpen_device->Enabled = false;
					 btnSave->Enabled = false;
					 btnEnroll->Text = "Enroll";

					 pb_gif->Visible = false;

					 if (enroll_save_image) {
						 char imagePath[200];
						 char string[200];
						 strcpy(string, (char*)(void*)Marshal::StringToHGlobalAnsi(image_type_path));
						 strcat(string, "\\");
						 strcat(string, date_buf);
						 strcat(string, "_");
						 strcat(string, filename);
						 strcat(string, "_");
						 strcat(string, finger_idx_buf);
						 strcat(string, "_%d.png");

						 for (int i = 1; i <= max_samples; i++)
						 {
							 sprintf(imagePath, string, i);
							 remove(imagePath);
						 }
					 }
				 }
				 else if (start_verify) {
					 verify_timer->Stop();
					 start_verify = false;

					 control_item_enable(true);
					 btnOpen_device->Enabled = false;
					 btnSave->Enabled = false;
					 btnVerify->Text = "Verify";

					 lb_message->Visible = false;
					 pb_gif->Visible = false;
				 }
				 else {
					 video_capture.release();

					 control_item_enable(false);
					 btnOpen_device->Enabled = true;					 

					 tb_enroll_name->Text = "0";
					 tb_enroll_count->Text = "0";
					 tb_enroll_image_quality->Text = "0";
					 tb_enroll_fingerprint_area->Text = "0";
					 tb_enroll_return_coverage->Text = "0";

					 lb_enroll_count->Text = "(0-0)";
					 lb_enroll_image_quality->Text = "(0-0)";
					 lb_enroll_fingerprint_area->Text = "(0-0)";
					 lb_enroll_return_coverage->Text = "(0-0)";

					 tb_verify_image_quality->Text = "0";
					 tb_verify_fingerprint_area->Text = "0";
					 lb_verify_image_quality->Text = "(0-0)";
					 lb_verify_fingerprint_area->Text = "(0-0)";

					 lb_message->Visible = false;
					 pb_gif->Visible = false;

					 sensorType = "";
				 }
	}
	private: System::Void PBGUI_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 realtime_timer->Enabled = false;
				 capture_timer->Enabled = false;
				 enroll_timer->Enabled = false;
				 verify_timer->Enabled = false;

				 cvReleaseCapture(&capture);
	}
	private: System::Void btnEnroll_Click(System::Object^  sender, System::EventArgs^  e) {
				 printf("btnEnroll_Click...\n\n");

				 pb_gif->Visible = false;

				 label_quality->Text = "  " + 0;
				 label_area->Text = "  " + 0;

				 textBox2->Focus();

				 if (cb_enroll_preview->CheckState == CheckState::Checked) {
					 enroll_preview = true;
					 pictureBox1->Image = nullptr;
				 }
				 else {
					 enroll_preview = false;
					 pictureBox1->Image = logoImage;
				 }
				 
				 if (cb_enroll_save_image->CheckState == CheckState::Checked) {
					 enroll_save_image = true;
					 /*
					 strcpy(file_path_string, app_path);
					 strcat(file_path_string, "\\");
					 strcat(file_path_string, (char*)(void*)Marshal::StringToHGlobalAnsi(sensorType));
					 strcat(file_path_string, "_image");
					 sprintf(imageFile, file_path_string);
					 _mkdir(imageFile);
					 */
					 image_type_path = image_path + sensorType;
					 if (!System::IO::Directory::Exists(image_type_path))
					 {
						 System::IO::Directory::CreateDirectory(image_type_path);
					 }

					 cb_enroll_preview->CheckState = CheckState::Checked;
					 enroll_preview = true;
					 pictureBox1->Image = nullptr;
				 }
				 else {
					 enroll_save_image = false;
				 }

				 if (btnEnroll->Text == "Enroll") {
					 if (capture = cvCreateCameraCapture(deviceNumber)) {
						 lb_message->Visible = true;
						 lb_message->Text = "Enrolling ....";

						 printf("Place finger to enroll fingerprint on device.\n");

						 label1->ForeColor = Color::Black;
						 label1->TextAlign = ContentAlignment::BottomCenter;
						 label1->Text = "Place finger on device, lift it off, \n then repeat.\n";
						 //label1->Text += "Move finger upwards/downwards slightly between attempts.\n";
						 //label1->Refresh();

						 lb_open_message_visible(false);
						 lb_enroll_message_visible(false);

						 if (tb_enroll_count->Text == "" || Convert::ToInt32(tb_enroll_count->Text) > max_enrollment_samples) {
							 tb_enroll_count->Text = Convert::ToString(set_enrollment_samples);
						 }

						 if (tb_enroll_image_quality->Text == "" || Convert::ToInt32(tb_enroll_image_quality->Text) > 100) {
							 tb_enroll_image_quality->Text = Convert::ToString(enrollment_image_quality);					
						 }

						 if (tb_enroll_fingerprint_area->Text == "" || Convert::ToInt32(tb_enroll_fingerprint_area->Text) > max_fingerprint_area) {
							 tb_enroll_fingerprint_area->Text = Convert::ToString(enrollment_fingerprint_area);
						 }

						 if (tb_enroll_return_coverage->Text == "" || Convert::ToInt32(tb_enroll_return_coverage->Text) > 100) {
							 tb_enroll_return_coverage->Text = "0";
						 }
						 else if (Convert::ToInt32(tb_enroll_return_coverage->Text) > 0){
							 tb_enroll_count->Text = "32";
						 }

						 control_item_enable(false);
						 btnEnroll->Text = "Enrolling..";
						 btnCancel->Enabled = true;					

						 lib_init();

						 set_enroll_count(Convert::ToInt32(tb_enroll_count->Text));

						 max_samples = get_enroll_count();

						 IntPtr convertString = Marshal::StringToHGlobalAnsi(template_type_path);
						 char* nativeChar = static_cast<char*>(convertString.ToPointer());

						 if (tb_enroll_name->Text == "" || tb_enroll_name->Text == "Your Name") {
							 tb_enroll_name->Text = "Your Name";
							 filename = "Finger";

							 set_archive_path(nativeChar, filename);
							 finger_idx = enroll_setup();
						 }
						 else {
							 filename = (char*)(void*)Marshal::StringToHGlobalAnsi(tb_enroll_name->Text);
							 set_archive_path(nativeChar, filename);
							 finger_idx = enroll_setup();
						 }
						 sprintf(finger_idx_buf, "%02d", finger_idx);

						 quality_chk_flag = true;	
						 get_frame = true;
						 start_enroll = true;
						 num_accepted_prev = 0;
						 num_accepted = 0;

						 enroll_timer->Interval = 33;
						 enroll_timer->Start();

						 printf("ENROLLMENT - starting capturing and enrollment\n");
						 printf("----------------------------------------------\n");
						 
						 strcpy(file_path_string, app_path);
						 strcat(file_path_string, "\\res");
						 strcat(file_path_string, "\\fingerprint_enroll_scan.gif");
						 sprintf(fingerprint_enrollScan, file_path_string);
						 enrollScan = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)fingerprint_enrollScan));
						 pb_gif->Visible = true;
						 pb_gif->Image = enrollScan;

						 strcpy(file_path_string, app_path);
						 strcat(file_path_string, "\\res");
						 strcat(file_path_string, "\\enroll");
						 strcat(file_path_string, "\\fingerprint_enroll_progress-00.png");
						 sprintf(fingerprint_enrollProgress, file_path_string);
						 enrollProgress = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)fingerprint_enrollProgress));
						 pb_png->Visible = true;
						 pb_png->Parent = pb_gif;
						 pb_png->Location = System::Drawing::Point(0, 0);
						 pb_png->Image = enrollProgress;
					 }
					 else {
						 not_detect_device();
					 }					 
				 }				 
	}		
	private: System::Void enroll_timer_Tick(System::Object^  sender, System::EventArgs^  e) {

				 pb_image_t* image = 0;
				 uint8_t image_quality = 0;
				 uint16_t fingerprint_area = 0;

				 //num_accepted = 0;
				 uint8_t coverage_area = 0;
				 int template_size = 0;
				 int file_index = -1;

				 if (frame = cvQueryFrame(capture)) {
					 if (get_frame) {
						 frame = NULL;
						 get_frame = false;
					 }
					 else {
						 int size = (frame->imageSize) / 3;

						 uchar* old_data = (uchar*)frame->imageData;
						 uchar* new_data = new uchar[size];

						 for (int i = 0; i < size; i++) {
							 new_data[i] = old_data[i * 3];
						 }

						 image = pb_image_create_mr(frame->width, frame->height, 508, 508, new_data, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);
						 if (!image) {
							 printf("ERROR: pb_image_create_mr() failed.\n");
						 }

						 quality_chk(image, &image_quality, &fingerprint_area);

						 if (quality_chk_flag) {
							 if (image_quality > Convert::ToInt32(tb_enroll_image_quality->Text) &&
								 fingerprint_area > Convert::ToInt32(tb_enroll_fingerprint_area->Text)) {
								 //label1->Text = "Place finger on device, lift it off, \n then repeat. ";
								 label_quality->Text = "  " + image_quality;
								 label_area->Text = "  " + fingerprint_area;

								 num_accepted = enroll_finger(image, &coverage_area);

								 printf("  PBGUI : num_accepted = %d\n", num_accepted);
								 printf("  PBGUI : coverage_area => %d %%\n", coverage_area);
								
								 double count_percent = ((double)num_accepted / (double)max_samples) * 100;
								 double coverage_percent = ((double)coverage_area / (double)Convert::ToInt32(tb_enroll_return_coverage->Text)) * 100;
								 double count_progressRatio = (double)(enrollProgressImage->Length - 2) / (double)max_samples;
								 double count_progressIndex = (double)count_progressRatio * num_accepted;
								 double coverage_progressRatio = (double)enrollProgressImage->Length / ((double)Convert::ToInt32(tb_enroll_return_coverage->Text) / 10);
								 double coverage_progressIndex = (double)coverage_progressRatio * (double)(coverage_area / 10);
								 printf("coverage_progressIndex : %f\n", (double)coverage_progressIndex);

								 if (Convert::ToInt32(tb_enroll_return_coverage->Text) == 0) {
									 if (num_accepted > num_accepted_prev) {
										 num_accepted_prev = num_accepted;

										 //label1->Text += (int)round(count_percent) + "%\n";
										 //label1->Text += "\n";
										 label1->Text = "Place finger on device, lift it off, \n then repeat.\n";

										 if ((int)round(count_progressIndex) >= enrollProgressImage->Length) {
											 pb_png->Image = enrollProgressImage[enrollProgressImage->Length - 2];
										 }
										 else {
											 pb_png->Image = enrollProgressImage[(int)round(count_progressIndex)];
										 }	
										 
									 }
									 else {
										 label1->Text = "Move finger more !\n";
										 //label1->Text += (int)round(count_percent) + "%";
										 //label1->Text += "\n";
									 }
								 }
								 else {
									 if (num_accepted > num_accepted_prev) {
										 num_accepted_prev = num_accepted;
										 if (coverage_percent > 100) {
											 coverage_percent = 100;
										 }
										 
										 //label1->Text += (int)round(coverage_percent) + "%\n";
										 //label1->Text += "\n";
										 
										 if ((int)round(coverage_progressIndex) >= enrollProgressImage->Length) {
											 printf("enroll progress index = %d", (int)round(coverage_progressIndex));
											 pb_png->Image = enrollProgressImage[enrollProgressImage->Length - 2];
										 }
										 else {
											 printf("enroll progress index = %d", (int)round(coverage_progressIndex));
											 pb_png->Image = enrollProgressImage[(int)round(coverage_progressIndex)];
										 }
										 
									 }
									 else {
										 label1->Text = "Move finger more !\n";
										 //label1->Text += (int)round(coverage_percent) + "%";
										 //label1->Text += "\n";
									 }
								 }

								 if (enroll_preview) {
									 pictureBox1->Image = gcnew Bitmap(pictureBox1->Width, pictureBox1->Height);
									 Graphics^ graphics = Graphics::FromImage(pictureBox1->Image);
									 System::Drawing::Bitmap^ b = gcnew System::Drawing::Bitmap(frame->width, frame->height, frame->widthStep,
										 System::Drawing::Imaging::PixelFormat::Format24bppRgb, (System::IntPtr)frame->imageData);
									 System::Drawing::RectangleF rect(0, 0, pictureBox1->Width, pictureBox1->Height);
									 graphics->DrawImage(b, rect);

									 if (enroll_save_image) {
										 char imagePath[200];
										 char string[200];
										
										 strcpy(string, (char*)(void*)Marshal::StringToHGlobalAnsi(image_type_path));
										 strcat(string, "\\");
										 strcat(string, date_buf);
										 strcat(string, "_");
										 strcat(string, filename);
										 strcat(string, "_");
										 strcat(string, finger_idx_buf);
										 strcat(string, "_%d.png");
										 sprintf(imagePath, string, num_accepted);
										 b->Save(Marshal::PtrToStringAnsi((IntPtr)imagePath), ImageFormat::Png);
									 }
								 }
								 quality_chk_flag = false;
							 }
						 }

						 if (image) {
							 pb_image_delete(image);
						 }

						 if (fingerprint_area == 0) {
							 quality_chk_flag = true;
						 }

						 if (Convert::ToInt32(tb_enroll_return_coverage->Text) == 0) {							
							 if (num_accepted == max_samples) {
								 enroll_timer->Stop();

								 enroll_finish(&template_size);

								 lib_deinit();

								 //strcpy(file_path_string, app_path);
								 //strcat(file_path_string, "\\res");
								 //strcat(file_path_string, "\\fingerprint_enroll_final.png");
								 //sprintf(fingerprint_enrollFinish, file_path_string);
								 //enrollFinish = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)fingerprint_enrollFinish));								
								 //pb_png->Image = enrollFinish;

								 pb_png->Image = enrollProgressImage[enrollProgressImage->Length - 1];

								 lb_message->Visible = true;
								 lb_message->Text = "Enroll Successfully\n";

								 //label1->TextAlign = ContentAlignment::MiddleLeft;
								 //label1->Text = " Template information \n";
								 label1->Text = "Template Name : " + Marshal::PtrToStringAnsi((IntPtr)get_output_file_name()) + "\n";

								 //lb_enroll_message_visible(true);								
								 //lb_enroll_parameter->Text = Marshal::PtrToStringAnsi((IntPtr)get_output_file_name()) + "\n";
								 //lb_enroll_parameter->Text += num_accepted + "\n";
								 //lb_enroll_parameter->Text += template_size + " bytes\n";
								 //lb_enroll_parameter->Text += coverage_area + "%";

								 control_item_enable(true);
								 btnOpen_device->Enabled = false;
								 btnSave->Enabled = false;
								 btnEnroll->Text = "Enroll";								 

								 get_frame = false;
								 start_enroll = false;								 
							 }
						 }
						 else {
							 if (coverage_area >= Convert::ToInt32(tb_enroll_return_coverage->Text)) {
								 enroll_timer->Stop();

								 enroll_finish(&template_size);
								
								 lib_deinit();

								 pb_png->Image = enrollProgressImage[enrollProgressImage->Length - 1];

								 lb_message->Visible = true;
								 lb_message->Text = "Enroll Successfully\n";

								 //label1->TextAlign = ContentAlignment::MiddleLeft;
								 //label1->Text = " Template information \n";
								 label1->Text = "Template Name : " + Marshal::PtrToStringAnsi((IntPtr)get_output_file_name()) + "\n";
								
								 //lb_enroll_message_visible(true);								 								
								 //lb_enroll_parameter->Text = Marshal::PtrToStringAnsi((IntPtr)get_output_file_name()) + "\n";
								 //lb_enroll_parameter->Text += num_accepted + "\n";
								 //lb_enroll_parameter->Text += template_size + " bytes\n";
								 //lb_enroll_parameter->Text += coverage_area + "%";

								 control_item_enable(true);
								 btnOpen_device->Enabled = false;
								 btnSave->Enabled = false;
								 btnEnroll->Text = "Enroll";								

								 get_frame = false;
								 start_enroll = false;
							 }
						 }					 
					 }
				 }
				 else {
					 not_detect_device();

					 if (image) {
						 pb_image_delete(image);
					 }

					 lib_deinit();
				 }
	}
	private: System::Void btnVerify_Click(System::Object^  sender, System::EventArgs^  e) {
				 printf("btnVerify_Click...\n\n");

				 pb_gif->Visible = false;

				 textBox2->Focus();

				 label_quality->Text = "  " + 0;
				 label_area->Text = "  " + 0;

				 if (cb_verify_preview->CheckState == CheckState::Checked) {
					 verify_preview = true;
					 pictureBox1->Image = nullptr;
				 }
				 else {
					 verify_preview = false;
					 pictureBox1->Image = logoImage;
				 }

				 if (cb_verify_con->CheckState == CheckState::Checked) {
					 verify_con = true;
				 }
				 else {
					 verify_con = false;				
				 }

				 if (btnVerify->Text == "Verify") {
					 fileList = list_files();
					 if (fileList->Length <= 0) {
						 lb_open_message_visible(false);
						 lb_enroll_message_visible(false);
						 lb_message->Visible = false;

						 label1->TextAlign = ContentAlignment::MiddleCenter;
						 label1->Text = "The database no templates to verify.";
						 label1->ForeColor = Color::Red;
					 } else {
						 if (capture = cvCreateCameraCapture(deviceNumber)) {
							 printf("Place finger to verify template on device.\n");

							 lb_message->Visible = true;
							 lb_message->Text = "Verifying ....";

							 lb_open_message_visible(false);
							 lb_enroll_message_visible(false);

							 control_item_enable(false);
							 btnVerify->Text = "Verifying..";	
							 btnCancel->Enabled = true;

							 label1->TextAlign = ContentAlignment::BottomCenter;
							 label1->Text = "Place your fingertip on the device \nto verify your identity.\n";
							 label1->Refresh();

							 if (tb_verify_image_quality->Text == "" || Convert::ToInt32(tb_verify_image_quality->Text) > 100) {
								 if ((int)dWidth == 120) {
									 tb_enroll_image_quality->Text = Convert::ToString(verify_image_quality);
								 }
							 }

							 if (tb_verify_fingerprint_area->Text == "" || Convert::ToInt32(tb_verify_fingerprint_area->Text) > max_fingerprint_area_301c) {
								 tb_verify_fingerprint_area->Text = Convert::ToString(verify_fingerprint_area);
							 }

							 lib_init();

							 IntPtr convertString = Marshal::StringToHGlobalAnsi(template_type_path);
							 char* nativeChar = static_cast<char*>(convertString.ToPointer());

							 set_archive_path(nativeChar, 0);

							 verify_setup();

							 quality_chk_flag = true;
							 get_frame = true;

							 verify_timer->Interval = 33;
							 verify_timer->Start();

							 verify_fail = 0;

							 start_verify = true;

							 printf("verify - starting capturing and verify\n");
							 printf("----------------------------------------------\n");

							 pb_gif->Visible = true;
							 strcpy(file_path_string, app_path);
							 strcat(file_path_string, "\\res");
							 strcat(file_path_string, "\\fingerprint_verify_scan.gif");	
							 sprintf(fingerprint_verifyScan, file_path_string);
							 verifyScan = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)fingerprint_verifyScan));
							 pb_gif->Image = verifyScan;

							 pb_png->Visible = true;
							 pb_png->Parent = pb_gif;
							 pb_png->Location = System::Drawing::Point(0, 0);
							 strcpy(file_path_string, app_path);
							 strcat(file_path_string, "\\res");
							 strcat(file_path_string, "\\verify");
							 strcat(file_path_string, "\\fingerprint_verify_ok.png");
							 sprintf(fingerprint_verifySuccess, file_path_string);
							 verifySuccess = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)fingerprint_verifySuccess));
							 pb_png->Image = nullptr;

							 strcpy(file_path_string, app_path);
							 strcat(file_path_string, "\\res");
							 strcat(file_path_string, "\\verify");
							 strcat(file_path_string, "\\fingerprint_verify_fail.png");
							 sprintf(fingerprint_verifyFail, file_path_string);
							 verifyFail = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)fingerprint_verifyFail));
						 }
					 }				 
				 }
				 else {
					 not_detect_device();
				 }
	}
	private: System::Void verify_timer_Tick(System::Object^  sender, System::EventArgs^  e) {

				 int cover = 0;
				 int res = -2;

				 pb_image_t*	image = 0;

				 uint8_t image_quality = 0;
				 uint16_t fingerprint_area = 0;
				 int matching_index = -1;

				 if (frame = cvQueryFrame(capture)) {
					 if (get_frame) {
						 frame = NULL;
						 get_frame = false;
					 }
					 else {
						 int size = (frame->imageSize) / 3;

						 uchar* old_data = (uchar*)frame->imageData;
						 uchar* new_data = new uchar[size];

						 for (int i = 0; i < size; i++) {
							 new_data[i] = old_data[i * 3];
						 }

						 image = pb_image_create_mr(frame->width, frame->height, 508, 508, new_data, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);
						 if (!image) {
							 printf("ERROR: pb_image_create_mr() failed.\n");
						 }

						 quality_chk(image, &image_quality, &fingerprint_area);
						 //printf("Quality : %d, Coverage = %d\n", resQuality, resCoverage);
						 if (quality_chk_flag) {
							 if (image_quality > Convert::ToInt32(tb_verify_image_quality->Text) &&
								 fingerprint_area > Convert::ToInt32(tb_verify_fingerprint_area->Text)) {
								 //printf("Quality : %d, Coverage = %d\n", image_quality, fingerprint_area);
								 label_quality->Text = "  " + image_quality;
								 label_area->Text = "  " + fingerprint_area;

								 res = verify_finger(image);

								 if (verify_preview) {
									 //System::Drawing::Graphics^ graphics = pictureBox1->CreateGraphics();	
									 pictureBox1->Image = gcnew Bitmap(pictureBox1->Width, pictureBox1->Height);
									 Graphics^ graphics = Graphics::FromImage(pictureBox1->Image);
									 //System::Drawing::Bitmap^ b = gcnew System::Drawing::Bitmap(frame->width, frame->height, frame->widthStep,
									 //	 System::Drawing::Imaging::PixelFormat::Format24bppRgb, (System::IntPtr)frame->imageData);
									 bmp = gcnew System::Drawing::Bitmap(frame->width, frame->height, frame->widthStep,
										 System::Drawing::Imaging::PixelFormat::Format24bppRgb, (System::IntPtr)frame->imageData);
									 System::Drawing::RectangleF rect(0, 0, pictureBox1->Width, pictureBox1->Height);
									 graphics->DrawImage(bmp, rect);								 
								 }
								 quality_chk_flag = false;
							 }
						 }

						 if (image) {
							 pb_image_delete(image);
						 }

						 if (fingerprint_area == 0) {
							 quality_chk_flag = true;
							 pb_gif->Image = enrollProgressImage[enrollProgressImage->Length - 1];
							 pb_png->Image = nullptr;
						 }

						 if (verify_con) {
							 if (res == -1) {
								 lb_message->Visible = true;
								 lb_message->Text = "Verify Failure";
								 label1->Text = "\nPlace finger to verify template\n\n";

								 pb_png->Image = verifyFail;
							 }
							 else if (res >= 0) {
								 verify_finish();

								 verify_fail = 0;

								 lb_message->Visible = true;
								 lb_message->Text = "Verify Successfully";
								 label1->Text = "\nPlace finger to verify template\n";
								 label1->Text += "\nTemplate: " + fileList[res];	

								 pb_png->Image = verifySuccess;
							 }
						 }
						 else {
							 if (res == -1) {
								 verify_fail++;

								 lb_message->Visible = true;
								 lb_message->Text = "Verify Failure";
								 label1->Text = "Verify fail " + verify_fail + " times, \n";
								 label1->Text += "place finger to verify template.\n";

								 pb_png->Image = verifyFail;

								 if (verify_preview) {
									 btnSave->Enabled = true;
								 }
								 else {
									 btnSave->Enabled = false;
								 }
							 }
							 else if (res >= 0) {
								 verify_timer->Stop();

								 verify_finish();

								 lib_deinit();
								 quality_chk_deinit();

								 lb_message->Visible = true;
								 lb_message->Text = "Verify Successfully";
								 label1->Text = "Template: " + fileList[res] + "\n";

								 pb_png->Image = verifySuccess;

								 control_item_enable(true);
								 btnOpen_device->Enabled = false;
								 if (verify_preview) {
									 btnSave->Enabled = true;
								 }
								 else {
									 btnSave->Enabled = false;
								 }
								 btnVerify->Text = "Verify";

								 start_verify = false;
							 }

							 if (verify_fail == 10) {
								 verify_timer->Stop();

								 lib_deinit();

								 label1->Text = "The verify of failures has reached 10,\n";
								 label1->Text += "so verify function stop.\n";

								 control_item_enable(true);
								 btnOpen_device->Enabled = false;
								 if (verify_preview) {
									 btnSave->Enabled = true;
								 }
								 else {
									 btnSave->Enabled = false;
								 }
								 btnVerify->Text = "Verify";
								
								 start_verify = false;
							 }
						 }
					 }
				 }
				 else {
					 not_detect_device();

					 if (image) {
						 pb_image_delete(image);
					 }
					 lib_deinit();
				 }
	}
	private: System::Void btnDeleteAll_Click(System::Object^  sender, System::EventArgs^  e) {
				 delete_files();

				 lb_open_message_visible(false);
				 lb_enroll_message_visible(false);

				 pb_gif->Visible = false;

				 lb_message->Visible = true;
				 lb_message->Text = "Delete Successfully";

				 label1->TextAlign = ContentAlignment::MiddleCenter;
				 label1->Text = "The database template already clear.";
	}

	private: System::Void btnSave_Click(System::Object^  sender, System::EventArgs^  e) {
				 time_t		now = time(0);
				 struct tm	tstruct;
				 tstruct = *localtime(&now);
				 strftime(datetime_buf, sizeof(datetime_buf), "%Y%m%d_%H%M", &tstruct);
			 
				 SaveFileDialog^ savefiledialog = gcnew SaveFileDialog;
				 savefiledialog->Filter = "PNG Image|*.png";

				 if (!System::IO::Directory::Exists(image_path + sensorType))
				 {
					 System::IO::Directory::CreateDirectory(image_path + sensorType);
					 savefiledialog->InitialDirectory = image_path + sensorType;
				 }
				 else {
					 savefiledialog->InitialDirectory = image_path + sensorType;
				 }
				 
				 savefiledialog->Title = "Save an Image File";
				 savefiledialog->FileName = Marshal::PtrToStringAnsi((IntPtr)datetime_buf);

				 if (savefiledialog->ShowDialog() == System::Windows::Forms::DialogResult::OK && savefiledialog->FileName != "")
				 {
					 //bmp->Save(savefiledialog->FileName, ImageFormat::Png);
					 pictureBox1->Image->Save(savefiledialog->FileName, ImageFormat::Png);
					 lb_message->Visible = true;
					 lb_message->Text = "Save Successfully\n\n";

					 pb_gif->Visible = true;
					 pb_png->Visible = true;
					 pb_png->Parent = pb_gif;
					 pb_png->Location = System::Drawing::Point(0, 0);
					 pb_png->Image = enrollProgressImage[enrollProgressImage->Length - 1];

					 label1->TextAlign = ContentAlignment::BottomCenter;
					 label1->Text = "File name : " + Path::GetFileName(savefiledialog->FileName) + "\n";
				 }
	
	}

	private: System::Void not_detect_device() {
				 realtime_timer->Stop();
				 enroll_timer->Stop();
				 verify_timer->Stop();

				 printf("Video capture device not exist.");
				 label1->TextAlign = ContentAlignment::MiddleCenter;
				 label1->Text = "Not detect fingerprint camera device,\n";
				 label1->Text += "please connect the device.";

				 control_item_enable(false);
				 btnOpen_device->Enabled = true;
				 btnRealTime->Text = "RealTime";
				 btnEnroll->Text = "Enroll";
				 btnVerify->Text = "Verify";			

				 lb_message->Visible = false;

				 lb_open_message_visible(false);
				 lb_enroll_message_visible(false);
	}

	private: Void set_adjust_parameter(int width, int height) {
				 if (width == 128 && height == 128) {
					 sensorType = "301B";
					 max_enrollment_samples = max_enrollment_samples_301b;
					 max_fingerprint_area = max_fingerprint_area_301b;
					 set_enrollment_samples = 8;
					 enrollment_image_quality = enrollment_image_quality_301b;
					 enrollment_fingerprint_area = enrollment_fingerprint_area_301b;
					 verify_image_quality = verify_image_quality_301b;
					 verify_fingerprint_area = verify_fingerprint_area_301b;
				 } 
				 else if (width == 120 && height == 120) {
					 sensorType = "301C";
					 max_enrollment_samples = max_enrollment_samples_301c;
					 max_fingerprint_area = max_fingerprint_area_301c;
					 set_enrollment_samples = 8;
					 enrollment_image_quality = enrollment_image_quality_301c;
					 enrollment_fingerprint_area = enrollment_fingerprint_area_301c;
					 verify_image_quality = verify_image_quality_301c;
					 verify_fingerprint_area = verify_fingerprint_area_301b;
				 }
				 else if (width == 176 && height == 176) {
					 sensorType = "501";
					 max_enrollment_samples = max_enrollment_samples_501;
					 max_fingerprint_area = max_fingerprint_area_501;
					 set_enrollment_samples = 5;
					 enrollment_image_quality = enrollment_image_quality_501;
					 enrollment_fingerprint_area = enrollment_fingerprint_area_501;
					 verify_image_quality = verify_image_quality_301c;
					 verify_fingerprint_area = verify_fingerprint_area_301b;
				 }
	}

	private: array<System::String^>^ list_files() {
				 System::String^ DS_file = ".DS_Store";

				 //array<System::String^>^ file = Directory::GetFiles(Marshal::PtrToStringAnsi((IntPtr)databaseFile));
				 array<System::String^>^ file = Directory::GetFiles(template_type_path);
				 array<System::String^>^ fileList = gcnew array<System::String^>(file->Length);

				 for (int i = 0; i < file->Length; i++) {
					 if (DS_file == Path::GetFileName(file[i])) {
						 printf("delete file name => %s\n", file[i]);
						 remove((char*)(void*)Marshal::StringToHGlobalAnsi(file[i]));
					 }
					 else {
						 fileList[i] = Path::GetFileName(file[i]);
					 }
				 }
				 printf("file length => %d\n", file->Length);			 
				 return fileList;
	}

	private: void delete_files() {
				 array<System::String^>^ file = Directory::GetFiles(template_type_path);
				 for (int i = 0; i < file->Length; i++) {
					 remove((char*)(void*)Marshal::StringToHGlobalAnsi(file[i]));
				 }
	}

	private: array<System::Drawing::Bitmap^>^ get_enroll_progress_image() {
				 array<System::Drawing::Bitmap^>^ progressList = gcnew array<System::Drawing::Bitmap^>(39);

				 char path[200];
				 char pathString[200];
				
				 struct _finddata_t c_file;
				 intptr_t hFile = 0;
				 int i = 0;

				 strcpy(file_path_string, app_path);
				 strcat(file_path_string, "\\res");
				 strcat(file_path_string, "\\enroll");
				 strcat(file_path_string, "\\*.png");

				 strcpy(pathString, app_path);
				 strcat(pathString, "\\res");
				 strcat(pathString, "\\enroll");
				 strcat(pathString, "\\%s");

				 hFile = _findfirst(file_path_string, &c_file);
				 if (hFile != -1) {
					 do {
						 sprintf(path, pathString, c_file.name);
						 progressList[i] = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)path));
						 i++;
					 } while (_findnext(hFile, &c_file) == 0);
				 }
				 _findclose(hFile);
				 
				 return progressList;
	}

	private: void control_item_enable(bool flag) {
				 if (flag) {					
					 btnOpen_device->Enabled = true;
					 btnCapture->Enabled = true;
					 btnRealTime->Enabled = true;
					 btnEnroll->Enabled = true;
					 btnVerify->Enabled = true;
					 btnDeleteAll->Enabled = true;
					 btnCancel->Enabled = true;
					 btnSave->Enabled = true;
					 cb_enroll_preview->Enabled = true;
					 cb_enroll_save_image->Enabled = true;
					 tb_enroll_name->Enabled = true;
					 tb_enroll_count->Enabled = true;
					 tb_enroll_return_coverage->Enabled = true;
					 tb_enroll_image_quality->Enabled = true;
					 tb_enroll_fingerprint_area->Enabled = true;
					 cb_verify_preview->Enabled = true;
					 cb_verify_con->Enabled = true;
					 tb_verify_image_quality->Enabled = true;
					 tb_verify_fingerprint_area->Enabled = true;
				 }
				 else {
					 btnOpen_device->Enabled = false;
					 btnCapture->Enabled = false;
					 btnRealTime->Enabled = false;
					 btnEnroll->Enabled = false;
					 btnVerify->Enabled = false;
					 btnDeleteAll->Enabled = false;
					 btnCancel->Enabled = false;
					 btnSave->Enabled = false;
					 cb_enroll_preview->Enabled = false;
					 cb_enroll_save_image->Enabled = false;
					 tb_enroll_name->Enabled = false;
					 tb_enroll_count->Enabled = false;
					 tb_enroll_return_coverage->Enabled = false;
					 tb_enroll_image_quality->Enabled = false;
					 tb_enroll_fingerprint_area->Enabled = false;
					 cb_verify_preview->Enabled = false;
					 cb_verify_con->Enabled = false;
					 tb_verify_image_quality->Enabled = false;
					 tb_verify_fingerprint_area->Enabled = false;
				 }
	}
	private: void lb_open_message_visible(bool flag) {
				 if (flag) {
					 lb_open_information->Visible = true;
					 lb_open_colon->Visible = true;
					 lb_open_parameter->Visible = true;
					 lb_open_information->Text = "Max enroll count\n";
					 lb_open_information->Text += "Max image quality\n";
					 lb_open_information->Text += "Max fingerprint area";
					 lb_open_colon->Text = ":\n";
					 lb_open_colon->Text += ":\n";
					 lb_open_colon->Text += ":";
				 }
				 else {
					 lb_open_information->Visible = false;
					 lb_open_colon->Visible = false;
					 lb_open_parameter->Visible = false;
				 }
	}
	private: void lb_enroll_message_visible(bool flag) {
				 if (flag) {
					 lb_enroll_information->Visible = true;
					 lb_enroll_colon->Visible = true;
					 lb_enroll_parameter->Visible = true;
					 lb_enroll_information->Text = "1) File Name\n";
					 lb_enroll_information->Text += "2) Count\n";
					 lb_enroll_information->Text += "3) Size\n";
					 lb_enroll_information->Text += "4) Coverage";
					 lb_enroll_colon->Text = ":\n";
					 lb_enroll_colon->Text += ":\n";
					 lb_enroll_colon->Text += ":\n";
					 lb_enroll_colon->Text += ":";
				 }
				 else {
					 lb_enroll_information->Visible = false;
					 lb_enroll_colon->Visible = false;
					 lb_enroll_parameter->Visible = false;
				 }
	}

	private: System::Void tb_enroll_count_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 /* e->KeyChar == (Char)48 ~ 57  -----> 0~9 */
				 /* e->KeyChar == (Char)13       -----> Enter */
				 /* e->KeyChar == (Char)8        -----> Backspace */
				 if (e->KeyChar == (Char)48 || e->KeyChar == (Char)49 ||
					 e->KeyChar == (Char)50 || e->KeyChar == (Char)51 ||
					 e->KeyChar == (Char)52 || e->KeyChar == (Char)53 ||
					 e->KeyChar == (Char)54 || e->KeyChar == (Char)55 ||
					 e->KeyChar == (Char)56 || e->KeyChar == (Char)57 ||
					 e->KeyChar == (Char)13 || e->KeyChar == (Char)8) {
					 e->Handled = false;
				 }
				 else {
					 e->Handled = true;
				 }
	}
	private: System::Void tb_enroll_image_quality_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 if (e->KeyChar == (Char)48 || e->KeyChar == (Char)49 ||
					 e->KeyChar == (Char)50 || e->KeyChar == (Char)51 ||
					 e->KeyChar == (Char)52 || e->KeyChar == (Char)53 ||
					 e->KeyChar == (Char)54 || e->KeyChar == (Char)55 ||
					 e->KeyChar == (Char)56 || e->KeyChar == (Char)57 ||
					 e->KeyChar == (Char)13 || e->KeyChar == (Char)8) {
					 e->Handled = false;
				 }
				 else {
					 e->Handled = true;
				 }
	}
	private: System::Void tb_enroll_fingerprint_area_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 if (e->KeyChar == (Char)48 || e->KeyChar == (Char)49 ||
					 e->KeyChar == (Char)50 || e->KeyChar == (Char)51 ||
					 e->KeyChar == (Char)52 || e->KeyChar == (Char)53 ||
					 e->KeyChar == (Char)54 || e->KeyChar == (Char)55 ||
					 e->KeyChar == (Char)56 || e->KeyChar == (Char)57 ||
					 e->KeyChar == (Char)13 || e->KeyChar == (Char)8) {
					 e->Handled = false;
				 }
				 else {
					 e->Handled = true;
				 }
	}
	private: System::Void tb_enroll_return_coverage_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 if (e->KeyChar == (Char)48 || e->KeyChar == (Char)49 ||
					 e->KeyChar == (Char)50 || e->KeyChar == (Char)51 ||
					 e->KeyChar == (Char)52 || e->KeyChar == (Char)53 ||
					 e->KeyChar == (Char)54 || e->KeyChar == (Char)55 ||
					 e->KeyChar == (Char)56 || e->KeyChar == (Char)57 ||
					 e->KeyChar == (Char)13 || e->KeyChar == (Char)8) {
					 e->Handled = false;
				 }
				 else {
					 e->Handled = true;
				 }
	}
	private: System::Void tb_verify_image_quality_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 if (e->KeyChar == (Char)48 || e->KeyChar == (Char)49 ||
					 e->KeyChar == (Char)50 || e->KeyChar == (Char)51 ||
					 e->KeyChar == (Char)52 || e->KeyChar == (Char)53 ||
					 e->KeyChar == (Char)54 || e->KeyChar == (Char)55 ||
					 e->KeyChar == (Char)56 || e->KeyChar == (Char)57 ||
					 e->KeyChar == (Char)13 || e->KeyChar == (Char)8) {
					 e->Handled = false;
				 }
				 else {
					 e->Handled = true;
				 }
	}
	private: System::Void tb_verify_fingerprint_area_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 if (e->KeyChar == (Char)48 || e->KeyChar == (Char)49 ||
					 e->KeyChar == (Char)50 || e->KeyChar == (Char)51 ||
					 e->KeyChar == (Char)52 || e->KeyChar == (Char)53 ||
					 e->KeyChar == (Char)54 || e->KeyChar == (Char)55 ||
					 e->KeyChar == (Char)56 || e->KeyChar == (Char)57 ||
					 e->KeyChar == (Char)13 || e->KeyChar == (Char)8) {
					 e->Handled = false;
				 }
				 else {
					 e->Handled = true;
				 }
	}

	OpenFileDialog^ openfiledialog;
	
	int fileCount = 0;
	int listViewItemNumber = 0;

	private: System::Void btnChooseFile_Click(System::Object^  sender, System::EventArgs^  e) {
				 openfiledialog = gcnew OpenFileDialog;
				 openfiledialog->RestoreDirectory = true;
				 if (System::IO::Directory::Exists(image_path + sensorType)) {
					 openfiledialog->InitialDirectory = image_path + sensorType;
				 }
				 else {
					 openfiledialog->InitialDirectory = image_path;
				 }
				 openfiledialog->Filter = "PNG Image|*.png";
				 openfiledialog->Multiselect = true;
				 openfiledialog->Title = "Open Image File";				
			
				 if (openfiledialog->ShowDialog() == System::Windows::Forms::DialogResult::OK && openfiledialog->FileName != "")
				 {
					 filePath = openfiledialog->FileNames;

					 fileCount = filePath->Length;

					 listView->Focus();

					 for (int i = 0; i < filePath->Length; i++)
					 {						 
						 System::String^ file_name = Path::GetFileName(filePath[i]);
						 System::String^ file_path = filePath[i];

						 System::String^ splite_file_name = file_name->Substring(0, file_name->Length - 4);

						 //listView->Items->Add(splite_file_name);
						 listView->Items->Add(file_name);

						 //IntPtr ptrToNativeFileNameString = Marshal::StringToHGlobalAnsi(splite_file_name);
						 //char* nativeFileNameString = static_cast<char*>(ptrToNativeFileNameString.ToPointer());

						 IntPtr ptrToNativeFileNameString = Marshal::StringToHGlobalAnsi(file_name);
						 char* nativeFileNameString = static_cast<char*>(ptrToNativeFileNameString.ToPointer());

						 IntPtr ptrToNativeFilePathString = Marshal::StringToHGlobalAnsi(file_path);
						 char* nativeFilePathString = static_cast<char*>(ptrToNativeFilePathString.ToPointer());

						 //printf("file name = %s\n", nativeFileNameString);
						 //printf("file path = %s\n", nativeFilePathString);
						 
						 FILENAME.push_back(nativeFileNameString);
						 FILEPATH.push_back(nativeFilePathString);
					 }			

					 listViewItemNumber += fileCount;

					 for (int a = 0; a < listViewItemNumber; a++)
					 {
						 listView->Items[a]->Focused = false;
						 listView->Items[a]->Selected = false;
					 }

					 listView->Items[listViewItemNumber - fileCount]->Focused = true;
					 listView->Items[listViewItemNumber - fileCount]->Selected = true;
				 }		
	}

	private: System::Void lv_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		
				 if (listView->SelectedItems->Count > 0)
				 {
					 printf("lv_SelectedIndexChanged\n");
					 for (int i = 0; i < listViewItemNumber; i++)
					 {
						 if (listView->Items[i]->Selected == true)
						 {						
							 System::String^ strPath = gcnew System::String(FILEPATH[i].c_str());
							 System::String^ strName = gcnew System::String(FILENAME[i].c_str());

							 pb_showFile->Image = Image::FromFile(strPath);
							 lb_filename->Text = strName;
						 }
					 }
				 }
	}

	private: System::Void tabControl_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {

				 if (tabControl1->SelectedTab == tabPage1)
				 {
					 listView->Items->Clear();
					 FILEPATH.clear();
					 FILENAME.clear();
					 listViewItemNumber = 0;

					 cb_301b->Checked = false;
					 cb_301c->Checked = false;
					 cb_501->Checked = false;
				 }

				 if (tabControl1->SelectedTab == tabPage2)
				 {
					 printf("tabControl_SelectedIndexChanged\n");

					 //pictureBox1->Image = logoImage;

					 btnSave->Enabled = false;

					 lb_message->Visible = false;
					 pb_gif->Visible = false;

					 lb_open_message_visible(false);
					 lb_enroll_message_visible(false);

					 label1->TextAlign = ContentAlignment::MiddleCenter;
					 label1->Text = "Welcome to use fingerprint tool";

					 textBox2->Focus();

					 label_quality->Text = "  " + 0;
					 label_area->Text = "  " + 0;

					 if (start_capture) {
						 capture_timer->Stop();
						 start_capture = false;

						 control_item_enable(true);
						 btnOpen_device->Enabled = false;
						 btnSave->Enabled = false;
						 btnCapture->Text = "Capture";
					 }
					 else if (start_realtime) {
						 realtime_timer->Stop();
						 start_realtime = false;

						 control_item_enable(true);
						 btnOpen_device->Enabled = false;
						 btnSave->Enabled = false;
						 btnRealTime->Text = "RealTime";
					 }
					 else if (start_enroll) {
						 enroll_timer->Stop();
						 start_enroll = false;

						 control_item_enable(true);
						 btnOpen_device->Enabled = false;
						 btnSave->Enabled = false;
						 btnEnroll->Text = "Enroll";

						 pb_gif->Visible = false;

						 if (enroll_save_image) {
							 char imagePath[200];
							 char string[200];
							 strcpy(string, (char*)(void*)Marshal::StringToHGlobalAnsi(image_type_path));
							 strcat(string, "\\");
							 strcat(string, date_buf);
							 strcat(string, "_");
							 strcat(string, filename);
							 strcat(string, "_");
							 strcat(string, finger_idx_buf);
							 strcat(string, "_%d.png");

							 for (int i = 1; i <= max_samples; i++)
							 {
								 sprintf(imagePath, string, i);
								 remove(imagePath);
								 /*
								 if (access(imagePath, 0) == 0)
								 {
								 remove(imagePath);
								 }
								 else {
								 break;
								 }
								 */
							 }
						 }
					 }
					 else if (start_verify) {
						 verify_timer->Stop();
						 start_verify = false;

						 control_item_enable(true);
						 btnOpen_device->Enabled = false;
						 btnSave->Enabled = false;
						 btnVerify->Text = "Verify";

						 lb_message->Visible = false;
						 pb_gif->Visible = false;
					 }

					 if (sensorType == "301B")
					 {
						 cb_301b->Checked = true;
					 }
					 else if (sensorType == "501")
					 {
						 cb_501->Checked = true;
					 }
					 else {
						 cb_301c->Checked = true;
					 }
				 }
	}
	private: System::Void cb_301c_CheckStateChanged(System::Object^  sender, System::EventArgs^  e) {				 

				 if (cb_301c->CheckState == CheckState::Checked)
				 {
					 printf("cb_301c_CheckStateChanged\n");

					 cb_301b->Checked = false;
					 cb_501->Checked = false;

					 listView->Items->Clear();

					 checkListViewItem(image_path + "301C");
				 }
	}

	private: System::Void cb_301b_CheckStateChanged(System::Object^  sender, System::EventArgs^  e) {

				 if (cb_301b->CheckState == CheckState::Checked)
				 {
					 printf("cb_301b_CheckStateChanged\n");

					 cb_301c->Checked = false;
					 cb_501->Checked = false;

					 listView->Items->Clear();
					 pb_showFile->Image = nullptr;
					 lb_filename->Text = "";

					 checkListViewItem(image_path + "301B");
				 }
	}
	private: System::Void cb_501_CheckStateChanged(System::Object^  sender, System::EventArgs^  e) {
				 if (cb_501->CheckState == CheckState::Checked)
				 {
					 printf("cb_501_CheckStateChanged\n");

					 cb_301b->Checked = false;
					 cb_301c->Checked = false;				

					 listView->Items->Clear();
					 pb_showFile->Image = nullptr;
					 lb_filename->Text = "";

					 checkListViewItem(image_path + "501");
				 }
	}

	private: void checkListViewItem(System::String^ path)
	{
				 //if (System::IO::Directory::Exists(Marshal::PtrToStringAnsi((IntPtr)path)))
				 if (System::IO::Directory::Exists(path))
				 {
					 //array<System::String^>^ file = Directory::GetFiles(Marshal::PtrToStringAnsi((IntPtr)path));
					 array<System::String^>^ file = Directory::GetFiles(path);
					 array<System::DateTime^>^ dateTime = gcnew array<System::DateTime^>(file->Length);

					 if (file->Length > 0)
					 {
						 listView->Focus();
						 /*
						 for (int x = 0; x < file->Length; x++)
						 {
						 FileInfo^ sfi = gcnew FileInfo(file->GetValue(x)->ToString());
						 dateTime[x] = sfi->LastWriteTime;
						 Console::WriteLine("name : " + sfi->Name);
						 Console::WriteLine("time : " + dateTime[x]);
						 }

						 Array::Sort(dateTime, file);
						 */

						 for (int i = 0; i < file->Length; i++) {

							 System::String^ file_name = Path::GetFileName(file[i]);
							 System::String^ file_path = file[i];

							 System::String^ splite_file_name = file_name->Substring(0, file_name->Length - 4);

							 //listView->Items->Add(splite_file_name);
							 listView->Items->Add(file_name);

							 //IntPtr ptrToNativeFileNameString = Marshal::StringToHGlobalAnsi(splite_file_name);
							 //char* nativeFileNameString = static_cast<char*>(ptrToNativeFileNameString.ToPointer());

							 IntPtr ptrToNativeFileNameString = Marshal::StringToHGlobalAnsi(file_name);
							 char* nativeFileNameString = static_cast<char*>(ptrToNativeFileNameString.ToPointer());

							 IntPtr ptrToNativeFilePathString = Marshal::StringToHGlobalAnsi(file_path);
							 char* nativeFilePathString = static_cast<char*>(ptrToNativeFilePathString.ToPointer());

							 FILENAME.push_back(nativeFileNameString);
							 FILEPATH.push_back(nativeFilePathString);
						 }
						 listViewItemNumber = file->Length;

						 listView->Items[listViewItemNumber - 1]->Focused = true;
						 listView->Items[listViewItemNumber - 1]->Selected = true;
						 listView->Items[listViewItemNumber - 1]->EnsureVisible();
					 }
				 }
	}
private: System::Void PBGUI_Load(System::Object^  sender, System::EventArgs^  e) {
}
};
}
