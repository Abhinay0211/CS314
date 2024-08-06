#include <iostream>
#include "libppm.h"
#include <cstdint>
#include <cmath>
using namespace std;

struct image_t* S1_smoothen(struct image_t *input_image)
{
    struct image_t* smooth_image = new image_t;
    int height = input_image->height;
    int width = input_image->width;

    smooth_image->height = height;
    smooth_image->width = width;
    smooth_image->image_pixels = new uint8_t**[height];
    for(int i = 0; i < height; i++) {
        smooth_image->image_pixels[i] = new uint8_t*[width];
        for(int j = 0; j < width; j++)
            smooth_image->image_pixels[i][j] = new uint8_t[3];
    }

    uint8_t*** pixels = input_image->image_pixels;
    uint8_t*** smooth = smooth_image->image_pixels;

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            for(int k = 0; k < 3; k++) {
                if(i == 0 && j == 0) {
                    // Top-left corner
                    smooth[i][j][k] = (pixels[i][j][k] + pixels[i+1][j][k] + pixels[i][j+1][k] + pixels[i+1][j+1][k]) / 4;
                }
                else if(i == 0 && j == width-1) {
                    // Top-right corner
                    smooth[i][j][k] = (pixels[i][j][k] + pixels[i+1][j][k] + pixels[i][j-1][k] + pixels[i+1][j-1][k]) / 4;
                }
                else if(i == height-1 && j == 0) {
                    // Bottom-left corner
                    smooth[i][j][k] = (pixels[i][j][k] + pixels[i-1][j][k] + pixels[i][j+1][k] + pixels[i-1][j+1][k]) / 4;
                }
                else if(i == height-1 && j == width-1) {
                    // Bottom-right corner
                    smooth[i][j][k] = (pixels[i][j][k] + pixels[i-1][j][k] + pixels[i][j-1][k] + pixels[i-1][j-1][k]) / 4;
                }
                else if(i == 0) {
                    // Top edge
                    smooth[i][j][k] = (pixels[i][j-1][k] + pixels[i][j][k] + pixels[i][j+1][k] +
                                       pixels[i+1][j-1][k] + pixels[i+1][j][k] + pixels[i+1][j+1][k]) / 6;
                }
                else if(i == height-1) {
                    // Bottom edge
                    smooth[i][j][k] = (pixels[i][j-1][k] + pixels[i][j][k] + pixels[i][j+1][k] +
                                       pixels[i-1][j-1][k] + pixels[i-1][j][k] + pixels[i-1][j+1][k]) / 6;
                }
                else if(j == 0) {
                    // Left edge
                    smooth[i][j][k] = (pixels[i-1][j][k] + pixels[i][j][k] + pixels[i+1][j][k] +
                                       pixels[i-1][j+1][k] + pixels[i][j+1][k] + pixels[i+1][j+1][k]) / 6;
                }
                else if(j == width-1) {
                    // Right edge
                    smooth[i][j][k] = (pixels[i-1][j][k] + pixels[i][j][k] + pixels[i+1][j][k] +
                                       pixels[i-1][j-1][k] + pixels[i][j-1][k] + pixels[i+1][j-1][k]) / 6;
                }
                else {
                    // Center
                    smooth[i][j][k] = (pixels[i-1][j-1][k] + pixels[i-1][j][k] + pixels[i-1][j+1][k] +
                                       pixels[i][j-1][k] + pixels[i][j][k] + pixels[i][j+1][k] +
                                       pixels[i+1][j-1][k] + pixels[i+1][j][k] + pixels[i+1][j+1][k]) / 9;
                }
            }
        }
    }

    return smooth_image;
}


struct image_t* S2_find_details(struct image_t *input_image, struct image_t *smoothened_image)
{
	int height = input_image->height;
	int width=input_image->width;
	struct image_t *details_image = new image_t;
	details_image->height=height;
	details_image->width=width;
	details_image->image_pixels = new uint8_t**[height];
	for(int i = 0; i <height; i++){
		details_image->image_pixels[i] = new uint8_t*[width];
		for(int j = 0; j < width; j++){
			details_image->image_pixels[i][j] = new uint8_t[3];
		}
	}
	uint8_t*** details = details_image->image_pixels;
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			for(int k=0;k<3;k++){
				details[i][j][k] = (abs)(input_image->image_pixels[i][j][k] -smoothened_image->image_pixels[i][j][k]);
			}
		}
	}
	return details_image;
}

struct image_t* S3_sharpen(struct image_t *input_image, struct image_t *details_image)
{
	// TODO
	int height = input_image->height;
	int width=input_image->width;
	struct image_t *sharpen_image = new image_t;
	sharpen_image->height=height;
	sharpen_image->width=width;
	sharpen_image->image_pixels = new uint8_t**[height];
	for(int i = 0; i <height; i++){
		sharpen_image->image_pixels[i] = new uint8_t*[width];
		for(int j = 0; j < width; j++){
			sharpen_image->image_pixels[i][j] = new uint8_t[3];
		}
	}
	uint8_t*** sharpen = sharpen_image->image_pixels;
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			for(int k=0;k<3;k++){
				int x = input_image->image_pixels[i][j][k] + details_image->image_pixels[i][j][k];
				if(x > 255){
					sharpen[i][j][k] = 255;
				}
				else{
					sharpen[i][j][k] = (uint_8)x;
				}
			}
		}
	}
	return sharpen_image;
//	return input_image; //TODO remove this line when adding your code
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		cout << "usage: ./a.out <path-to-original-image> <path-to-transformed-image>\n\n";
		exit(0);
	}
	
	struct image_t *input_image = read_ppm_file(argv[1]);
	
	struct image_t *smoothened_image = S1_smoothen(input_image);
	
	struct image_t *details_image = S2_find_details(input_image, smoothened_image);
	
	struct image_t *sharpened_image = S3_sharpen(input_image, details_image);
	
	write_ppm_file(argv[2], sharpened_image);
	
	return 0;
}
