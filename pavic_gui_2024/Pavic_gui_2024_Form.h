#pragma once


#include <thread> // Inclua esta linha para usar std::thread
#include <algorithm> // Inclua esta linha para usar std::min
#include <msclr/gcroot.h> // Include this at the top of your .cpp 
#include <chrono> // Timer
#include "CudaBlur.h" // Inclua o novo header


namespace pavicgui2024 {
	//#include "include/Diagnostic.h"





	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging; // <-- Adicione esta linha para BitmapData, ImageLockMode, PixelFormat
	using namespace std::chrono; // Timer

	using namespace std; // Assuming Diagnostics is in the std namespace

	//================================================================================
	// // --- In�cio da fun��o auxiliar para aplicar o filtro s�pia parcialmente ---
	//// Esta fun��o ser� executada pelas threads.
	//// Ela precisa ser definida fora da classe do formul�rio para ser usada com std::thread de forma mais simples.
	void ApplySepiaFilterPartial(Bitmap^ inputImage, Bitmap^ outputImage, int startY, int endY) {
		for (int i = 0; i < inputImage->Width; i++) {
			for (int j = startY; j < endY; j++) {
				// Passo 1: Obter a cor do pixel atual
				Color pixelColor = inputImage->GetPixel(i, j);

				// Passo 2: Extrair os valores originais de Vermelho, Verde e Azul
				int r = pixelColor.R;
				int g = pixelColor.G;
				int b = pixelColor.B;

				// Passo 3: Calcular os novos valores de pixel usando a F�rmula S�pia
				double tr = 0.393 * r + 0.769 * g + 0.189 * b;
				double tg = 0.349 * r + 0.686 * g + 0.168 * b;
				double tb = 0.272 * r + 0.534 * g + 0.131 * b;

				// Passo 4: Limitar os valores ao intervalo 0-255
				int newR = Math::Min(255, (int)tr);
				int newG = Math::Min(255, (int)tg);
				int newB = Math::Min(255, (int)tb);

				outputImage->SetPixel(i, j, Color::FromArgb(newR, newG, newB));
			}
		}
	}
	// --- Fim da fun��o auxiliar ---
	// 
	// ==================================================================================
	//================================================================================
	// // --- In�cio da fun��o auxiliar para aplicar o filtro s�pia parcialmente ---
	//// Esta fun��o ser� executada pelas threads.
	//// Ela precisa ser definida fora da classe do formul�rio para ser usada com std::thread de forma mais simples.
	void ApplySepiaFilterWindow(Bitmap^ inputImage, Bitmap^ outputImage, int startX, int endX, int startY, int endY) {
		for (int i = startX; i < endX; i++) {
			for (int j = startY; j < endY; j++) {
				// Passo 1: Obter a cor do pixel atual
				Color pixelColor = inputImage->GetPixel(i, j);

				// Passo 2: Extrair os valores originais de Vermelho, Verde e Azul
				int r = pixelColor.R;
				int g = pixelColor.G;
				int b = pixelColor.B;

				// Passo 3: Calcular os novos valores de pixel usando a F�rmula S�pia
				double tr = 0.393 * r + 0.769 * g + 0.189 * b;
				double tg = 0.349 * r + 0.686 * g + 0.168 * b;
				double tb = 0.272 * r + 0.534 * g + 0.131 * b;

				// Passo 4: Limitar os valores ao intervalo 0-255
				int newR = Math::Min(255, (int)tr);
				int newG = Math::Min(255, (int)tg);
				int newB = Math::Min(255, (int)tb);

				outputImage->SetPixel(i, j, Color::FromArgb(newR, newG, newB));
			}
		}
	}
	// --- Fim da fun��o auxiliar ---
	// ===============================================================================
	// Helper struct to pass arguments to the unmanaged thread function
	struct SepiaThreadArgs {
		msclr::gcroot<Bitmap^> inputImage;
		msclr::gcroot<Bitmap^> outputImage;
		int startX;
		int endX;
		int startY;
		int endY;
	};

	// This function MUST be an unmanaged (native) C++ function
	// It cannot directly take Bitmap^ as parameters.
	void ApplySepiaFilterWindow_Unmanaged(SepiaThreadArgs* args) {
		Bitmap^ inputImage = args->inputImage;   // Get managed handle from gcroot
		Bitmap^ outputImage = args->outputImage; // Get managed handle from gcroot

		// Ensure the managed objects are still valid before using
		if (inputImage == nullptr || outputImage == nullptr) {
			// Handle error or return
			return;
		}

		// You would typically use LockBits here for performance with raw pixel data
		// because GetPixel/SetPixel are very slow and involve interop calls per pixel.
		// For simplicity, keeping your original GetPixel/SetPixel logic:
		for (int i = args->startX; i < args->endX; i++) {
			for (int j = args->startY; j < args->endY; j++) {
				Color pixelColor = inputImage->GetPixel(i, j);

				int r = pixelColor.R;
				int g = pixelColor.G;
				int b = pixelColor.B;

				double tr = 0.393 * r + 0.769 * g + 0.189 * b;
				double tg = 0.349 * r + 0.686 * g + 0.168 * b;
				double tb = 0.272 * r + 0.534 * g + 0.131 * b;

				int newR = System::Math::Min(255, (int)tr); // Use System::Math for managed functions
				int newG = System::Math::Min(255, (int)tg);
				int newB = System::Math::Min(255, (int)tb);

				outputImage->SetPixel(i, j, Color::FromArgb(newR, newG, newB));
			}
		}

		// Don't delete args here if it was created on the stack of the calling function.
		// If it was dynamically allocated (e.g., `new SepiaThreadArgs()`), then you'd delete it here.
	}

