This project implements a basic ray tracer that renders a 3D scene made up of boxes and spheres. An OpenGL renderer is also provided for debugging purposes. The ray tracer performs Phong shading with ambient, diffuse, and specular lighting. It also computes shadows by casting shadow rays and supports reflections with a recursion limit (bounce) to avoid infinite bouncing.

Shading and Shadows:
Calculates color using ambient, diffuse, and specular components. A shadow ray (with a small bias) is cast from the hit point toward each light to determine if the point is in shadow.

Reflections:
If a material is reflective, the ray tracer casts a reflection ray. The final color is a blend of direct illumination and reflected light. A bounce parameter (default 5) limits the number of reflections.

Contributors:
Tingrui Zhang
Ruixuan Yang
