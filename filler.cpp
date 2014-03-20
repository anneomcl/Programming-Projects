#include "filler.h"
#include <math.h>

animation filler::dfs::fillSolid( PNG & img, int x, int y, 
        RGBAPixel fillColor, int tolerance, int frameFreq ) 
{
	solidColorPicker solidFill(fillColor);

	return filler::fill<Stack>(img, x, y, solidFill, tolerance, frameFreq);
}

animation filler::dfs::fillGrid( PNG & img, int x, int y, 
        RGBAPixel gridColor, int gridSpacing, int tolerance, int frameFreq ) 
{
	gridColorPicker gridFill(gridColor, gridSpacing);

	return filler::fill<Stack>(img, x, y, gridFill, tolerance, frameFreq);
}

animation filler::dfs::fillGradient( PNG & img, int x, int y, 
        RGBAPixel fadeColor1, RGBAPixel fadeColor2, int radius, 
        int tolerance, int frameFreq ) 
{
	gradientColorPicker gradientFill(fadeColor1, fadeColor2, radius, x, y);

	return filler::fill<Stack>(img, x, y, gradientFill, tolerance, frameFreq);
}

animation filler::dfs::fill( PNG & img, int x, int y, 
        colorPicker & fillColor, int tolerance, int frameFreq ) 
{
	return filler::fill<Stack>(img, x, y, fillColor, tolerance, frameFreq);
}

animation filler::bfs::fillSolid( PNG & img, int x, int y, 
        RGBAPixel fillColor, int tolerance, int frameFreq ) 
{
	solidColorPicker solidFill(fillColor);

	return filler::fill<Queue>(img, x, y, solidFill, tolerance, frameFreq);
}

animation filler::bfs::fillGrid( PNG & img, int x, int y, 
        RGBAPixel gridColor, int gridSpacing, int tolerance, int frameFreq ) 
{
	gridColorPicker gridFill(gridColor, gridSpacing);

	return filler::fill<Queue>(img, x, y, gridFill, tolerance, frameFreq);
}

animation filler::bfs::fillGradient( PNG & img, int x, int y, 
        RGBAPixel fadeColor1, RGBAPixel fadeColor2, int radius, 
        int tolerance, int frameFreq ) 
{
	gradientColorPicker gradientFill(fadeColor1, fadeColor2, radius, x, y);

	return filler::fill<Queue>(img, x, y, gradientFill, tolerance, frameFreq);
}

animation filler::bfs::fill( PNG & img, int x, int y, 
        colorPicker & fillColor, int tolerance, int frameFreq ) 
{
	return filler::fill<Queue>(img, x, y, fillColor, tolerance, frameFreq);
}

template <template <class T> class OrderingStructure>
animation filler::fill( PNG & img, int x, int y,
        colorPicker & fillColor, int tolerance, int frameFreq ) 
{
	OrderingStructure<RGBAPixel> sq; //pixel structure
	OrderingStructure<int> sqX; //Xcoord structure
	OrderingStructure<int> sqY; //Ycoord structure
	int** marks = new int *[img.width()]; //array of pointers with size width 0 1 2 3
	for(int i = 0; i < img.width(); i++) //array of pointers to arrays of size height x = 0->y = 0 1
	{
		marks[i] = new int[img.height()];
		for(int j = 0; j < img.height(); j++) //set coordinates to mark values e.g 0,1 = 0
			marks[i][j] = 0;
	}

	int count = 0; //count when to add new frame
	animation fillAnimation; //animation to return

	RGBAPixel origin = (*img(x,y));
	sq.add(origin);
	sqX.add(x);
	sqY.add(y);

	while(!sq.isEmpty())
	{
		RGBAPixel pixel = sq.remove();
		int xCoord = sqX.remove();
		int yCoord = sqY.remove();

		int originRed = origin.red, pixelRed = pixel.red, 
			originBlue = origin.blue, pixelBlue = pixel.blue,
			originGreen = origin.green, pixelGreen = pixel.green; 

		int toleranceValue = pow((originRed - pixelRed), 2) + 
			pow((originBlue - pixelBlue), 2) + 
			pow((originGreen - pixelGreen), 2);

		if(marks[xCoord][yCoord] == 0 && toleranceValue <= tolerance)
		{	

			marks[xCoord][yCoord] = 1;
			count++;
			*img(xCoord,yCoord) = fillColor(xCoord,yCoord);

			if(count == frameFreq)
			{
				fillAnimation.addFrame(img);
				count = 0;
			}
		
		
			if(xCoord < img.width() - 1)
			{
				sqX.add(xCoord + 1);
				sqY.add(yCoord);
				sq.add(*img(xCoord+1,yCoord));
			}

			if(yCoord < img.height() - 1)
			{
				sqX.add(xCoord);
				sqY.add(yCoord+1);
				sq.add(*img(xCoord,yCoord+1));
			}

			if(xCoord - 1 >= 0)
			{
				sqX.add(xCoord-1);
				sqY.add(yCoord);
				sq.add(*img(xCoord-1,yCoord));
			}

			if(yCoord - 1 >= 0)	
			{
				sqX.add(xCoord);
				sqY.add(yCoord-1);
				sq.add(*img(xCoord,yCoord-1));
			}
		}
	}
    return fillAnimation;
}