	// This function MUST be an unmanaged (native) C++ function
	// It cannot directly take Bitmap^ as parameters.
	void ApplyBWFilterWindow_Unmanaged(SepiaThreadArgs* args) {
		Bitmap^ inputImage = args->inputImage;   // Get managed handle from gcroot
		Bitmap^ outputImage = args->outputImage; // Get managed handle from gcroot

		// Ensure the managed objects are still valid before using
		if (inputImage == nullptr || outputImage == nullptr) {
			// Handle error or return
			return;
		}

		//// Apply the black and white filter
		// You would typically use LockBits here for performance with raw pixel data
		// because GetPixel/SetPixel are very slow and involve interop calls per pixel.
		// For simplicity, keeping your original GetPixel/SetPixel logic:
		for (int i = args->startX; i < args->endX; i++) {
			for (int j = args->startY; j < args->endY; j++) {

				Color pixelColor = inputImage->GetPixel(i, j);
				int grayValue = (int)(0.299 * pixelColor.R + 0.587 * pixelColor.G + 0.114 * pixelColor.B);
				outputImage->SetPixel(i, j, Color::FromArgb(grayValue, grayValue, grayValue));

			}
		}


		// Don't delete args here if it was created on the stack of the calling function.
		// If it was dynamically allocated (e.g., `new SepiaThreadArgs()`), then you'd delete it here.
	}

	// 
	// ===============================================================================
	// 
	//================================================================================
	//================================================================================
	// --- In�cio da fun��o auxiliar para aplicar o filtro s�pia parcialmente ---
	// Esta fun��o ser� executada pelas threads e operar� em dados brutos.
	// Ela precisa ser definida fora da classe do formul�rio.
	void ApplySepiaFilterPartialRaw(IntPtr inputScan0, int inputStride, IntPtr outputScan0, int outputStride, int width, int bytesPerPixel, int startY, int endY) {
		// Converter IntPtr para ponteiros de bytes n�o gerenciados
		unsigned char* ptrInput = (unsigned char*)inputScan0.ToPointer();
		unsigned char* ptrOutput = (unsigned char*)outputScan0.ToPointer();

		for (int j = startY; j < endY; j++) {
			for (int i = 0; i < width; i++) {
				// Calcular o offset para o pixel atual na linha
				long offsetInput = (long)j * inputStride + (long)i * bytesPerPixel;
				long offsetOutput = (long)j * outputStride + (long)i * bytesPerPixel;

				// Obter os valores de cor (assumindo formato BGR ou BGRA)
				int b = ptrInput[offsetInput];
				int g = ptrInput[offsetInput + 1];
				int r = ptrInput[offsetInput + 2];
				// Se for 32bpp (BGRA), o quarto byte � o canal alfa.
				// int a = (bytesPerPixel == 4) ? ptrInput[offsetInput + 3] : 255; // Linha comentada por enquanto, se precisar de alfa, descomente e ajuste

				// Aplicar a f�rmula S�pia
				double tr = 0.393 * r + 0.769 * g + 0.189 * b;
				double tg = 0.349 * r + 0.686 * g + 0.168 * b;
				double tb = 0.272 * r + 0.534 * g + 0.131 * b;

				// Limitar os valores ao intervalo 0-255
				int newR = Math::Min(255, (int)tr);
				int newG = Math::Min(255, (int)tg);
				int newB = Math::Min(255, (int)tb);

				// Definir os novos valores de pixel na imagem de sa�da
				ptrOutput[offsetOutput] = (unsigned char)newB;
				ptrOutput[offsetOutput + 1] = (unsigned char)newG;
				ptrOutput[offsetOutput + 2] = (unsigned char)newR;
				// Se for 32bpp (BGRA), manter o canal alfa original ou definir como 255
				// if (bytesPerPixel == 4) ptrOutput[offsetOutput + 3] = (unsigned char)a; // Linha comentada por enquanto, se precisar de alfa, descomente e ajuste
			}
		}
	}
	// --- Fim da fun��o auxiliar ---
	//==============================================================


