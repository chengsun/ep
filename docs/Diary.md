Progress Log
============

24/12/12 *(~3 hours)*
--------
* Over the Christmas break, I have decided that I want to do some sort of 3D
  animation for my Extended Project. The reasons for this are outlined in my
  rationale.
* I spent today learning about and experimenting with 3D modelling in Blender.  
  Although I made some interesting models and effects, I decided that making 3D
  models in an art package such as Blender was not the way I wanted to approach
  this project.

1/1/13 *(~3 hours)*
------
* I have been researching the way I can get 3D things to display on the screen.
  I learnt about what OpenGL was, and how to use it. Mostly I used [this
  tutorial][arcsynthesis], which I found to be very useful and thorough.
* I implemented a “player” program. This would be a program that sets
  everything up ready for the actual 3D demo to use, and provides a window for
  the 3D demo to draw into.
* I made two basic OpenGL examples to get to grips with using it.  
  My first test, called `test_triangle`, simply draws a white triangle.  
  The second test, called `test_points`, draws some random white points on the
  screen. I got both tests working on Linux.

8/1/13 *(~1 hour)*
------
* Now that the Christmas break is over, I wanted to start researching the
  techniques that I will use.
* I was learning about mesh representations and operations, from [this blog
  post][ryg halfedge-theory].
* I started my research on fluid dynamics and simulation techniques.  
  I found out that there are three approaches I can take: solving Navier-Stokes
  (NS) equations directly, using Smoothed Particle Hydrodynamics (SPH), or
  Lattice Boltzmann (LBM).  
  [This demo][smash numbres] uses SPH. (However, this demo was made to run on
  machines far powerful than I have access to)  
  Blender uses LBM. (However, Blender's fluid simulation is intended for
  non-realtime rendering)

12/1/13 *(~1 hour)*
-------
* I read about using signed distance fields in [a presentation at GDC
  2012][smash gdc], [a presentation at NVScene by iq][iq rwwtt] and [a
  PDF][upenn raymarch].
