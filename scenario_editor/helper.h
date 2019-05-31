#ifndef HELPER_H
#define HELPER_H
#include <iostream>
#include <vector>
#include <math.h>



float fov_area(int height)
{
    float side = height * sqrt(3);
    return (side * side);
}

float pixel_to_meter(float area)
{
    float side = sqrt(area);
    float pixel = 1000/side; // 1000 is the window size need to optimize
    return pixel;
}

float meter_to_pixel(float pixi, float dist)
{
    return (dist/pixi);
}

float distancefromCorner(int x1, int y1)
{
    int x = x1 - 0; //calculating number to square in next step
    int y = y1 - 0;
    float dist;

    dist = pow(x, 2) + pow(y, 2);       //calculating Euclidean distance
    dist = sqrt(dist);

    return dist;
}

float distancefromCenter(int x1, int y1)
{
    int x = x1 - 500;
    int y = y1 - 500;
    float dist;

    dist = pow(x, 2) + pow(y, 2);
    dist = sqrt(dist);
    return dist;
}

float focal_length(int image_width, float fov=120.0)
{
    auto A = image_width/2;
    std::cout<<A<<std::endl;
    auto a = fov/2;
    std::cout<<a<<std::endl;
//    float focal_length = A/tan(a *(3.14/180.0));
//    float focal_length = (image_width/2)*(atan(fov/2));
    float focal_length = image_width/(2*tan(fov*(3.14/360.0)));
    return focal_length;
}

void real_world_points(int width, int height, int mouse_x, int mouse_y, int camera_height)
{
    float inv_K[3][3];
    float mouse[3][0];
    float real_coord[3][0];
    float fl = focal_length(width, 120);
    std::cout<<"Focal Length--------->"<<fl<<'\n';
    auto cu = width/2;
    auto cv = height/2;
    float determinant = 0;
    float K[3][3];
    K[0][0] = fl;
    K[0][1] = 0;
    K[0][2] = cu;
    K[1][0] = 0;
    K[1][1] = fl;
    K[1][2] = cv;
    K[2][0] = 0;
    K[2][1] = 0;
    K[2][2] = 1;


    mouse[0][0] = mouse_x;
    mouse[1][0] = mouse_y;
    mouse[2][0] = 1;

    //finding determinant
    auto up = (K[0][0]*K[1][1]*K[2][2]) + (K[0][1]*K[1][2]*K[2][0]) + (K[0][2]*K[1][0]*K[2][1]);
    auto down = (K[2][0]*K[1][1]*K[0][2]) + (K[2][1]*K[1][2]*K[0][0]) + (K[2][2]*K[1][0]*K[0][1]);
    determinant = up - down;
    std::cout<<"The determinant------> "<< determinant<<'\n';
    // Finding Inverse of K

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            inv_K[i][j] = ((K[(j+1)%3][(i+1)%3] * K[(j+2)%3][(i+2)%3]) - (K[(j+1)%3][(i+2)%3] * K[(j+2)%3][(i+1)%3]))/ determinant;
            std::cout<<inv_K[i][j]<<'\t';
        }
        std::cout<<'\n';
    }

    std::cout<<mouse_x<<'\t'<<mouse_y<<'\n';

    // Now Calculating 3d coordinates

    real_coord[0][0] = ((inv_K[0][0]*mouse[0][0])+(inv_K[0][1]*mouse[1][0])+(inv_K[0][2]*mouse[2][0]));
    real_coord[1][0] = ((inv_K[1][0]*mouse[0][0])+(inv_K[1][1]*mouse[1][0])+(inv_K[1][2]*mouse[2][0]));
    real_coord[2][0] = ((inv_K[2][0]*mouse[0][0])+(inv_K[2][1]*mouse[1][0])+(inv_K[2][2]*mouse[2][0]));

    std::cout<<real_coord[0][0]<<'\t'<<real_coord[1][0]<<'\t'<<real_coord[0][0]<<'\n';



}


void twoD_to_threeD()
{

}

#endif // HELPER_H