	/// <summary>
	/// Summary for Pavic_gui_2024_Form
	/// </summary>
	public ref class Pavic_gui_2024_Form : public System::Windows::Forms::Form
	{
	public:
		Pavic_gui_2024_Form(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}



	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Pavic_gui_2024_Form()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ bt_open;
	protected:
	private: System::Windows::Forms::Button^ bt_close;
	private: System::Windows::Forms::Button^ bt_exit;
	private: System::Windows::Forms::PictureBox^ pbox_input;
	private: System::Windows::Forms::PictureBox^ pbox_copy;
	private: System::Windows::Forms::PictureBox^ pbox_output;
	private: System::Windows::Forms::Button^ bt_copy;
	private: System::Windows::Forms::Button^ bt_filter_bw;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::Button^ bt_close_copy;
	private: System::Windows::Forms::Button^ bt_close_output;

	private: System::Windows::Forms::Label^ label11;
	private: System::Windows::Forms::Label^ label12;
	private: System::Diagnostics::Stopwatch^ copyStopwatch;
	private: System::Diagnostics::Stopwatch^ filterStopwatch;
	private: System::Windows::Forms::Button^ bt_filter_Sepia;
	private: System::Windows::Forms::Button^ bt_filter_Sepia_MultiThread;
	private: System::Windows::Forms::Button^ bt_filter_Sepia_top;
	private: System::Windows::Forms::Button^ bt_filter_Sepia_botton;


	private: System::Windows::Forms::Label^ lb_timer;
	private: System::Windows::Forms::TextBox^ textB_Time;
	private: System::Windows::Forms::Button^ button1;
	private: System::Windows::Forms::Button^ bt_filter_Sepia_left;
	private: System::Windows::Forms::Button^ bt_filter_Sepia_Thread;
	private: System::Windows::Forms::Button^ bt_filter_cuda_blur;






	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->bt_open = (gcnew System::Windows::Forms::Button());
			this->bt_close = (gcnew System::Windows::Forms::Button());
			this->bt_exit = (gcnew System::Windows::Forms::Button());
			this->pbox_input = (gcnew System::Windows::Forms::PictureBox());
			this->pbox_copy = (gcnew System::Windows::Forms::PictureBox());
			this->pbox_output = (gcnew System::Windows::Forms::PictureBox());
			this->bt_copy = (gcnew System::Windows::Forms::Button());
			this->bt_filter_bw = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->bt_close_copy = (gcnew System::Windows::Forms::Button());
			this->bt_close_output = (gcnew System::Windows::Forms::Button());
			this->bt_filter_Sepia = (gcnew System::Windows::Forms::Button());
			this->bt_filter_Sepia_MultiThread = (gcnew System::Windows::Forms::Button());
			this->bt_filter_Sepia_top = (gcnew System::Windows::Forms::Button());
			this->bt_filter_Sepia_botton = (gcnew System::Windows::Forms::Button());
			this->lb_timer = (gcnew System::Windows::Forms::Label());
			this->textB_Time = (gcnew System::Windows::Forms::TextBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->bt_filter_Sepia_left = (gcnew System::Windows::Forms::Button());
			this->bt_filter_Sepia_Thread = (gcnew System::Windows::Forms::Button());
			this->bt_filter_cuda_blur = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbox_input))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbox_copy))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbox_output))->BeginInit();
			this->SuspendLayout();
			// 
			// bt_open
			// 
			this->bt_open->Location = System::Drawing::Point(14, 15);
			this->bt_open->Name = L"bt_open";
			this->bt_open->Size = System::Drawing::Size(213, 57);
			this->bt_open->TabIndex = 0;
			this->bt_open->Text = L"Open";
			this->bt_open->UseVisualStyleBackColor = true;
			this->bt_open->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_open_Click);
			// 
			// bt_close
			// 
			this->bt_close->Location = System::Drawing::Point(436, 295);
			this->bt_close->Name = L"bt_close";
			this->bt_close->Size = System::Drawing::Size(142, 43);
			this->bt_close->TabIndex = 1;
			this->bt_close->Text = L"Close";
			this->bt_close->UseVisualStyleBackColor = true;
			this->bt_close->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_close_Click);
			// 
			// bt_exit
			// 
			this->bt_exit->Location = System::Drawing::Point(14, 143);
			this->bt_exit->Name = L"bt_exit";
			this->bt_exit->Size = System::Drawing::Size(213, 57);
			this->bt_exit->TabIndex = 2;
			this->bt_exit->Text = L"Exit";
			this->bt_exit->UseVisualStyleBackColor = true;
			this->bt_exit->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_exit_Click);
			// 
			// pbox_input
			// 
			this->pbox_input->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pbox_input->Location = System::Drawing::Point(20, 346);
			this->pbox_input->Name = L"pbox_input";
			this->pbox_input->Size = System::Drawing::Size(559, 507);
			this->pbox_input->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pbox_input->TabIndex = 3;
			this->pbox_input->TabStop = false;
			this->pbox_input->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::pbox_input_Click);
			// 
			// pbox_copy
			// 
			this->pbox_copy->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pbox_copy->Location = System::Drawing::Point(624, 346);
			this->pbox_copy->Name = L"pbox_copy";
			this->pbox_copy->Size = System::Drawing::Size(559, 507);
			this->pbox_copy->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pbox_copy->TabIndex = 4;
			this->pbox_copy->TabStop = false;
			// 
			// pbox_output
			// 
			this->pbox_output->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->pbox_output->Location = System::Drawing::Point(1222, 346);
			this->pbox_output->Name = L"pbox_output";
			this->pbox_output->Size = System::Drawing::Size(559, 507);
			this->pbox_output->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pbox_output->TabIndex = 5;
			this->pbox_output->TabStop = false;
			// 
			// bt_copy
			// 
			this->bt_copy->Location = System::Drawing::Point(14, 78);
			this->bt_copy->Name = L"bt_copy";
			this->bt_copy->Size = System::Drawing::Size(213, 57);
			this->bt_copy->TabIndex = 6;
			this->bt_copy->Text = L"Copy";
			this->bt_copy->UseVisualStyleBackColor = true;
			this->bt_copy->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_copy_Click);
			// 
			// bt_filter_bw
			// 
			this->bt_filter_bw->Location = System::Drawing::Point(324, 17);
			this->bt_filter_bw->Name = L"bt_filter_bw";
			this->bt_filter_bw->Size = System::Drawing::Size(213, 57);
			this->bt_filter_bw->TabIndex = 7;
			this->bt_filter_bw->Text = L"Filter BW";
			this->bt_filter_bw->UseVisualStyleBackColor = true;
			this->bt_filter_bw->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_filter_bw_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(1589, 869);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(215, 20);
			this->label1->TabIndex = 8;
			this->label1->Text = L"Caio Cesar Faneco Gonzaga";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(10, 886);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(140, 20);
			this->label2->TabIndex = 9;
			this->label2->Text = L" PAVIC LAB: 2025";
			// 
			// bt_close_copy
			// 
			this->bt_close_copy->Location = System::Drawing::Point(1041, 295);
			this->bt_close_copy->Name = L"bt_close_copy";
			this->bt_close_copy->Size = System::Drawing::Size(142, 43);
			this->bt_close_copy->TabIndex = 10;
			this->bt_close_copy->Text = L"Close";
			this->bt_close_copy->UseVisualStyleBackColor = true;
			this->bt_close_copy->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_close_copy_Click);
			// 
			// bt_close_output
			// 
			this->bt_close_output->Location = System::Drawing::Point(1641, 295);
			this->bt_close_output->Name = L"bt_close_output";
			this->bt_close_output->Size = System::Drawing::Size(142, 43);
			this->bt_close_output->TabIndex = 11;
			this->bt_close_output->Text = L"Close";
			this->bt_close_output->UseVisualStyleBackColor = true;
			this->bt_close_output->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_close_output_Click);
			// 
			// bt_filter_Sepia
			// 
			this->bt_filter_Sepia->Location = System::Drawing::Point(324, 80);
			this->bt_filter_Sepia->Name = L"bt_filter_Sepia";
			this->bt_filter_Sepia->Size = System::Drawing::Size(213, 57);
			this->bt_filter_Sepia->TabIndex = 12;
			this->bt_filter_Sepia->Text = L"Filter Sepia";
			this->bt_filter_Sepia->UseVisualStyleBackColor = true;
			this->bt_filter_Sepia->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_filter_Sepia_Click);
			// 
			// bt_filter_Sepia_MultiThread
			// 
			this->bt_filter_Sepia_MultiThread->Location = System::Drawing::Point(324, 143);
			this->bt_filter_Sepia_MultiThread->Name = L"bt_filter_Sepia_MultiThread";
			this->bt_filter_Sepia_MultiThread->Size = System::Drawing::Size(213, 57);
			this->bt_filter_Sepia_MultiThread->TabIndex = 13;
			this->bt_filter_Sepia_MultiThread->Text = L"Filter Sepia MultiThread";
			this->bt_filter_Sepia_MultiThread->UseVisualStyleBackColor = true;
			this->bt_filter_Sepia_MultiThread->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_filter_Sepia_MultiThread_Click);
			// 
			// bt_filter_Sepia_top
			// 
			this->bt_filter_Sepia_top->Location = System::Drawing::Point(543, 17);
			this->bt_filter_Sepia_top->Name = L"bt_filter_Sepia_top";
			this->bt_filter_Sepia_top->Size = System::Drawing::Size(213, 57);
			this->bt_filter_Sepia_top->TabIndex = 14;
			this->bt_filter_Sepia_top->Text = L"Filter Sepia- Top";
			this->bt_filter_Sepia_top->UseVisualStyleBackColor = true;
			this->bt_filter_Sepia_top->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_filter_Sepia_left_Click);
			// 
			// bt_filter_Sepia_botton
			// 
			this->bt_filter_Sepia_botton->Location = System::Drawing::Point(762, 17);
			this->bt_filter_Sepia_botton->Name = L"bt_filter_Sepia_botton";
			this->bt_filter_Sepia_botton->Size = System::Drawing::Size(213, 57);
			this->bt_filter_Sepia_botton->TabIndex = 15;
			this->bt_filter_Sepia_botton->Text = L"Filter Sepia - Botton";
			this->bt_filter_Sepia_botton->UseVisualStyleBackColor = true;
			this->bt_filter_Sepia_botton->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::button2_Click);
			// 
			// lb_timer
			// 
			this->lb_timer->AutoSize = true;
			this->lb_timer->Location = System::Drawing::Point(1108, 115);
			this->lb_timer->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->lb_timer->Name = L"lb_timer";
			this->lb_timer->Size = System::Drawing::Size(48, 20);
			this->lb_timer->TabIndex = 16;
			this->lb_timer->Text = L"Timer";
			// 
			// textB_Time
			// 
			this->textB_Time->Location = System::Drawing::Point(1113, 140);
			this->textB_Time->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			this->textB_Time->Name = L"textB_Time";
			this->textB_Time->Size = System::Drawing::Size(240, 26);
			this->textB_Time->TabIndex = 17;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(762, 82);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(213, 57);
			this->button1->TabIndex = 19;
			this->button1->Text = L"Filter Sepia - Right";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::button1_Click_1);
			// 
			// bt_filter_Sepia_left
			// 
			this->bt_filter_Sepia_left->Location = System::Drawing::Point(543, 82);
			this->bt_filter_Sepia_left->Name = L"bt_filter_Sepia_left";
			this->bt_filter_Sepia_left->Size = System::Drawing::Size(213, 57);
			this->bt_filter_Sepia_left->TabIndex = 18;
			this->bt_filter_Sepia_left->Text = L"Filter Sepia- Left";
			this->bt_filter_Sepia_left->UseVisualStyleBackColor = true;
			this->bt_filter_Sepia_left->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_filter_Sepia_left_Click_1);
			// 
			// bt_filter_Sepia_Thread
			// 
			this->bt_filter_Sepia_Thread->Location = System::Drawing::Point(543, 145);
			this->bt_filter_Sepia_Thread->Name = L"bt_filter_Sepia_Thread";
			this->bt_filter_Sepia_Thread->Size = System::Drawing::Size(213, 57);
			this->bt_filter_Sepia_Thread->TabIndex = 20;
			this->bt_filter_Sepia_Thread->Text = L"Filter Sepia- Thread";
			this->bt_filter_Sepia_Thread->UseVisualStyleBackColor = true;
			this->bt_filter_Sepia_Thread->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_filter_Sepia_Thread_Click);
			// 
			// bt_filter_cuda_blur
			// 
			this->bt_filter_cuda_blur->Location = System::Drawing::Point(762, 145);
			this->bt_filter_cuda_blur->Name = L"bt_filter_cuda_blur";
			this->bt_filter_cuda_blur->Size = System::Drawing::Size(213, 57);
			this->bt_filter_cuda_blur->TabIndex = 21;
			this->bt_filter_cuda_blur->Text = L"Filter Cuda - Blur";
			this->bt_filter_cuda_blur->UseVisualStyleBackColor = true;
			this->bt_filter_cuda_blur->Click += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::bt_filter_cuda_blur_Click);
			// 
			// Pavic_gui_2024_Form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(9, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSize = true;
			this->ClientSize = System::Drawing::Size(1819, 949);
			this->Controls->Add(this->bt_filter_cuda_blur);
			this->Controls->Add(this->bt_filter_Sepia_Thread);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->bt_filter_Sepia_left);
			this->Controls->Add(this->textB_Time);
			this->Controls->Add(this->lb_timer);
			this->Controls->Add(this->bt_filter_Sepia_botton);
			this->Controls->Add(this->bt_filter_Sepia_top);
			this->Controls->Add(this->bt_filter_Sepia_MultiThread);
			this->Controls->Add(this->bt_filter_Sepia);
			this->Controls->Add(this->bt_close_output);
			this->Controls->Add(this->bt_close_copy);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->bt_filter_bw);
			this->Controls->Add(this->bt_copy);
			this->Controls->Add(this->pbox_output);
			this->Controls->Add(this->pbox_copy);
			this->Controls->Add(this->pbox_input);
			this->Controls->Add(this->bt_exit);
			this->Controls->Add(this->bt_close);
			this->Controls->Add(this->bt_open);
			this->Name = L"Pavic_gui_2024_Form";
			this->Text = L"PROJECT: IMPACTLAB LAB 2025";
			this->Load += gcnew System::EventHandler(this, &Pavic_gui_2024_Form::Pavic_gui_2024_Form_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbox_input))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbox_copy))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pbox_output))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void bt_open_Click(System::Object^ sender, System::EventArgs^ e) {
		OpenFileDialog^ ofd = gcnew OpenFileDialog();
		if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			auto start = high_resolution_clock::now();
			Bitmap^ bmp = gcnew Bitmap(ofd->FileName); // Carrega a imagem aqui
			auto end = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(end - start);
			pbox_input->Image = bmp; // Exibe a imagem
			lb_timer->Text = "Open image time: " + duration.count().ToString() + " ms";
			textB_Time->Text = "Impact labd 2025";
		}
	}
	private: System::Void bt_close_Click(System::Object^ sender, System::EventArgs^ e) {

		pbox_input->Image = nullptr;


	}
	private: System::Void bt_copy_Click(System::Object^ sender, System::EventArgs^ e) {
	Bitmap^ inputImage = (Bitmap^)pbox_input->Image;
	if (inputImage != nullptr) {
		auto start = high_resolution_clock::now();
		Bitmap^ copyImage = gcnew Bitmap(inputImage); // Cópia real dos dados
		auto end = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(end - start);
		pbox_copy->Image = copyImage;
		lb_timer->Text = "Copy image time: " + duration.count().ToString() + " ms";
		textB_Time->Text = "Impact labd 2025";
	}

	}
	private: System::Void bt_filter_bw_Click(System::Object^ sender, System::EventArgs^ e) {

		// Get the input image
		Bitmap^ inputImage = (Bitmap^)pbox_input->Image;
		if (inputImage != nullptr) {
			auto start = high_resolution_clock::now();
			// Create a new output image with the same dimensions
			Bitmap^ outputImage = gcnew Bitmap(inputImage->Width, inputImage->Height);

			// Apply the black and white filter
			for (int i = 0; i < inputImage->Width; i++) {
				for (int j = 0; j < inputImage->Height; j++) {
					Color pixelColor = inputImage->GetPixel(i, j);
					int grayValue = (int)(0.299 * pixelColor.R + 0.587 * pixelColor.G + 0.114 * pixelColor.B);
					outputImage->SetPixel(i, j, Color::FromArgb(grayValue, grayValue, grayValue));
				}
			}
			auto end = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(end - start);
			// Display the output image
			pbox_output->Image = outputImage;
			lb_timer->Text = "Filter BW time: " + duration.count().ToString() + " ms";
			textB_Time->Text = "Impact labd 2025";

		}
	}
	private: System::Void bt_exit_Click(System::Object^ sender, System::EventArgs^ e) {
		Application::Exit();
	}
	private: System::Void bt_close_copy_Click(System::Object^ sender, System::EventArgs^ e) {
		pbox_copy->Image = nullptr;

	}
	private: System::Void bt_close_output_Click(System::Object^ sender, System::EventArgs^ e) {
		pbox_output->Image = nullptr;
	}
	private: System::Void Pavic_gui_2024_Form_Load(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void pbox_input_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void bt_filter_Sepia_Click(System::Object^ sender, System::EventArgs^ e) {
		// Get the input image
		Bitmap^ inputImage = (Bitmap^)pbox_input->Image;

		if (inputImage != nullptr) {
			auto start = high_resolution_clock::now();
			// Create a new output image with the same dimensions
			Bitmap^ outputImage = gcnew Bitmap(inputImage->Width, inputImage->Height);

			// Apply the black and white filter
			for (int i = 0; i < inputImage->Width; i++) {
				for (int j = 0; j < inputImage->Height; j++) {
					// Step:1 Get the color of the current pixel
					Color pixelColor = inputImage->GetPixel(i, j);

					// Step : 2  Extract original Red, Green, annd Blue Values
					int r = pixelColor.R;
					int g = pixelColor.G;
					int b = pixelColor.B;
					// Step: 3 Calculate the new pixel values using Sepia Formula
					/*	tr = 0.393R + 0.769G + 0.189B
						tg = 0.349R + 0.686G + 0.168B
						tb = 0.272R + 0.534G + 0.131B
						*/
					double tr = 0.393 * r + 0.769 * g + 0.189 * b;
					double tg = 0.349 * r + 0.686 * g + 0.168 * b;
					double tb = 0.272 * r + 0.534 * g + 0.131 * b;

					// Step : 4 Clamp the values  0- 255 range 
					int newR = Math::Min(255, (int)tr);
					int newG = Math::Min(255, (int)tg);
					int newB = Math::Min(255, (int)tb);

					//outputImage->SetPixel(i, j, Color::FromArgb(tr, tg, tb));
					outputImage->SetPixel(i, j, Color::FromArgb(newR, newG, newB));

					/*	tr = 0.393R + 0.769G + 0.189B
						tg = 0.349R + 0.686G + 0.168B
						tb = 0.272R + 0.534G + 0.131B*/

				}
			}
			auto end = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(end - start);
			// Display the output image
			pbox_output->Image = outputImage;
			lb_timer->Text = "Filter Sepia time: " + duration.count().ToString() + " ms";
			textB_Time->Text = "Impact labd 2025";
		}

	}
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void bt_filter_Sepia_MultiThread_Click(System::Object^ sender, System::EventArgs^ e) {

		// Get the input image
		Bitmap^ inputImage = (Bitmap^)pbox_input->Image;

		// Create a new output image with the same dimensions
		Bitmap^ outputImage = gcnew Bitmap(inputImage->Width, inputImage->Height);

		// Apply the black and white filter
		// inputImage->Width -->> X
		// inputImage->Height -->> Y

		//Image Top !!
		int startY_Top = 0;
		int endY_Top = inputImage->Height / 2;

		//Image Botton !!
		int startY_Botton = inputImage->Height / 2;
		int endY_Botton = inputImage->Height;

		// Image Left
		int startX_left = 0;
		int endX_left = inputImage->Width / 2;

		// Image Right
		int startX_Right = inputImage->Width / 2;
		int endX_Right = inputImage->Width;

		// Start Timer
		auto start = high_resolution_clock::now(); // Start Timer

		//  Filter with threads 

		// Image 1/4
		SepiaThreadArgs* threadArgs = new SepiaThreadArgs(); // Dynamically allocate
		threadArgs->inputImage = inputImage;
		threadArgs->outputImage = outputImage;
		threadArgs->startX = startX_left;
		threadArgs->endX = endX_left;
		threadArgs->startY = startY_Top;
		threadArgs->endY = endY_Top;

		// Image 4/4 
		SepiaThreadArgs* threadArgs_1 = new SepiaThreadArgs(); // Dynamically allocate
		threadArgs_1->inputImage = inputImage;
		threadArgs_1->outputImage = outputImage;
		threadArgs_1->startX = startX_Right;
		threadArgs_1->endX = endX_Right;
		threadArgs_1->startY = startY_Botton;
		threadArgs_1->endY = endY_Botton;

		std::thread t1(ApplySepiaFilterWindow_Unmanaged, threadArgs);
		std::thread t2(ApplyBWFilterWindow_Unmanaged, threadArgs_1);

		t1.join();
		t2.join();
		// Stop Timer
		auto end = high_resolution_clock::now(); // End Timer

		// Calculate duration  
		auto duration = duration_cast<milliseconds>(end - start);
		//::cout << "Read and Write Images Pixel Time: " << duration.count() << " ms" << std::endl;

		//copyStopwatch->Stop();
		lb_timer->Text = "Filter Sepia Multithread time: " + duration.count().ToString() + " ms";
		textB_Time->Text = "Impact labd 2025";

		delete threadArgs, threadArgs_1; // Remember to free the allocated memory


		//pbox_output->Image = outputImage;
		pbox_copy->Image = outputImage;

	}
	private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) {
		Bitmap^ inputImage = (Bitmap^)pbox_input->Image;
		if (inputImage != nullptr) {
			auto start = high_resolution_clock::now();
			Bitmap^ outputImage = gcnew Bitmap(inputImage->Width, inputImage->Height);
			int startY_Botton = inputImage->Height / 2;
			int endY_Botton = inputImage->Height;
			ApplySepiaFilterPartial(inputImage, outputImage, startY_Botton, endY_Botton);
			auto end = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(end - start);
			pbox_output->Image = outputImage;
			lb_timer->Text = "Filter Sepia Bottom time: " + duration.count().ToString() + " ms";
			textB_Time->Text = "Impact labd 2025";
		}
	}
	private: System::Void bt_filter_Sepia_left_Click(System::Object^ sender, System::EventArgs^ e) {
		Bitmap^ inputImage = (Bitmap^)pbox_input->Image;
		if (inputImage != nullptr) {
			auto start = high_resolution_clock::now();
			Bitmap^ outputImage = gcnew Bitmap(inputImage->Width, inputImage->Height);
			int startY_Top = 0;
			int endY_Top = inputImage->Height / 2;
			ApplySepiaFilterPartial(inputImage, outputImage, startY_Top, endY_Top);
			auto end = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(end - start);
			pbox_copy->Image = outputImage;
			lb_timer->Text = "Filter Sepia Top time: " + duration.count().ToString() + " ms";
			textB_Time->Text = "Impact labd 2025";
		}
	}
	private: System::Void bt_filter_Sepia_left_Click_1(System::Object^ sender, System::EventArgs^ e) {

		Bitmap^ inputImage = (Bitmap^)pbox_input->Image;
		if (inputImage != nullptr) {
			auto start = high_resolution_clock::now();
			Bitmap^ outputImage = gcnew Bitmap(inputImage->Width, inputImage->Height);
			int startY_Top = 0;
			int endY_Top = inputImage->Height / 2;
			int startX_left = 0;
			int endX_left = inputImage->Width / 2;
			ApplySepiaFilterWindow(inputImage, outputImage, startX_left, endX_left, startY_Top, endY_Top);
			auto end = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(end - start);
			pbox_copy->Image = outputImage;
			lb_timer->Text = "Tempo filtro Sepia Left: " + duration.count().ToString() + " ms";
			textB_Time->Text = "Impact labd 2025";
		}
	}
	private: System::Void button1_Click_1(System::Object^ sender, System::EventArgs^ e) {
		Bitmap^ inputImage = (Bitmap^)pbox_input->Image;
		if (inputImage != nullptr) {
			auto start = high_resolution_clock::now();
			Bitmap^ outputImage = gcnew Bitmap(inputImage->Width, inputImage->Height);
			int startY_Botton = inputImage->Height / 2;
			int endY_Botton = inputImage->Height;
			int startX_Right = inputImage->Width / 2;
			int endX_Right = inputImage->Width;
			ApplySepiaFilterWindow(inputImage, outputImage, startX_Right, endX_Right, startY_Botton, endY_Botton);
			auto end = high_resolution_clock::now();
			auto duration = duration_cast<milliseconds>(end - start);
			pbox_output->Image = outputImage;
			lb_timer->Text = "Filter Sepia Rigth: " + duration.count().ToString() + " ms";
			textB_Time->Text = "Impact labd 2025";
		}
	}
	private: System::Void bt_filter_Sepia_Thread_Click(System::Object^ sender, System::EventArgs^ e) {

		// Get the input image
		Bitmap^ inputImage = (Bitmap^)pbox_input->Image;

		// Create a new output image with the same dimensions
		Bitmap^ outputImage = gcnew Bitmap(inputImage->Width, inputImage->Height);

		// Apply the black and white filter
		// inputImage->Width -->> X
		// inputImage->Height -->> Y

		//Image Top !!
		int startY_Top = 0;
		int endY_Top = inputImage->Height / 2;

		//Image Botton !!
		int startY_Botton = inputImage->Height / 2;
		int endY_Botton = inputImage->Height;

		// Image Left
		int startX_left = 0;
		int endX_left = inputImage->Width / 2;

		// Image Right
		int startX_Right = inputImage->Width / 2;
		int endX_Right = inputImage->Width;

		// Start Timer
		auto start = high_resolution_clock::now(); // Start Timer

		//  Filter with threads 

		SepiaThreadArgs* threadArgs = new SepiaThreadArgs(); // Dynamically allocate
		threadArgs->inputImage = inputImage;
		threadArgs->outputImage = outputImage;
		threadArgs->startX = startX_left;
		threadArgs->endX = endX_Right;
		threadArgs->startY = startY_Top;
		threadArgs->endY = endY_Botton;

		std::thread t1(ApplySepiaFilterWindow_Unmanaged, threadArgs);

		t1.join();
		// Stop Timer
		auto end = high_resolution_clock::now(); // End Timer

		// Calculate duration  
		auto duration = duration_cast<milliseconds>(end - start);
		//::cout << "Read and Write Images Pixel Time: " << duration.count() << " ms" << std::endl;

		//copyStopwatch->Stop();
		lb_timer->Text = "Filter Sepia Thread time: " + duration.count().ToString() + " ms";
		textB_Time->Text = "Impact labd 2025";

		delete threadArgs; // Remember to free the allocated memory


		pbox_output->Image = outputImage;
	}
		private: System::Void bt_filter_cuda_blur_Click(System::Object^ sender, System::EventArgs^ e) {
			Bitmap^ inputImage = (Bitmap^)pbox_input->Image;
			if (inputImage != nullptr) {
				auto start = high_resolution_clock::now();

				// Bloqueia os bits do bitmap de entrada para leitura
				Rectangle rect = Rectangle(0, 0, inputImage->Width, inputImage->Height);
				BitmapData^ bmpData = inputImage->LockBits(rect, ImageLockMode::ReadOnly, inputImage->PixelFormat);

				// Cria um bitmap de saída
				Bitmap^ outputImage = gcnew Bitmap(inputImage->Width, inputImage->Height, inputImage->PixelFormat);
				BitmapData^ outBmpData = outputImage->LockBits(rect, ImageLockMode::WriteOnly, outputImage->PixelFormat);

				int channels = Bitmap::GetPixelFormatSize(inputImage->PixelFormat) / 8;

				// Chama a função wrapper do CUDA
				apply_cuda_blur(
					(unsigned char*)bmpData->Scan0.ToPointer(),
					(unsigned char*)outBmpData->Scan0.ToPointer(),
					inputImage->Width,
					inputImage->Height,
					channels,
					5 // Raio do blur
				);

				// Desbloqueia os bitmaps
				inputImage->UnlockBits(bmpData);
				outputImage->UnlockBits(outBmpData);

				auto end = high_resolution_clock::now();
				auto duration = duration_cast<milliseconds>(end - start);

				pbox_output->Image = outputImage;
				lb_timer->Text = "Filter CUDA Blur time: " + duration.count().ToString() + " ms";
			}
	}
};
}