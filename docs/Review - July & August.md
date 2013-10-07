July/August end-of-month review
===

(written 4 September)

These two months have seen a large number of successes, which I am extremely
pleased about. I have also encountered several difficulties, which I am
confident about being able to resolve next month.

I am working very hard on the bridge simulation, which I intend to show off
most of the capabilities of the Verlet integration physics engine, as well as
being fun to play around with! Currently I have made it successfully simulate a
suspension cable material, which is flexible. In addition the length constraint
in the physics engine breaks the cable if excessive weight is applied.

Work still needs to be done on the simulation, however. Currently there is no
rigidity in the physics simulation, which means that no proper bridges can be
built yet, only flexible materials such as suspension cable can be simulated.
In order to make rigid materials such as metal beams I need to figure out how
to not only apply length constraints but also angle constraints. I am having a
hard time finding this information.

Overall, I am very happy with my progress -- it has mostly gone to plan, and
the results of the simulations.

In addition, I have successfully gotten Texture code to work. This is a large
part of OpenGL, and has been rather tricky to get working, mostly because
OpenGL has carried with it a lot of historical baggage which means that a lot
of functions need to be called in order to set up textures.



<!-- vim: set tw=79: -->
