# GraphicsEngine
Graphics Engine using OpenGL and C++

### Current State Of The Project

While I am still working through 'Learn OpenGL' I have not yet had the chance to improve the car rendering just yet. 
But I want to display some of the cool scenes, that could be produced using the contents and tutorials of the aforementioned literature:

***SSAO Scene***

*With Blur*

<img src="https://github.com/user-attachments/assets/39dd5215-065f-4671-9489-c8f61ee2acec" width="540" height="360">

*Without Blur (noisy)*

<img src="https://github.com/user-attachments/assets/af2a6c42-1e69-42c6-a71d-c18f4478cc4f" width="540" height="360">

In this scene I've implementen screen-spaced ambient occlusion. Essentially, we can model soft shadows by taking into account how the geometry around a certain fragment looks.
If the fragment is in a corner for example and therefore more occluded by geometry from receiving light scatters, this fragment will appear darker, have a lower ambient factor. 
The image might seem a bit blurry. That's because ao is caluclated using a 3x3 noise texture (in my case). Applying a blur (3x3 box blur) smoothes this noise patter out. 
In combination with Blinn-Phong lighting we get some really nice results.

---

***Deferred Shading Scene***

<img src="https://github.com/user-attachments/assets/ff96734a-5d6f-4550-a77d-30b42b424bb9" width="540" height="360">


Here you can see several backpack models (imported using assimp). These models are lit by differently colored point lights using the 
Blinn-Phong lighting model with some attenuation on top of that. In this scene I used a deferred renderer (or actually deferred and forward rendering), which 
is an optimization for rendering scenes with multiple light sources. 
A nice addition would be to implement proper bloom (so the light cubes look like actual lights). 


---

***Car Scene***


<img src="https://github.com/philip-seitz/GraphicsEngine/assets/93658810/031b790b-2d82-44b4-ab9a-9e6b477b682c" width="540" height="360">


currently implemented features like:
- perspective (3D)
- back face culling
- camera with movement (lookAT)
- simple form of direct lighting
- rendering certain types of .obj files
- drawing grids of variable sizes
- colored x and y grid lines
- different types of fog (exponential squared in this example)
- Mult Sample Anti Aliasing (MSAA 4x) for models and grid lines

## Time Lapse

older versions:

<img src="https://github.com/philip-seitz/GraphicsEngine/assets/93658810/10c45a73-42b7-4f07-a6ae-02af7363c2e9" width="540" height="360">
