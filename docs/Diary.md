Progress Log
============

24/12/12
--------
* I spent today learning about and experimenting with 3D modelling in Blender.  
  Although I made some interesting models and effects, I decided that making 3D models in an art package such as Blender was not the way I wanted to approach this project.

1/1/13
------
* I was learning about what OpenGL was, and how to use it.
* I implemented a “player” program. This would be a program that sets everything up ready for the actual 3D demo to use, and provides a window for the 3D demo to draw into.
* I made two basic OpenGL examples to get to grips with it.  
  My first test, called `test_triangle`, simply draws a white triangle.  
  The second test, called `test_points`, draws some random white points on the screen.  
  I got both tests working on Linux.

8/1/13
------
* I was learning about mesh representations and operations, from [this blog post][ryg halfedge-theory].
* I started my research on fluid dynamics and simulation techniques.  
  I found out that there are three approaches I can take: solving Navier-Stokes (NS) equations directly, using Smoothed Particle Hydrodynamics (SPH), or Lattice Boltzmann (LBM).  
  [This demo][smash numbres] uses SPH. (However, this demo was made to run on machines far powerful than I have access to)  
  Blender uses LBM. (However, Blender's fluid simulation is intended for non-realtime rendering)

12/1/13
-------
* I read about using signed distance fields in [a presentation at GDC 2012][smash gdc], [a presentation at NVScene by iq][iq rwwtt] and [a PDF][upenn raymarch].
* I made a very basic signed distance field test, `test_sdf`. However, this doesn't display the distance field in 3D, but instead a slice through a 2D plane.

13/1/13
-------
* I successfully compiled and tested my program on Windows.
* I started using git to keep track of older versions of my code and my documents.
* I started implementing a mesh representation.

14/1/13
-------
* I read further about Navier-Stokes equations.
* I wrote my rationale, and expanded my progress log with references.
* I started my timeline plan.

15/1/13
-------
Found detailed explanation of Navier-Stokes in [a PDF][fluid rest of us].

18/1/13
-------
Reformatted Rationale, Diary and Timeline as Markdown documents. This makes version control of these documents much easier.


[ryg halfedge-theory]: http://fgiesen.wordpress.com/2012/02/21/half-edge-based-mesh-representations-theory/
[upenn raymarch]: http://www.seas.upenn.edu/~pjia/raymarch/report.pdf
[smash gdc]: http://directtovideo.files.wordpress.com/2012/03/gdc_2012_released.pdf
[smash numbres]: http://directtovideo.wordpress.com/2011/05/03/numb-res/
[iq rwwtt]: http://www.iquilezles.org/www/material/nvscene2008/rwwtt.pdf
[fluid rest of us]: http://people.sc.fsu.edu/~jburkardt/pdf/fluid_flow_for_the_rest_of_us.pdf
