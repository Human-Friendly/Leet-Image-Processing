// # 1337Coding # Martin Angelov 06.14.2020 # ----> Leet Image Processing example

#include <bitmap.h>
#include <imgproc/imgproc.h>

// ------------------------------------------------------------------------------------------------
//
//

void main()
{
	ImageProcessing imgproc;

	Point3f pos,
		rot;

	float val;

	std::cout << "Enter the wanted position as X, Y, Z coordinates." << std::endl;

	std::cin >> val;
	pos.setX(val);
	std::cin >> val;
	pos.setY(val);
	std::cin >> val;
	pos.setZ(val);

	std::cout << "Enter the wanted orientation as X, Y, Z axis angles." << std::endl;

	std::cin >> val;
	rot.setX(val);
	std::cin >> val;
	rot.setY(val);
	std::cin >> val;
	rot.setZ(val);

	std::cout << "Processing the image..." << std::endl;

	// Reading the image
	BMP input = BMP("testInput.bmp");
	
	// Creating the buffers ---->
	std::vector<Color> inputData;

	int width = input.bmp_info_header.width;
	int height = input.bmp_info_header.height;

	int size = width * height;
	
	int counter = 0;

	bool hasAlpha = input.bmp_info_header.bit_count == 32;

	for (size_t i = 0; i < size; i++)
	{
		unsigned char red = input.data[counter++];
		unsigned char green = input.data[counter++];
		unsigned char blue = input.data[counter++];

		if (hasAlpha)
			counter++;

		inputData.push_back(Color(red, green, blue));
	}

	ColorBuffer inputBuffer = ColorBuffer(inputData, width, height),
		outputBuffer;	
	// Creating the buffers <----

	// Warp the image in perspective projection via position and orientation
	imgproc.warpPerspective(pos, rot, inputBuffer, outputBuffer);

	// Save the image to bmp ---->
	BMP output = BMP(outputBuffer.getWidth(), outputBuffer.getHeight(), false);

	std::vector<Color> outputData = outputBuffer.GetData();

	counter = 0;

	for (size_t y = 0; y < outputBuffer.getHeight(); y++)
	{
		for (size_t x = 0; x < outputBuffer.getWidth(); x++)
			output.set_pixel(x, y, outputData[counter].red, outputData[counter].green, outputData[counter++].blue, 0);
	}

	output.write("testOutput.bmp");
	// Save the image to bmp <----
}