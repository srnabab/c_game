glslc C:\D\c_game\Shaders\triangle.frag -o C:\D\c_game\Shaders\triangle_fragment.spv
glslc C:\D\c_game\Shaders\triangle.vert -o C:\D\c_game\Shaders\triangle_vertex.spv
glslc C:\D\c_game\Shaders\particle.comp -o C:\D\c_game\Shaders\particle_compute.spv
glslc C:\D\c_game\Shaders\particle.vert -o C:\D\c_game\Shaders\particle_vertex.spv
glslc C:\D\c_game\Shaders\particle.frag -o C:\D\c_game\Shaders\particle_fragment.spv
copy C:\D\c_game\Shaders\*.* C:\D\c_game\run\Shaders
exit