* I made a very basic signed distance field test, `test_sdf`. However, this
  doesn't display the distance field in 3D, but instead a slice through a 2D
  plane.  
  ![SDF test](https://raw.github.com/chengsun/ep/master/docs/images/120113_sdf.png)

13/1/13 *(~2 hours)*
-------
* I successfully compiled and tested my program on Windows.
* I started using git to keep track of older versions of my code and my
  documents.
* I started implementing a mesh representation based on [this blog post][ryg
  halfedge-redux]. This will be used for "solid" (i.e. non-fluid) stuff, such
  as the landscape.

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
* Reformatted Rationale, Diary and Timeline as Markdown documents. This makes
  version control of these documents much easier.

19/1/13 *(~3 hours)*
-------
* Continued work on mesh representation. I am trying to implement the [vertex
  split operation][ryg halfedge-practice].

20/1/13 *(~2 hours)*
-------
* Continued work on mesh vertex split. Added `test_mesh` to test mesh
  operations.  
  I am finding it quite tricky to code the operations correctly from
  descriptions of them. I have a poor visual memory so I cannot visualise what
  each operation does to a mesh, especially in 3D. To work around this, I am
  having to draw out a lot of small diagrams to help me think about all of the
  special cases.

21/1/13 *(~4 hours)*
-------
* Expanded on my Rationale and Timeline.
* Continued work on mesh vertex split operation.

22/1/13 *(~4 hours)*
-------
* After more work, the mesh vertex splitting operation seems to work now.  
  I still need to test the corner cases.
* I am now trying to implement the [face split operation][ryg
  halfedge-practice].

23/1/13 *(30 minutes)*
-------
* Added some information to my Rationale.
* Updated my Diary with the amount of time I spent working on my Extended
  Project each day.  
  I gave approximate times to the days before today, from
  memory, as best I can. From today onwards I will track the time I spend more
  closely.
* I started looking at how to go about implementing a scene graph. I am using
  information from [this OpenGL tutorial][arcsynthesis 17-scenegraph].

24/1/13 *(2 hours)*
-------
* I changed the style of the code slightly, to allow the use of cleaner OO
  syntax.
  This means that future code will be easier to write and understand.
* I have finished implementing the face split operation; it seems to work now.

25/1/13 *(2 hours)*
-------
* I started implementing mesh rendering, which actually draws the mesh to
  screen. This way I can visually evaluate the results of my code. Up until
  now I have had to use textual logging when checking of the state of the mesh,
  which is time-consuming and prone to error.  
  I have needed to refer to the
  documentation on the OpenGL API a lot, which is used for rendering. Most of
  the information I needed was found in [this tutorial][arcsynthesis] and [the
  OpenGL wiki][opengl core-api].  
  I managed to successfully draw the mesh
  primitives that I have so far, a 2D n-sided polygon, with `test_mesh`.
* I sent out an email to ryg, the author of the half-edge mesh representation
  blog series that I have been using, to ask whether he could give me some
  information about extrusion. This topic was on his list of blog posts to
  write, but it seems that he hasn't gotten round to it yet. Hopefully I'll get
  a response, but if not I will look elsewhere for information.

27/1/13 *(30 minutes)*
-------
* I am moving all the code which prepared the mesh for rendering, to be
  associated with a Program (GPU shader) object rather than a Mesh object, as
  this makes more sense.

28/1/13 *(1 hour)*
-------
* I continued refactoring code.
* I started looking at implementing vector and matrix operations, for
  transformations such as rotation and scaling.

29/1/13 *(20 minutes)*
-------
* I read about using quaternions to represent 3D rotations through [this
  tutorial][cprogramming quat].

30/1/13 *(3 hours)*
-------
* I started writing the vector and matrix code.
  I am referring to my FP4 maths class notes, and also for implementation
  details I am gaining inspiration by reading the code from an existing
  vector/matrix library called [GLM][glm].
* Whilst writing the code, I encountered some problems...  
  `../include/types.h:244:48: error: could not convert ‘{{((((float)(&(&
  a)->Mat2::operator[](0ul))->Vec2::operator[](0ul)) * ((float)(&(&
  b)->Mat2::operator[](0ul))->Vec2::operator[](0ul))) + (((float)(&(&
  a)->Mat2::operator[](1ul))->Vec2::operator[](0ul)) * ((float)(&(&
  b)->Mat2::operator[](0ul))->Vec2::operator[](1ul)))), ((((float)(&(&
  a)->Mat2::operator[](0ul))->Vec2::operator[](1ul)) * ((float)(&(&
  b)->Mat2::operator[](0ul))->Vec2::operator[](0ul))) + (((float)(&(&
  a)->Mat2::operator[](1ul))->Vec2::operator[](1ul)) * ((float)(&(&
  b)->Mat2::operator[](0ul))->Vec2::operator[](1ul))))}, {((((float)(&(&
  a)->Mat2::operator[](0ul))->Vec2::operator[](0ul)) * ((float)(&(&
  b)->Mat2::operator[](1ul))->Vec2::operator[](0ul))) + (((float)(&(&
  a)->Mat2::operator[](1ul))->Vec2::operator[](0ul)) * ((float)(&(&
  b)->Mat2::operator[](1ul))->Vec2::operator[](1ul)))), ((((float)(&(&
  a)->Mat2::operator[](0ul))->Vec2::operator[](1ul)) * ((float)(&(&
  b)->Mat2::operator[](1ul))->Vec2::operator[](0ul))) + (((float)(&(&
  a)->Mat2::operator[](1ul))->Vec2::operator[](1ul)) * ((float)(&(&
  b)->Mat2::operator[](1ul))->Vec2::operator[](1ul))))}}’ from ‘<brace-enclosed
  initializer list>’ to ‘Mat2’`  
  Fixing the error was rather trivial, once I realised that the best thing to
  do was not to bother figuring out the error message!

31/1/13 *(~30 minutes)*
-------
* Some minor tweaks to the mesh ring generator, and split a header file to
  speed up compilation.

3/2/13 *(30 minutes)*
------
* No work done this weekend, because of a mock test tomorrow.
* Previously I have been concerned that I did not have powerful enough hardware
  on which to develop my demo. That has been somewhat alleviated by some
  comments by [navis][navis 2007] on the fact that he developed Lifeforce (a
  very impressive and large-scale demo) on weaker hardware than mine, by
  reducing the rendering resolution. However, hopefully I will be able to find
  (borrow?) some hardware to present the finished demo in higher quality.
* On the other hand, I am also becoming concerned about simulating my
  waterfall. If I were to use a full particle simulation of the fluid, I would
  need to simulate each of very many particles. However, particle simulation in
  realtime is very hard especially with the amount of particles I would need,
  [according to smash][smash numbres], who made an entire demo revolving around
  fluid simulation on high-end machines. Furthermore, unlike smash, I do not
  have the hardware to do anything extremely sophisticated.  
  Even if I do get a particle simulation working, rendering the particles and
  making it look like a fluid would be a challenge all in itself. Looking at
  the ["state of the art" in actually rendering these particles][hoetzlein
  surface-reconstruction], the techniques either look unrealistic or don't run
  in real time.  
  The only way that I can envision the waterfall simulation succeeding is by
  cheating a lot. This means not strictly obeying physics and using "common
  sense physics" and graphical effects to trick the viewer. This is something I
  will have to seriously consider this month. If it does not show signs of
  plausibility soon, I may have to replace the water simulation idea.

19/2/13 *(30 minutes)*
-------
* I am very concerned about the lack of progress over the past two weeks.  
  The past week (the half-term break) was spent mostly in bed, as I have been
  rather ill. This has annoyingly meant that I have not been able to make use of
  the plenty of time that the break provided me with.  
  I hope that this break will reinstill enthusiasm in my extended project over
  the next few weeks, as I have a lot to do in order to reach my target this
  month.
* I have decided that I am going to write an end-of-month review every month,
  to summarise the work that I have done in that month, evaluate whether my
  targets have been reached, and look forwards into the future to see what I am
  aiming to achieve next month.  
  I am starting to write the January report today. (I think this is OK, seeing
  as I haven't done much in February so far...)
* From the advice that we've been getting in the weekly extended project
  periods, it is becoming clear that I need to take more careful note of which
  sources I am using, and what I am gathering from each of them. In order to
  achieve this I am going to aim to write a document about each technique that
  I find from my sources, whether I use it or not in the final project.
* On a more positive note, I have started to decide how fluid simulation is
  going to work in my demo.  
  I would like to simulate two things: a waterfall in the side of a cliff, and
  a vast ocean below, stretching as far as the eye (or camera) can see.  
  Originally I was thinking of simulating everything with millions of
  "particles," which represent small units of water that interact with each
  other to create the effects that we expect such as ripples and waves.
  However, this is obviously not feasible for the ocean, which would require
  billions of particles.  
  Now I am thinking of separating the waterfall and the ocean. The waterfall
  will still be particle-based, however the ocean will instead be a big 2D
  plane, and I will keep track of the water level at lots of points on that
  plane. In this way I can simulate wave behaviour using the concepts that we
  are learning in physics, such as Huygen's principle, and I will not have to
  deal with billions of particles. This way, the ocean will be easier to render
  as well.  
  I will explain my thoughts in a more detailed write-up separately.

22/2/13 *(10 minutes)*
-------
* I found an explanation of how to perform a wave simulation in a
  two-dimensional grid [here][gamedev water-effect]. I also found [a Java
  applet by Paul Falstad][falstad ripple] which simulates waves; the source
  code is kindly provided so I am reading through to figure out how the
  simulation is performed.

4/3/13 *(1 hour)*
------
* I continued writing my January review. I also finished my February review.
  (Chronology? What's that?)

6/3/13 *(2 hours)*
------
* I worked on the wave simulation.

7/3/13 *(3 hours)*
------
* After some fiddling, I have a working wave simulation.
* I have been reading about using textures in OpenGL, so that I can see the
  results of my wave simulation. I have successfully written the texture code,
  and also wrote a scene called `test_wave` that tests all of this.  
  The texture code will come in very useful later as well.  
  ![Wave simulation and 2D visualisation](https://raw.github.com/chengsun/ep/master/docs/images/070313_wave2D.png)

29/3/13 *(3 hours)*
-------
* I have been researching on and off for the past few weeks. Unfortunately I
  have not noted these times in my diary.
* I have fixed edges so that they no longer reflect waves, by damping.
* I am working on improving wave simulation performance. For a 256x256 grid,
  simulation has improved from 60 to 90 fps.

30/3/13 *(1 hour)*
-------
* More work on wave simulation (100 fps attained).
* I sent out an email to Paul Falstad with some queries about the mathematical
  basis of his simulation code (the concepts of which I am using).

31/3/13 *(1 hour)*
-------
* More work on wave simulation

20/4/13
-------
* I've been reading lots of online papers on fluid simulation.
* Researched cloth simulation using verlet integration.
* Found implementation in JavaScript at [this website][subprotocol verlet].

27/4/13
-------
* Looked at efficient collision detection with octrees.

28/4/13
-------
* After a review of progress so far, I am scaling down the ambition of my
  project to just two things that I want to demonstrate: cloth simulation using
  verlet integration, and wave simulation using discrete grid-based methods.
* I have updated the Timeline to reflect this.
* I have written an end of April review.
* I have started exploring 3D transformations, lighting and shading using
  vectors and matrices. I have written a quick prototype demonstrating these
  three things using JavaScript. The program displays a rotating cube that
  experiences flat shading from a single light source at infinity. This can be
  found under `experiments/cube.html`.  
  ![3D transformations and lighting](https://raw.github.com/chengsun/ep/master/docs/images/280413_cube.png)

29/4/13
-------
* I'm looking at rendering to a texture using an FBO. The documents that I am
  using is [this website][gamedev framebuffer101].

30/4/13
-------
* Updated Timeline with more granular targets and estimates of time that it
  will take.
* Updated and finished end of April review.

6/5/13 *(1 hour)*
------
* Abstracted shader construction from compilation

7/5/13 *(2 hours)*
------
* Started working on 3D grid mesh.

8/5/13 *(2 hours)*
------
* Continued working on 3D grid mesh.
* Fixed vector determinant calculation, and implemented transpose.

31/5/13 *(3 hours)*
-------
* Worked on FBOs

11/6/13 *(1 hour)*
-------
* Researched text rendering in OpenGL. Used a [tutorial][sdltutorials sdl-ttf]
  and the [official SDL-ttf docs][sdl-ttf docs].
* Read paper by Valve on [smooth text rendering using SDFs][valve sdf-mag].

12/6/13 *(1 hour)*
-------
* Started implementing text rendering.

17/6/13 *(2 hours)*
-------
* Moved to using [GLM][glm] for vector and matrices. This is a well-tested
  library and is used in many projects already.
* Updated Timeline

19/6/13 *(30 minutes)*
-------
* Fixed a bug in mesh code.

25/6/13 *(1 hour)*
-------
* Continued working on mesh grid code.

28/6/13 *(3 hours)*
-------
* Found a [web page][democritis verlet] as a reference for the Verlet
  algorithm.
* In order to test the algorithm, I wrote a prototype verlet integration
  physics engine in JavaScript. It moves one end of a ribbon around, leaving
  the other end free to interact with physics. This experiment can be found
  under `experiments/verletrag.html`.  
  ![Verlet integration on a ribbon](https://raw.github.com/chengsun/ep/master/docs/images/280613_verlet.png)

30/6/13 *(3 hours)*
-------
* In order to further test the Verlet algorithm, I wrote a program to simulate
  a ragdoll in 2D, which can be controlled by the mouse. In addition, there is
  collision detection using a technique found in a book (Graphics Gems) that
  uses maths which I learnt in FP4. I am very pleased with the outcome. This
  experiment can be found under `experiments/verletrag.html`.  
  ![Verlet integration on a ragdoll](https://raw.github.com/chengsun/ep/master/docs/images/300613_verletrag.jpg)
* Started writing May & June end-of-month review.

23/7/13 *(4 hours)*
-------
* Arrived home from summer school; work on EP restarts from today.
* Worked on OpenGL shader program and texture code. Not quite working yet.
* Looking to implement text output by tomorrow.

24/7/13 *(4 hours)*
-------
* Continued working on Texture code. Got texture code to work well, by using
  information from the [OpenGL wiki][opengl core-api].
* Text output is almost done, except for a bug in the texture upload, which
  causes the texture to be the wrong width. After an hour's worth of debugging
  I believe that this is caused by the graphics driver expecting a texture
  width that is divisible by 8, which I am not providing.
  I will have to change the code tomorrow to deal with this issue.

25/7/13 *(5 hours)*
-------
* Tracked down the bug where the texture would not render properly if its width
  was not divisible by 8. This was caused due to the stride not being equal to
  the number of pixels; there was padding on the end of each row to ensure that
  each row was aligned to 32 bytes. Fixed this by changing pixel access in
  SDL_Surfaces to `y*surf->pitch + x` rather than `y*surf->w + x`. Now all
  sizes of SDL_Surface work.
* Completed the text rendering code. An almost complete implementation of the
  method outlined in the [Valve paper][valve sdf-mag].
* Wrote a test to show off features of the text rendering:
    * High quality output
    * Small texture size (128x128)
    * Customisable boldness
    * Glow effect
    
  ![Rendering high-quality text with various effects](https://raw.github.com/chengsun/ep/master/docs/images/260713_text.png)

27/7/13 *(4 hours)*
-------
* Worked on PNG writer code. This will allow me to save a texture that is
  stored in memory into the hard drive for use later.
* Worked on implementing the framebuffer object (FBO). This will allow me not
  only to render to the screen, but also to an in-memory texture, which I can
  subsequently save using my PNG writing code, or use in a subsequent mesh.
* Wrote a test `test_rendertex` to test rendering text into a texture using a
  framebuffer object, and subsequently saving that texture using the PNG
  writer code. Currently the code is not working: random garbage gets stored in
  the texture.

28/7/13 *(4 hours)*
-------
* Implemented colour (and transparent) textures using vec4 to represent the
  RGBA values.
* Fixed and finished both the PNG writer code and the framebuffer code. The
  test works. Below shows the text as rendered into a square 128x128 texture.  
  ![Text as rendered into a texture](https://raw.github.com/chengsun/ep/master/docs/images/280713_rendertex.png)

13/08/13 *(4 hours)*
--------
* Implemented matrix stacks. The idea was based on this guide in the [OpenGL
  red book][opengl red matrix-stacks]. However I implemented it manually rather
  than using the OpenGL fixed functions.
* Implemented the perspective transform using the matrix stacks. Now 3D objects
  that are further away look smaller on the screen. I used [this question and
  answer site][stackoverflow glm-perspective] to help me understand and
  implement the transform.
* Tried drawing face indices onto the grid in order to help me debug the code.

17/08/13 *(3 hours)*
--------
* Started implementing creases into the mesh engine. This allows values (such
  as texture UV) to change discontinuously. I am using the approach described
  [in this blog post][ryg halfedge-redux]. It is not completely working.



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
[opengl red matrix-stacks]: http://www.glprogramming.com/red/chapter03.html#name6
[cprogramming quat]: http://www.cprogramming.com/tutorial/3d/quaternions.html
[navis 2007]: http://navis-asd.blogspot.co.uk/2010/04/2007-and-now.html
[hoetzlein surface-reconstruction]: http://www.rchoetzlein.com/theory/2010/surface-reconstruction-of-sph-fluids/
[gamedev water-effect]: http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/the-water-effect-explained-r915
[falstad ripple]: http://falstad.com/ripple/
[subprotocol verlet]: http://subprotocol.com/verlet-js/
[gamedev framebuffer101]: http://www.gamedev.net/page/resources/_/technical/opengl/opengl-frame-buffer-object-101-r2331
[sdltutorials sdl-ttf]: http://www.sdltutorials.com/sdl-ttf
[sdl-ttf docs]: http://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf.html
[valve sdf-mag]: http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
[glm]: http://glm.g-truc.net/
[democritis verlet]: http://www.compsoc.man.ac.uk/~lucky/Democritus/Theory/verlet.html#verlet
[stackoverflow glm-perspective]: http://stackoverflow.com/questions/8115352/glmperspective-explanation

<!-- vim: set tw=79: -->
