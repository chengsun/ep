Progress Log
============

24/12/12 *(~3 hours)*
--------
* Over the Christmas break, I have decided that I want to do some sort of 3D animation for my Extended Project. The reasons for this are outlined in my rationale.
* I spent today learning about and experimenting with 3D modelling in Blender.  
  Although I made some interesting models and effects, I decided that making 3D models in an art package such as Blender was not the way I wanted to approach this project.

1/1/13 *(~3 hours)*
------
* I have been researching the way I can get 3D things to display on the screen. I learnt about what OpenGL was, and how to use it. Mostly I used [this tutorial][arcsynthesis], which I found to be very useful and thorough.
* I implemented a “player” program. This would be a program that sets everything up ready for the actual 3D demo to use, and provides a window for the 3D demo to draw into.
* I made two basic OpenGL examples to get to grips with using it.  
  My first test, called `test_triangle`, simply draws a white triangle.  
  The second test, called `test_points`, draws some random white points on the screen.  
  I got both tests working on Linux.

8/1/13 *(~1 hour)*
------
* Now that the Christmas break is over, I wanted to start researching the techniques that I will use.
* I was learning about mesh representations and operations, from [this blog post][ryg halfedge-theory].
* I started my research on fluid dynamics and simulation techniques.  
  I found out that there are three approaches I can take: solving Navier-Stokes (NS) equations directly, using Smoothed Particle Hydrodynamics (SPH), or Lattice Boltzmann (LBM).  
  [This demo][smash numbres] uses SPH. (However, this demo was made to run on machines far powerful than I have access to)  
  Blender uses LBM. (However, Blender's fluid simulation is intended for non-realtime rendering)

12/1/13 *(~1 hour)*
-------
* I read about using signed distance fields in [a presentation at GDC 2012][smash gdc], [a presentation at NVScene by iq][iq rwwtt] and [a PDF][upenn raymarch].
* I made a very basic signed distance field test, `test_sdf`. However, this doesn't display the distance field in 3D, but instead a slice through a 2D plane.

13/1/13 *(~2 hours)*
-------
* I successfully compiled and tested my program on Windows.
* I started using git to keep track of older versions of my code and my documents.
* I started implementing a mesh representation based on [this blog post][ryg halfedge-redux]. This will be used for "solid" (i.e. non-fluid) stuff, such as the landscape.

14/1/13 *(~1 hour)*
-------
* I read further about Navier-Stokes equations.
* I wrote my rationale, and expanded my progress log with references.
* I started my timeline plan.

15/1/13 *(~30 minutes)*
-------
* Found detailed explanation of Navier-Stokes in [a PDF][fluid rest of us].

18/1/13 *(~30 minutes)*
-------
* Reformatted Rationale, Diary and Timeline as Markdown documents. This makes version control of these documents much easier.

19/1/13 *(~3 hours)*
-------
* Continued work on mesh representation. I am trying to implement the [vertex split operation][ryg halfedge-practice].

20/1/13 *(~2 hours)*
-------
* Continued work on mesh vertex split. Added `test_mesh` to test mesh operations.  
  I am finding it quite tricky to code the operations correctly from descriptions of them. I have a poor visual memory so I cannot visualise what each operation does to a mesh, especially in 3D. To work around this, I am having to draw out a lot of small diagrams to help me think about all of the special cases.

21/1/13 *(~4 hours)*
-------
* Expanded on my Rationale and Timeline.
* Continued work on mesh vertex split operation.

22/1/13 *(~4 hours)*
-------
* After more work, the mesh vertex splitting operation seems to work now.  
  I still need to test the corner cases.
* I am now trying to implement the [face split operation][ryg halfedge-practice].

23/1/13 *(30 minutes)*
-------
* Added some information to my Rationale.
* Updated my Diary with the amount of time I spent working on my Extended Project each day.  
  I gave approximate times to the days before today, from memory, as best I can. From today onwards I will track the time I spend more closely.
* I started looking at how to go about implementing a scene graph. I am using information from [this OpenGL tutorial][arcsynthesis 17-scenegraph].

24/1/13 *(2 hours)*
-------
* I changed the style of the code slightly, to allow the use of cleaner OO syntax.
  This means that future code will be easier to write and understand.
* I have finished implementing the face split operation; it seems to work now.

25/1/13 *(2 hours)*
-------
* I started implementing mesh rendering, which actually draws the mesh to screen.
  This way I can visually evaluate the results of my code. Up until now I have had to use textual logging when checking of the state of the mesh, which is time-consuming and prone to error.
  I have needed to refer to the documentation on the OpenGL API a lot, which is used for rendering. Most of the information I needed was found in [this tutorial][arcsynthesis] and [the OpenGL wiki][opengl core-api].
  I managed to successfully draw the mesh primitives that I have so far, a 2D n-sided polygon, with `test_mesh`.

27/1/13 *(30 minutes)*
-------
* I am moving all the code which prepared the mesh for rendering, to be associated with a Program (GPU shader) object rather than a Mesh object, as this makes more sense.

28/1/13 *(1 hour)*
-------
* I continued refactoring code.
* I started looking at implementing vector and matrix operations, for transformations such as rotation and scaling.

29/1/13 *(20 minutes)*
-------
* I read about using quaternions to represent 3D rotations through [this tutorial][cprogramming quat].



[ryg halfedge-theory]: http://fgiesen.wordpress.com/2012/02/21/half-edge-based-mesh-representations-theory/
[ryg halfedge-practice]: http://fgiesen.wordpress.com/2012/03/24/half-edge-based-mesh-representations-practice/
[ryg halfedge-redux]: http://fgiesen.wordpress.com/2012/04/03/half-edges-redux/
[upenn raymarch]: http://www.seas.upenn.edu/~pjia/raymarch/report.pdf
[smash gdc]: http://directtovideo.files.wordpress.com/2012/03/gdc_2012_released.pdf
[smash numbres]: http://directtovideo.wordpress.com/2011/05/03/numb-res/
[iq rwwtt]: http://www.iquilezles.org/www/material/nvscene2008/rwwtt.pdf
[fluid rest of us]: http://people.sc.fsu.edu/~jburkardt/pdf/fluid_flow_for_the_rest_of_us.pdf
[arcsynthesis]: http://www.arcsynthesis.org/gltut/
[arcsynthesis 17-scenegraph]: http://www.arcsynthesis.org/gltut/Texturing/Tutorial%2017.html#d0e15853
[opengl core-api]: http://www.opengl.org/wiki/Category:Core_API_Reference
[cprogramming quat]: http://www.cprogramming.com/tutorial/3d/quaternions.html
