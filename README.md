# Learning SDL3

The final goal is actually to learn more about compute shaders. The original plan was to use SDL2 and OpenGL because of past experiences but I found out that SDL3 just got released when I was getting started. SDL3 has a generic, crossplatform GPU library that makes Vulkan much easier to work with. I would have to relearn SDL2 and OpenGL but it did not feel like a good time investment now that we have SDL3 with simplified Vulkan that superseeds both.

*There isn't many good SDL3 tutorials at the moment because SDL3 is new. This is not a tutorial or guide but it might aid someoone else on a similar journey like me. This is just artifacts from my own journey where I try to keep it as simple as possible for my own sake. This is just a repository that reflects how I brake down and do something like this.*

The learning plan currently looks something like this:
1. Create a window with SDL3
2. Draw a triangle with SDL33
3. Draw multiple triangles etc with buffers in SDL3
4. Do some calculations with a compute shader in SDL3

These steps are based on how I have learned graphics in the past. Kind of a best practice, for me. Each step allows me to learn a reasnoable amount of things while raising the difficulty a bit. It might feel like you suddenly have to climb Mount Everest if you are new to graphics programming and that is probably because of a lack of theory. I will have a mountain like this to climb, it will be Vulkan. This is normal for everyone.
