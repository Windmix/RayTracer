#include <stdio.h>
#include "window.h"
#include "vec3.h"
#include "raytracer.h"
#include "sphere.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include "Bvh.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



#define degtorad(angle) angle * MPI / 180

// Function to write PGM image
void WritePNG(const std::vector<Color>& framebuffer, int width, int height, const std::string& filename) 
{
    std::vector<unsigned char> imageData(width * height * 3);  // RGB format (3 bytes per pixel)

    // Fill the image data with the framebuffer
    for (int i = 0; i < width * height; i++) 
    {
        imageData[i * 3 + 0] = static_cast<unsigned char>(std::min(255.0f, framebuffer[framebuffer.size()-i-1].r * 255.0f)); // Red
        imageData[i * 3 + 1] = static_cast<unsigned char>(std::min(255.0f, framebuffer[framebuffer.size()-i-1].g * 255.0f)); // Green
        imageData[i * 3 + 2] = static_cast<unsigned char>(std::min(255.0f, framebuffer[framebuffer.size()-i-1].b * 255.0f)); // Blue
    }

    // Write the image to a PNG file
    stbi_write_png(("./images/" + filename).c_str(), width, height, 3, imageData.data(), width * 3);

}


int main(int argc, char** argv)
{ 

    // Check if the correct number of arguments is passed
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <width> <height> <rays_per_pixel> <ammountOfSphere> [output_filename]" << std::endl;
        return 1;
    }

    // Parse command-line arguments
    int width = std::stoi(argv[1]);        // Image width
    int height = std::stoi(argv[2]);       // Image height
    int raysPerPixel = std::stoi(argv[3]); // Rays per pixel
    int ammountOfSpheres = std::stoi(argv[4]); // ammount of spheres;
    int maxBounces = 5;

    std::string outputFilename = (argc > 5) ? argv[5] : "output.png"; //output file, defaults to "output.png"

    std::cout << "Rendering image with the following parameters:" << std::endl;
    std::cout << "Width: " << width << ", Height: " << height << ", Rays per pixel: " << raysPerPixel <<", Ammount of spheres: " << ammountOfSpheres << std::endl;
    std::cout << "Output filename: " << outputFilename <<"\n\nrendering........" << std::endl;

    // Define the size of the image for benchmarking
    std::vector<Color> framebuffer;

    framebuffer.resize(width * height);

   

    Raytracer rt = Raytracer(width, height, framebuffer, raysPerPixel, maxBounces);



    // Create some objects
    Material* mat = new Material();
    mat->type = "Lambertian";
    mat->color = { 0.5,0.5,0.5 };
    mat->roughness = 0.3;
    Sphere* ground = new Sphere(1000, { 0,-1000, -1 }, mat);

    rt.AddObject(ground);

    const vec3 boundsMin = { -50.0f, 0.0f, -100.0f };
    const vec3 boundsMax = { 50.0f, 50.0f, 20.0f };

    for (int it = 0; it < ammountOfSpheres; it++)
    {
        Material* mat = new Material();
        mat->type = "Lambertian";
        float r = RandomFloat();
        float g = RandomFloat();
        float b = RandomFloat();
        mat->color = { r, g, b };
        mat->roughness = RandomFloat();
        const float span = 10.0f;

        vec3 pos =
        {
            boundsMin.x + RandomFloat() * (boundsMax.x - boundsMin.x),  // x between -50 and 50
            boundsMin.y + RandomFloat() * (boundsMax.y - boundsMin.y),  // y between 0 and 50
            boundsMin.z + RandomFloat() * (boundsMax.z - boundsMin.z)   // z between -100 and 20
        };

        Sphere* ground = new Sphere(RandomFloat() * 1.5f + 0.5f, pos, mat);
        rt.AddObject(ground);

    }

    // Step 3: Build the BVH for optimization
    BVH bvh;
    bvh.build(rt.objects);

    // camera
    vec3 camPos = { 0.0f, 10.0f, 0.0f };

    float rotx = 0;
    float roty = 0;

    mat4 xMat = (rotationx(rotx));
    mat4 yMat = (rotationy(roty));

    mat4 cameraRotTransform = multiply(yMat, xMat);           // Apply rotation
    mat4 translationMatrix = translation(camPos);
    mat4 cameraMatrix = multiply(cameraRotTransform, translationMatrix);
    rt.SetViewMatrix(cameraMatrix);


    auto t0 = std::chrono::high_resolution_clock::now();
    rt.MultiThreadingRayTraceWithBVH(bvh.root,32);
    auto t1 = std::chrono::high_resolution_clock::now();

    double frameTime = std::chrono::duration<double, std::milli>(t1 - t0).count();

    // Calculate rays per second (MRays/s)
    double totalRays = rt.totalRaytracers;  // Total number of rays
    double totalTime = frameTime / 1000.0;  // Convert milliseconds to seconds
    double mRaysPerSecond = totalRays / totalTime / 1e6;  // Mega-rays per second


    std::cout << "render complete, render time: " << totalTime << " s\n";
    std::cout << "total Ammount Of Ray Tracers: " << totalRays << std::endl;
    std::cout << "Mega-rays per second: " << mRaysPerSecond << " MRays/s" << std::endl;

    WritePNG(framebuffer, width, height, outputFilename);


    std::cout << "Image saved to: " << outputFilename << std::endl;

    return 0;
} 